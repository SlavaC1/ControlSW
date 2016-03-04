using BSS.Contracts;
using System;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    [DataContract]
    public class BatchStatistics : InternalObservableObject
    {
        #region Private Fields

        private DateTime? burnEndTime;

        private DateTime? burnStartTime;

        private ushort? currentSessionID;

        private bool hasReferenceTags;

        private uint lotBurnedCartridgesCount;

        private uint lotBurningAttemptsCount;

        private uint lotFailuresCount;

        private MaterialInfo materialInfo;

        private uint sessionBurnedCartridgesCount;

        private uint sessionBurningAttemptCount;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BatchStatistics"/> class.
        /// </summary>
        public BatchStatistics()
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the burn end time.
        /// </summary>
        /// <value>
        /// The burn end time.
        /// </value>
        [DataMember]
        public DateTime? BurnEndTime
        {
            get
            {
                return burnEndTime;
            }
            set
            {
                burnEndTime = value;
                RaisePropertyChanged(() => BurnEndTime);
            }
        }

        /// <summary>
        /// Gets or sets the burn start time.
        /// </summary>
        /// <value>
        /// The burn start time.
        /// </value>
        [DataMember]
        public DateTime? BurnStartTime
        {
            get
            {
                return burnStartTime;
            }
            set
            {
                burnStartTime = value;
                RaisePropertyChanged(() => BurnStartTime);
            }
        }

        /// <summary>
        /// Gets or sets the current session identifier.
        /// </summary>
        /// <value>
        /// The current session identifier.
        /// </value>
        [DataMember]
        public ushort? CurrentSessionID
        {
            get
            {
                return currentSessionID;
            }
            set
            {
                currentSessionID = value;
                RaisePropertyChanged(() => CurrentSessionID);
                RaisePropertyChanged(() => SessionBurningAttemptCount);
                RaisePropertyChanged(() => SessionBurnedCartridgesCount);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether any tag was burned as reference successfully in this Lot.
        /// </summary>
        /// <value>
        /// <c>true</c> if any tag was burned as reference successfully in this Lot; otherwise, <c>false</c>.
        /// </value>
        [DataMember]
        public virtual bool HasReferenceTags
        {
            get
            {
                return hasReferenceTags;
            }
            set
            {
                hasReferenceTags = value;
                RaisePropertyChanged(() => HasReferenceTags);
            }
        }

        /// <summary>
        /// Gets or sets the number of distinct tags which were burned successfully in this Lot.
        /// </summary>
        /// <value>
        /// The number of distinct tags which were burned successfully in this Lot.
        /// </value>
        [DataMember]
        public virtual uint LotBurnedCartridgesCount
        {
            get
            {
                return lotBurnedCartridgesCount;
            }
            set
            {
                lotBurnedCartridgesCount = value;
                RaisePropertyChanged(() => LotBurnedCartridgesCount);
            }
        }

        /// <summary>
        /// Gets or sets the number of total burning attemps in this Lot.
        /// </summary>
        /// <value>
        /// The number of total burning attemps in this Lot.
        /// </value>
        public virtual uint LotBurningAttemptsCount
        {
            get
            {
                return lotBurningAttemptsCount;
            }
            set
            {
                lotBurningAttemptsCount = value;
                RaisePropertyChanged(() => LotBurningAttemptsCount);
            }
        }

        /// <summary>
        /// Gets or sets the number of total burning failures in this Lot.
        /// </summary>
        /// <value>
        /// The number of total burning failures in this Lot.
        /// </value>
        [DataMember]
        public uint LotFailuresCount
        {
            get
            {
                return lotFailuresCount;
            }
            set
            {
                lotFailuresCount = value;
                RaisePropertyChanged(() => LotFailuresCount);
            }
        }

        /// <summary>
        /// Gets or sets the material information.
        /// </summary>
        /// <value>
        /// The material information.
        /// </value>
        [DataMember]
        public MaterialInfo MaterialInfo
        {
            get
            {
                return materialInfo;
            }
            set
            {
                materialInfo = value;
                RaisePropertyChanged(() => MaterialInfo);
            }
        }

        /// <summary>
        /// Gets or sets the number of distinct tags which were burned successfully in this session.
        /// </summary>
        /// <value>
        /// The number of distinct tags which were burned successfully in this session.
        /// </value>
        [DataMember]
        public virtual uint SessionBurnedCartridgesCount
        {
            get
            {
                return sessionBurnedCartridgesCount;
            }
            set
            {
                sessionBurnedCartridgesCount = value;
                RaisePropertyChanged(() => SessionBurnedCartridgesCount);
            }
        }

        /// <summary>
        /// Gets or sets the number of total burning attemps in this session.
        /// </summary>
        /// <value>
        /// The number of total burning attemps in this session.
        /// </value>
        [DataMember]
        public virtual uint SessionBurningAttemptCount
        {
            get
            {
                return sessionBurningAttemptCount;
            }
            set
            {
                sessionBurningAttemptCount = value;
                RaisePropertyChanged(() => SessionBurningAttemptCount);
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Determines whether Lot is closed.
        /// </summary>
        /// <returns></returns>
        public bool IsClosed()
        {
            return burnEndTime != null;
        }

        /// <summary>
        /// Determines whether Lot is new.
        /// </summary>
        /// <returns></returns>
        public bool IsNew()
        {
            return BurnStartTime == null;
        }

        /// <summary>
        /// Determines whether Lot is opened.
        /// </summary>
        /// <returns></returns>
        public bool IsOpened()
        {
            return BurnStartTime != null && BurnEndTime == null;
        }

        #endregion Public Methods
    }
}