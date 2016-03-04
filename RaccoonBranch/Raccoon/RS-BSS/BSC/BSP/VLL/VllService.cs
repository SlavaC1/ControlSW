using BSS.Contracts;
using CBS.BSP.DAL;
using CBS.BSP.HSM;
using CBS.BSP.Properties;
using CBS.VLL.Contracts;
using log4net;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;
using WcfInfras;

namespace CBS.BSP.VLL
{
    /// <summary>
    /// A service for loading VaultIC tags list into DB.
    /// </summary>
    [ServiceBehavior(IncludeExceptionDetailInFaults = DebugHelper.IncludeExceptionDetailInFaults)]
    public class VllService : BspService, IVll
    {
        #region Private Fields

        /// <summary>
        /// The HSM service
        /// </summary>
        private HsmService hsmService;

        /// <summary>
        /// The RTDB access layer.
        /// </summary>
        private RtDal rtDal;

        /// <summary>
        /// Holds the public key element for the VLL.
        /// </summary>
        private byte[] publicKeyIS;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="VllService"/> class.
        /// </summary>
        public VllService()
        {
            Logger = LogManager.GetLogger(typeof(VllService));
            OperationContext currentOperationContext = OperationContext.Current;
            if (currentOperationContext != null)
            {
                Logger.Info(Resources.VllStarted);
            }

            hsmService = new HsmService();
            rtDal = new RtDal();

            try
            {
                string codeBase = Assembly.GetExecutingAssembly().CodeBase;
                UriBuilder uri = new UriBuilder(codeBase);
                string path = Uri.UnescapeDataString(uri.Path);
                path = Path.GetDirectoryName(path);
                using (StreamReader fs = new StreamReader(Path.Combine(path, "InsidePublicKeyHexa.txt")))
                {
                    publicKeyIS = fs.ReadToEnd().Trim().Replace(" ", String.Empty).ToByteArray();
                }
            }
            catch (Exception ex)
            {
                Logger.Error(Resources.CannotLoadISPublicKey, ex);
            }
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Loads the content of the VaultIC tags list into DB.
        /// </summary>
        /// <param name="name">List name. Used for logging purposes only.</param>
        /// <param name="content">The content of the VaultIC tags list.</param>
        public async Task LoadVaultICListFileAsync(string name, string content)
        {
            try
            {
                Logger.InfoFormat(Resources.LoadingList, name);

                IVllCallback vllCallback = OperationContext.Current.GetCallbackChannel<IVllCallback>();
                vllCallback.ReportProgress(null, 0, 0);

                TagEqualityComparer tagEqualityComparer = new TagEqualityComparer();
                IEnumerable<Tag> tagsToLoad = await ParseTags(content, vllCallback);
                int listCount = tagsToLoad.Count();
                vllCallback.ReportProgress(null, 0, listCount);

                var allTagsInDb = rtDal.GetAllTags();
                var conflictedTags = allTagsInDb.Intersect(tagsToLoad, tagEqualityComparer);

                int newTagsCount = conflictedTags.Count(tag => tag.Status == (byte)TagRegistration.New);
                int usedTagsCount = conflictedTags.Count(tag => tag.Status == (byte)TagRegistration.Used);

                tagsToLoad = tagsToLoad.Except(conflictedTags, tagEqualityComparer);
                int numberOfTagsToLoad = tagsToLoad.Count();

                foreach (Tag tag in conflictedTags)
                {
                    Logger.InfoFormat(Resources.TagFoundInDB, tag.SerialNumber.ToArray().ToHexString(), (TagRegistration)tag.Status);
                }

                int index = 1;
                foreach (var tag in tagsToLoad)
                {
                    Logger.InfoFormat(Resources.LoadingTagToDB, tag.SerialNumber.ToArray().ToHexString(), tag.PublicKey.ToArray().ToHexString());
                    rtDal.LoadTag(tag);
                    vllCallback.ReportProgress(tag.SerialNumber.ToArray().ToHexString(), index++, numberOfTagsToLoad);
                }

                //rtDal.LoadTags(tagsToLoad);
                vllCallback.OnLoadingCompleted(
                    listCount,
                    numberOfTagsToLoad,
                    newTagsCount,
                    usedTagsCount);
            }
            catch (Exception ex)
            {
                ExceptionDetail detail = new ExceptionDetail(ex);
                throw new FaultException<ExceptionDetail>(detail, ex.Message);
            }
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Converts the string representation of a single VaultIC tag into a tag record.
        /// </summary>
        /// <param name="contentLine">The content line.</param>
        /// <param name="vllCallback"></param>
        /// <param name="index">The line index.</param>
        /// <param name="linesCount">Total number of lines.</param>
        /// <returns></returns>
        private Tag ParseTag(string contentLine, IVllCallback vllCallback, int index, int linesCount)
        {
            string[] tokens = contentLine.Split(new char[] { ',', ' ' }, StringSplitOptions.None);
            Tag tag = new Tag
            {
                Date = DateTime.Parse(tokens[0], CultureInfo.InvariantCulture),
                SerialNumber = tokens[1].ToByteArray(),
                PublicKey = tokens[2].ToByteArray(),
                Status = (byte)TagRegistration.New,
            };

            byte[] message = tag.Encode();
            tag.Signature = hsmService.SignData(message);
            vllCallback.ReportProgress(tokens[1], index, linesCount);

            return tag;
        }

        /// <summary>
        /// Converts the string representation of the VaultIC tags list into tag records.
        /// </summary>
        /// <param name="content">The content of the VaultIC tags list.</param>
        /// <returns></returns>
        /// <exception cref="System.FaultException{System.ServiceModel.ExceptionDetail}">
        /// content is null
        /// or
        /// invalid VLL
        /// </exception>
        private async Task<Tag[]> ParseTags(string content, IVllCallback vllCallback)
        {
            if (content == null)
            {
                ArgumentNullException exception = new ArgumentNullException("content");
                ExceptionDetail detail = new ExceptionDetail(exception);
                throw new FaultException<ExceptionDetail>(detail, exception.Message);
            }

            if (publicKeyIS == null)
            {
                InvalidOperationException exception = new InvalidOperationException("public key is not set");
                ExceptionDetail detail = new ExceptionDetail(exception);
                throw new FaultException<ExceptionDetail>(detail, exception.Message);
            }

            string[] lines = content.Split(new string[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries);
            if (lines.Length <= 1)
            {
                ArgumentException exception = new ArgumentException("invalid VLL", "content");
                ExceptionDetail detail = new ExceptionDetail(exception);
                throw new FaultException<ExceptionDetail>(detail, exception.Message);
            }

            string lastLine = lines.Last();
            Array.Resize(ref lines, lines.Length - 1);
            byte[] signature = lastLine.Split(new char[] { ',', ' ' }, StringSplitOptions.None)[2].ToByteArray();

            string messageStr = String.Join("\n", lines) + "\n";
            byte[] message = Encoding.UTF8.GetBytes(messageStr);
            bool isValid = await hsmService.VerifySignatureAsync(publicKeyIS, message, signature);
            if (!isValid)
            {
                ArgumentException exception = new ArgumentException("invalid VLL signature", "content");
                ExceptionDetail detail = new ExceptionDetail(exception);
                throw new FaultException<ExceptionDetail>(detail, exception.Message);
            }

            var tags = lines
                .Select((line, index) => ParseTag(line, vllCallback, index, lines.Length))
                .ToArray();

            return tags;
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected override void Dispose(bool dispose)
        {
            if (dispose)
            {
                if (rtDal != null)
                {
                    rtDal.Dispose();
                }

                if (hsmService != null)
                {
                    hsmService.Dispose();
                }

                Logger.Info(Resources.VllEnded);
            }
        }

        #endregion IDisposable Members

        #region Private Classes

        /// <summary>
        /// An equality comparer for comparing bytes of buffers sequentially.
        /// </summary>
        private class TagEqualityComparer : IEqualityComparer<Tag>
        {
            #region Public Methods

            /// <summary>
            /// Determines whether the specified objects are equal.
            /// </summary>
            /// <param name="x">The first object of type <paramref name="T" /> to compare.</param>
            /// <param name="y">The second object of type <paramref name="T" /> to compare.</param>
            /// <returns>
            /// true if the specified objects are equal; otherwise, false.
            /// </returns>
            public bool Equals(Tag x, Tag y)
            {
                if (x == null)
                {
                    return y == null;
                }

                if (y == null)
                {
                    return false;
                }

                return x.SerialNumber.ToArray().SequenceEqual(y.SerialNumber.ToArray());
            }

            /// <summary>
            /// Returns a hash code for this instance.
            /// </summary>
            /// <param name="obj">The object.</param>
            /// <returns>
            /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
            /// </returns>
            public int GetHashCode(Tag obj)
            {
                if (obj == null)
                {
                    return 0;
                }

                return obj.SerialNumber.ToArray().Aggregate(
                    0,
                    (code, item) => code ^ item.GetHashCode());
            }

            #endregion Public Methods
        }

        #endregion Private Classes
    }
}