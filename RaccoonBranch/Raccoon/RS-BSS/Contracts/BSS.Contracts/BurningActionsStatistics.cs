using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    [DataContract]
    public class BurningActionsStatistics : BatchStatistics
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BurningActionsStatistics"/> class.
        /// </summary>
        public BurningActionsStatistics()
        {
            Tags = new ObservableCollection<TagStatistics>();
            Tags.CollectionChanged += (sender, e) =>
            {
                RefreshCounters();
                if (e.NewItems != null)
                {
                    foreach (TagStatistics ts in e.NewItems)
                    {
                        ts.PropertyChanged += HandleTagPropertyChanged;
                    }
                }
                if (e.OldItems != null)
                {
                    foreach (TagStatistics ts in e.NewItems)
                    {
                        ts.PropertyChanged -= HandleTagPropertyChanged;
                    }
                }
            };
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether any tag was burned as reference successfully in this Lot.
        /// </summary>
        /// <value>
        /// <c>true</c> if any tag was burned as reference successfully in this Lot; otherwise, <c>false</c>.
        /// </value>
        [DataMember]
        public override bool HasReferenceTags
        {
            get
            {
                return Tags.Any(tag => tag.IsReference && String.IsNullOrEmpty(tag.LastError));
            }
        }

        /// <summary>
        /// Gets the number of distinct tags which were burned successfully in this Lot.
        /// </summary>
        /// <value>
        /// The number of distinct tags which were burned successfully in this Lot.
        /// </value>
        [DataMember]
        public override uint LotBurnedCartridgesCount
        {
            get
            {
                return (uint)Tags.Count(tag => String.IsNullOrEmpty(tag.LastError));
            }
        }

        /// <summary>
        /// Gets the number of total burning attemps in this Lot.
        /// </summary>
        /// <value>
        /// The number of total burning attemps in this Lot.
        /// </value>
        [DataMember]
        public override uint LotBurningAttemptsCount
        {
            get
            {
                if (!Tags.Any())
                {
                    return 0;
                }

                return (uint)Tags.Sum(tagStatistics => tagStatistics.BurningAttemptCount);
            }
        }

        /// <summary>
        /// Gets the number of distinct tags which were burned successfully in this session.
        /// </summary>
        /// <value>
        /// The number of distinct tags which were burned successfully in this session.
        /// </value>
        [DataMember]
        public override uint SessionBurnedCartridgesCount
        {
            get
            {
                return (uint)Tags.Count(tag =>
                    tag.SessionID == CurrentSessionID &&
                    String.IsNullOrEmpty(tag.LastError));
            }
        }

        /// <summary>
        /// Gets the number of total burning attemps in this session.
        /// </summary>
        /// <value>
        /// The number of total burning attemps in this session.
        /// </value>
        [DataMember]
        public override uint SessionBurningAttemptCount
        {
            get
            {
                return (uint)Tags
                    .Where(tag => tag.SessionID == CurrentSessionID)
                    .Sum(tag => tag.BurningAttemptCount);
            }
        }

        /// <summary>
        /// Gets the collection of tags burning statistics.
        /// </summary>
        /// <value>
        /// The collection of tags burning statistics.
        /// </value>
        [DataMember]
        public ObservableCollection<TagStatistics> Tags
        {
            get;
            private set;
        }

        #endregion Public Properties

        #region Private Methods

        private void HandleTagPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName.EndsWith("Count") || e.PropertyName.EndsWith("HasReferenceTags"))
            {
                RefreshCounters();
            }
        }

        /// <summary>
        /// Refreshes the counters.
        /// </summary>
        private void RefreshCounters()
        {
            RaisePropertyChanged(() => LotBurnedCartridgesCount);
            RaisePropertyChanged(() => LotFailuresCount);
            RaisePropertyChanged(() => SessionBurnedCartridgesCount);
            RaisePropertyChanged(() => SessionBurningAttemptCount);
            RaisePropertyChanged(() => LotBurningAttemptsCount);
            RaisePropertyChanged(() => HasReferenceTags);
        }

        #endregion Private Methods
    }
}