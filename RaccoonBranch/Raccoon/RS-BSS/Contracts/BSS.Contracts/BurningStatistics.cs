using System;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    [DataContract]
    public class BurningStatistics : InternalObservableObject, ICloneable
    {
        #region Private Fields

        /// <summary>
        /// The lot counters.
        /// </summary>
        private BurnCounters lotCounters;

        /// <summary>
        /// The material information.
        /// </summary>
        private MaterialInfo materialInfo;

        /// <summary>
        /// The session counters
        /// </summary>
        private BurnCounters sessionCounters;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BurningStatistics"/> class.
        /// </summary>
        /// <param name="materialInfo">The material information.</param>
        /// <exception cref="System.ArgumentNullException">materialInfo</exception>
        public BurningStatistics(MaterialInfo materialInfo)
        {
            if (materialInfo == null)
            {
                throw new ArgumentNullException("materialInfo");
            }

            this.materialInfo = materialInfo;
            this.lotCounters = new BurnCounters();
            this.sessionCounters = new BurnCounters();
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the lot counters.
        /// </summary>
        /// <value>
        /// The lot counters.
        /// </value>
        /// <exception cref="System.ArgumentNullException">value</exception>
        [DataMember]
        public BurnCounters LotCounters
        {
            get
            {
                return lotCounters;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                lotCounters = value;
                RaisePropertyChanged(() => LotCounters);
            }
        }

        /// <summary>
        /// Gets or sets the material info.
        /// </summary>
        /// <value>
        /// The material info.
        /// </value>
        [DataMember]
        public MaterialInfo MaterialInfo
        {
            get
            {
                return materialInfo;
            }
        }

        /// <summary>
        /// Gets or sets the session counters.
        /// </summary>
        /// <value>
        /// The session counters.
        /// </value>
        /// <exception cref="System.ArgumentNullException">value</exception>
        [DataMember]
        public BurnCounters SessionCounters
        {
            get
            {
                return sessionCounters;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                sessionCounters = value;
                RaisePropertyChanged(() => SessionCounters);
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Starts a new burning session.
        /// </summary>
        public void StartSession()
        {
            SessionCounters = new BurnCounters();
        }

        /// <summary>
        /// Ends current burning session.
        /// </summary>
        public void EndSession()
        {
            SessionCounters = null;
        }

        /// <summary>
        /// Writes the burn.
        /// </summary>
        /// <param name="successfull">if set to <c>true</c>, burn is successfull.</param>
        public void WriteBurn(bool successfull)
        {
            if (SessionCounters == null)
            {
                //throw new InvalidOperationException(Resources.SessionNotStarted);
            }

            LotCounters.Total++;
            SessionCounters.Total++;
            if (successfull)
            {
                LotCounters.Successfull++;
                SessionCounters.Successfull++;
            }
        }

        #endregion Public Methods

        #region IClonable Members

        /// <summary>
        /// Creates a new object that is a copy of the current instance.
        /// </summary>
        /// <returns>
        /// A new object that is a copy of this instance.
        /// </returns>
        public object Clone()
        {
            BurningStatistics cloned = new BurningStatistics((MaterialInfo)materialInfo.Clone());
            cloned.lotCounters = lotCounters;
            cloned.sessionCounters = sessionCounters;
            return cloned;
        }

        #endregion IClonable Members

        #region Public Classes

        public class BurnCounters : InternalObservableObject
        {
            #region Private Fields

            private uint successfull;
            private uint total;

            #endregion Private Fields

            #region Public Properties

            public uint Successfull
            {
                get
                {
                    return successfull;
                }
                set
                {
                    successfull = value;
                    RaisePropertyChanged(() => Successfull);
                }
            }

            public uint Total
            {
                get
                {
                    return total;
                }
                set
                {
                    total = value;
                    RaisePropertyChanged(() => Total);
                }
            }

            #endregion Public Properties
        }

        #endregion Public Classes
    }
}