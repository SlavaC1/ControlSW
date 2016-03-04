using BSS.Contracts;
using CBS.BSP.HSM;
using CBS.BSP.Properties;
using Hsm.Contracts;
using log4net;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Data.Linq;
using System.Linq;
using System.Threading.Tasks;

namespace CBS.BSP.DAL
{
    /// <summary>
    /// The Raccoon tags DB access layer.
    /// </summary>
    public class RtDal : IDisposable
    {
        #region Private Fields

        private readonly ILog logger;

        /// <summary>
        /// The VLL database.
        /// </summary>
        private RaccoonTagsDataContext rtdb;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="RtDal"/> class.
        /// </summary>
        public RtDal()
        {
            logger = LogManager.GetLogger("BspService");

            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                string connectionString = config.ConnectionStrings.ConnectionStrings["CBS.BSP.Properties.Settings.RTDBConnectionString"].ConnectionString;
                rtdb = new RaccoonTagsDataContext(connectionString);
            }
            catch (Exception ex)
            {
                logger.Warn(Resources.ErrorReadingConfig, ex);
            }
        }

        #endregion Public Constructors

        #region Public Methods

        public async Task EndLotAsync(string batchNumber)
        {
            var batch = await FindBatchAsync(batchNumber);
            if (batch == null)
            {
                throw new RowNotInTableException();
            }

            batch.BurnEndTime = DateTime.Now;

            logger.Debug(Resources.UpdatingDB);
            rtdb.SubmitChanges();
        }

        public async Task<Batch> FindBatchAsync(string batchNumber)
        {
            return await Task.Run(() =>
                {
                    return rtdb.Batches.FirstOrDefault(batch => batch.BatchNumber == batchNumber);
                });
        }

        /// <summary>
        /// Gets the tag record from DB given its serial number.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns>The tag object.</returns>
        public async Task<Tag> FindTagAsync(byte[] serialNumber)
        {
            return await Task<Tag>.Run(() =>
                {
                    Binary binSerialNumber = new Binary(serialNumber);
                    return rtdb.Tags.FirstOrDefault(tag => tag.SerialNumber.Equals(binSerialNumber));
                });
        }

        public IEnumerable<Tag> GetAllTags()
        {
            return rtdb.Tags;
        }

        public async Task<BatchStatistics> GetLotAsync(string batchNumber)
        {
            Batch batch = await FindBatchAsync(batchNumber);
            if (batch == null)
            {
                return null;
            }

            return new BatchStatistics
            {
                BurnStartTime = batch.BurnStartTime,
                MaterialInfo = new MaterialInfo
                {
                    TagStructID = (ushort)batch.IdtVersion,
                    MaterialID = (ushort)batch.MaterialID,
                    MaterialName = batch.Material.Name,
                    MaterialKind = (MaterialKind)batch.MaterialKind,
                    ManufacturingTime = batch.ManufacturingDate,
                    BatchNumber = batch.BatchNumber.Trim(),
                    ExpirationDate = batch.ExpirationDate,
                    InitialWeight = (ushort)batch.InitialWeight,
                    ExtraWeight = (ushort)batch.ExtraWeight,
                },
                HasReferenceTags = rtdb.spHasReferenceTags(batchNumber).First().Column1.Value,
                LotBurnedCartridgesCount = (uint)rtdb.spBurnedTagsCount(batchNumber).First().Column1.Value,
                LotBurningAttemptsCount = (uint)rtdb.spBurningAttemptsCount(batchNumber).First().Column1.Value,
                LotFailuresCount = (uint)rtdb.spFailuresCount(batchNumber).First().Column1.Value,
            };
        }

        /// <summary>
        /// Inserts the burning action asynchronous.
        /// </summary>
        /// <param name="tagStatistics">The tag statistics.</param>
        /// <param name="hsmService">The HSM service.</param>
        /// <returns></returns>
        public async Task InsertBurningActionAsync(TagStatistics tagStatistics, IHsm hsmService)
        {
            byte[] message;

            var batchRecord = await FindBatchAsync(tagStatistics.TagInfo.MaterialInfo.BatchNumber);
            if (batchRecord == null)
            {
                batchRecord = new Batch
                {
                    BatchNumber = tagStatistics.BatchStatistics.MaterialInfo.BatchNumber,
                    BurnStartTime = tagStatistics.BatchStatistics.BurnStartTime,
                    BurnEndTime = tagStatistics.BatchStatistics.BurnEndTime,
                    IdtVersion = (short)tagStatistics.BatchStatistics.MaterialInfo.TagStructID,
                    MaterialID = (short)tagStatistics.BatchStatistics.MaterialInfo.MaterialID,
                    MaterialKind = (byte)tagStatistics.BatchStatistics.MaterialInfo.MaterialKind,
                    ManufacturingDate = tagStatistics.BatchStatistics.MaterialInfo.ManufacturingTime,
                    ExpirationDate = tagStatistics.BatchStatistics.MaterialInfo.ExpirationDate,
                    InitialWeight = (short)tagStatistics.BatchStatistics.MaterialInfo.InitialWeight,
                    ExtraWeight = (short)tagStatistics.BatchStatistics.MaterialInfo.ExtraWeight,
                };

                message = batchRecord.Encode();
                batchRecord.Signature = await hsmService.SignDataAsync(message);

                rtdb.Batches.InsertOnSubmit(batchRecord);
            }

            var burningActionRecord = new BurningAction
            {
                Id = rtdb.BurningActions.Count(),
                SerialNumber = tagStatistics.TagInfo.SerialNumber,
                StationName = tagStatistics.StationName,
                BurningTime = tagStatistics.BurningTime,
                IsReference = tagStatistics.IsReference,
                ErrorMessage = tagStatistics.LastError,
                Batch = batchRecord,
            };

            message = burningActionRecord.Encode();
            burningActionRecord.Signature = await hsmService.SignDataAsync(message);

            logger.Debug(Resources.UpdatingDB);
            rtdb.BurningActions.InsertOnSubmit(burningActionRecord);
            rtdb.SubmitChanges();
        }

        public bool IsConnected()
        {
            return IsConnected(rtdb.Connection.State);
        }

        /// <summary>
        /// Loads a tag record into DB.
        /// </summary>
        /// <param name="tags">The tag record.</param>
        public void LoadTag(Tag tag)
        {
            logger.DebugFormat(Resources.InsertingTagToDB, tag.SerialNumber.ToArray().ToHexString());
            rtdb.Tags.InsertOnSubmit(tag);
            rtdb.SubmitChanges();
        }

        /// <summary>
        /// Loads tag records into DB.
        /// </summary>
        /// <param name="tags">The tag records.</param>
        public void LoadTags(IEnumerable<Tag> tags)
        {
            rtdb.Tags.InsertAllOnSubmit(tags);
            rtdb.SubmitChanges();
        }

        /// <summary>
        /// Reads the batch history from RTDB.
        /// </summary>
        /// <param name="batchNumber">The batch number.</param>
        /// <returns></returns>
        public async Task<BurningActionsStatistics> ReadBatchHistoryAsync(string batchNumber, IHsm hsmService)
        {
            Batch batchRow = await FindBatchAsync(batchNumber);
            if (batchRow == null)
            {
                return null;
            }

            var burningActionsStatistics = new BurningActionsStatistics
            {
                BurnStartTime = batchRow.BurnStartTime,
                BurnEndTime = batchRow.BurnEndTime,
                MaterialInfo = new MaterialInfo
                {
                    TagStructID = (ushort)batchRow.IdtVersion,
                    MaterialID = (ushort)batchRow.MaterialID,
                    MaterialName = batchRow.Material.Name,
                    MaterialKind = (MaterialKind)batchRow.MaterialKind,
                    ManufacturingTime = batchRow.ManufacturingDate,
                    BatchNumber = batchRow.BatchNumber.Trim(),
                    ExpirationDate = batchRow.ExpirationDate,
                    InitialWeight = (ushort)batchRow.InitialWeight,
                    ExtraWeight = (ushort)batchRow.ExtraWeight,
                },
                HasReferenceTags = rtdb.spHasReferenceTags(batchNumber).First().Column1.Value,
                LotBurnedCartridgesCount = (uint)rtdb.spBurnedTagsCount(batchNumber).First().Column1.Value,
                LotBurningAttemptsCount = (uint)rtdb.spBurningAttemptsCount(batchNumber).First().Column1.Value,
                LotFailuresCount = (uint)rtdb.spFailuresCount(batchNumber).First().Column1.Value,
            };

            var burningActionByTag = batchRow.BurningActions
                .GroupBy(ba => ba.SerialNumber.ToArray(), new BufferEqualityComparer());

            var tagStatisticsCreationTasks = burningActionByTag.Select(baGroup =>
                {
                    DateTime latestBurnTime = baGroup.Max(ba => ba.BurningTime);
                    var mostRecentBurningAction = baGroup.FirstOrDefault(ba => ba.BurningTime == latestBurnTime);
                    return CreateTagStatistics(mostRecentBurningAction, (ushort)baGroup.Count(), burningActionsStatistics.MaterialInfo, hsmService);
                });

            foreach (var tagStatisticsCreationTask in tagStatisticsCreationTasks)
            {
                var tagStatistics = await tagStatisticsCreationTask;
                burningActionsStatistics.Tags.Add(tagStatistics);
            }

            return burningActionsStatistics;
        }

        /// <summary>
        /// Reads the batch numbers.
        /// </summary>
        /// <returns></returns>
        public Task<string[]> ReadBatchNumbersAsync()
        {
            return Task<string[]>.Run(() =>
                {
                    return rtdb.Batches
                        .Select(batch => batch.BatchNumber)
                        .ToArray();
                });
        }

        /// <summary>
        /// Reads the material name.
        /// </summary>
        /// <param name="materialID">The material ID.</param>
        /// <returns></returns>
        public Task<string> ReadMaterialNameAsync(ushort materialID)
        {
            return Task<string>.Run(() =>
                {
                    var material = rtdb.Materials.FirstOrDefault(m => m.ID == materialID);
                    if (material == null)
                    {
                        return String.Empty;
                    }

                    return material.Name;
                });
        }

        /// <summary>
        /// Updates the tag record in DB given its serial number.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <param name="tagRegistration">The new tag registration.</param>
        public async Task<bool> UpdateAndSignTagAsync(byte[] serialNumber, TagRegistration tagRegistration, HsmService hsmService)
        {
            if (serialNumber == null)
            {
                throw new ArgumentNullException("serialNumber");
            }

            Tag tag = await FindTagAsync(serialNumber);
            if (tag == null)
            {
                throw new RowNotInTableException(String.Format(Resources.TagNotFound, serialNumber.ToHexString()));
            }

            tag.Status = (byte)tagRegistration;
            tag.LastUpdateTime = DateTime.Now;
            byte[] message = tag.Encode();
            tag.Signature = await hsmService.SignDataAsync(message);
            if (tag.Signature == null)
            {
                return false;
            }

            logger.Debug(Resources.UpdatingDB);
            rtdb.SubmitChanges();
            return true;
        }

        #endregion Public Methods

        #region Private Methods

        private async Task<TagStatistics> CreateTagStatistics(BurningAction burningAction, ushort burningAttemptsCount, MaterialInfo materialInfo, IHsm hsmService)
        {
            TagInfo tagInfo = new TagInfo
            {
                MaterialInfo = materialInfo,
                CurrentMaterialWeight = materialInfo.GetTotalWeight(),
                SerialNumber = burningAction.SerialNumber.ToArray(),
            };

            TagStatistics tagStatistics = new TagStatistics(tagInfo);
            tagStatistics.StationName = burningAction.StationName.Trim();
            tagStatistics.IsReference = burningAction.IsReference;
            tagStatistics.BurningTime = burningAction.BurningTime;
            tagStatistics.BurningAttemptCount = burningAttemptsCount;
            if (String.IsNullOrEmpty(burningAction.ErrorMessage))
            {
                tagStatistics.LastError = null;
            }
            else
            {
                tagStatistics.LastError = burningAction.ErrorMessage.Trim();
            }

            byte[] message = burningAction.Encode();
            byte[] signature = burningAction.Signature.ToArray();
            bool isVerified = await hsmService.VerifySignatureAsync(message, signature);
            tagStatistics.HasInvalidSignature = !isVerified;

            return tagStatistics;
        }

        private bool IsConnected(ConnectionState connectionState)
        {
            return connectionState == ConnectionState.Open ||
                connectionState == ConnectionState.Executing ||
                connectionState == ConnectionState.Fetching;
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                if (rtdb != null)
                {
                    rtdb.Dispose();
                }
            }
        }

        #endregion IDisposable Members

        #region Private Classes

        /// <summary>
        /// An equality comparer for comparing bytes of buffers sequentially.
        /// </summary>
        private class BufferEqualityComparer : IEqualityComparer<byte[]>
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
            public bool Equals(byte[] x, byte[] y)
            {
                if (x == null)
                {
                    return y == null;
                }

                if (y == null)
                {
                    return false;
                }

                return x.SequenceEqual(y);
            }

            /// <summary>
            /// Returns a hash code for this instance.
            /// </summary>
            /// <param name="obj">The object.</param>
            /// <returns>
            /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
            /// </returns>
            public int GetHashCode(byte[] obj)
            {
                if (obj == null)
                {
                    return 0;
                }

                return obj.Aggregate(
                    0,
                    (code, item) => code ^ item.GetHashCode());
            }

            #endregion Public Methods
        }

        #endregion Private Classes
    }
}