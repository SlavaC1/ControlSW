using System;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    [DataContract]
    public class TagStatistics : InternalObservableObject
    {
        #region Private Fields

        private BatchStatistics batchStatistics;

        private ushort burningAttemptCount;

        private DateTime burningTime;

        private bool hasInvalidSignature;

        private bool isReference;

        private string lastError;

        private ushort sessionID;

        private string stationName;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="TagStatistics"/> class.
        /// </summary>
        /// <param name="tagInfo">The tag information.</param>
        /// <exception cref="System.ArgumentNullException">tagInfo</exception>
        public TagStatistics(TagInfo tagInfo)
        {
            if (tagInfo == null)
            {
                throw new ArgumentNullException("tagInfo");
            }

            this.TagInfo = tagInfo;
        }

        #endregion Public Constructors

        #region Public Properties

        [DataMember]
        public BatchStatistics BatchStatistics
        {
            get
            {
                return batchStatistics;
            }
            set
            {
                batchStatistics = value;
                RaisePropertyChanged(() => BatchStatistics);
            }
        }

        [DataMember]
        public ushort BurningAttemptCount
        {
            get
            {
                return burningAttemptCount;
            }
            set
            {
                burningAttemptCount = value;
                RaisePropertyChanged(() => BurningAttemptCount);
            }
        }

        /// <summary>
        /// Gets or sets the burning time.
        /// </summary>
        /// <value>
        /// The burning time.
        /// </value>
        [DataMember]
        public DateTime BurningTime
        {
            get
            {
                return burningTime;
            }
            set
            {
                burningTime = value;
                RaisePropertyChanged(() => BurningTime);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether record has an invalid signature in DB.
        /// </summary>
        /// <value>
        /// <c>true</c> if record has an invalid signature; otherwise, <c>false</c>.
        /// </value>
        [DataMember]
        public bool HasInvalidSignature
        {
            get
            {
                return hasInvalidSignature;
            }
            set
            {
                hasInvalidSignature = value;
                RaisePropertyChanged(() => HasInvalidSignature);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether this burning is a reference burning.
        /// </summary>
        /// <value>
        /// <c>true</c> if this burning is a reference burning; otherwise, <c>false</c>.
        /// </value>
        [DataMember]
        public bool IsReference
        {
            get
            {
                return isReference;
            }
            set
            {
                isReference = value;
                RaisePropertyChanged(() => IsReference);
            }
        }

        /// <summary>
        /// Gets or sets the last error.
        /// </summary>
        /// <value>
        /// The last error.
        /// </value>
        [DataMember]
        public string LastError
        {
            get
            {
                return lastError;
            }
            set
            {
                lastError = value;
                RaisePropertyChanged(() => LastError);
            }
        }

        /// <summary>
        /// Gets or sets the session identifier.
        /// </summary>
        /// <value>
        /// The session identifier.
        /// </value>
        [DataMember]
        public ushort SessionID
        {
            get
            {
                return sessionID;
            }
            set
            {
                sessionID = value;
                RaisePropertyChanged(() => SessionID);
            }
        }

        /// <summary>
        /// Gets or sets the burning station name or ID.
        /// </summary>
        /// <value>
        /// The burning station name or ID.
        /// </value>
        [DataMember]
        public string StationName
        {
            get
            {
                return stationName;
            }
            set
            {
                stationName = value;
                RaisePropertyChanged(() => StationName);
            }
        }

        /// <summary>
        /// Gets the tag information.
        /// </summary>
        /// <value>
        /// The tag information.
        /// </value>
        [DataMember]
        public TagInfo TagInfo
        {
            get;
            private set;
        }

        #endregion Public Properties
    }
}