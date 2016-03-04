using CBS.BSP.Properties;
using log4net;
using System;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Security;
using System.ServiceModel;

namespace CBS.BSP
{
    /// <summary>
    /// Holds common functionality for all BSP services.
    /// </summary>
    public abstract class BspService : IDisposable
    {
        #region Private Fields

        /// <summary>
        /// The burrning software users manager.
        /// </summary>
        private BssUsersManager bsUsersManager;

        #endregion Private Fields

        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BspService"/> class.
        /// </summary>
        protected BspService()
        {
            Logger = LogManager.GetLogger(typeof(BspService));
            string bsaUserNamesRaw = String.Empty;

            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement = config.AppSettings.Settings["OracleFilePath"];
                configElement = config.AppSettings.Settings["BsaUserNames"];
                if (configElement != null)
                {
                    bsaUserNamesRaw = configElement.Value;
                }
            }
            catch (Exception ex)
            {
                Logger.Warn(Resources.ErrorReadingConfig, ex);
            }

            string[] bsaUserNames = bsaUserNamesRaw.Split(',')
                .Select(token => Environment.UserDomainName + Path.DirectorySeparatorChar + token)
                .ToArray();

            bsUsersManager = new BssUsersManager(bsaUserNames);
        }

        #endregion Protected Constructors

        #region Protected Properties

        /// <summary>
        /// Gets the logger.
        /// </summary>
        /// <value>
        /// The logger.
        /// </value>
        protected ILog Logger
        {
            get;
            set;
        }

        #endregion Protected Properties

        #region Protected Methods

        /// <summary>
        /// Checks if user is allowed to access service.
        /// </summary>
        protected void CheckUserName()
        {
            if (OperationContext.Current == null)
            {
                // called internally, no need to check
                return;
            }

            string userName = OperationContext.Current.ServiceSecurityContext.PrimaryIdentity.Name;
            bool userFound = false;
            Exception bsaException = null;
            try
            {
                userFound = bsUsersManager.CheckUserName(userName);
            }
            catch (Exception ex)
            {
                bsaException = ex;
            }

            if (!userFound)
            {
                ThrowFaultException(new SecurityException(
                    String.Format(Resources.UserNotAllowed, userName),
                    bsaException));
            }
        }

        /// <summary>
        /// Throws a fault exception.
        /// </summary>
        /// <param name="ex">The exception.</param>
        /// <exception cref="FaultException{ExceptionDetail}"></exception>
        protected void ThrowFaultException(Exception ex)
        {
            Logger.Error(Resources.ErrorOccurred, ex);
            ExceptionDetail detail = new ExceptionDetail(ex);
            string message = ex.Message;
            AggregateException agex = ex as AggregateException;
            if (agex != null)
            {
                message = String.Join(Environment.NewLine, agex.InnerExceptions.Select(innerEx => innerEx.Message));
            }

            throw new FaultException<ExceptionDetail>(detail, message);
        }

        #endregion Protected Methods

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
            // do nothing
        }

        #endregion IDisposable Members
    }
}