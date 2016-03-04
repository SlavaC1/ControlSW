using BSS.App.ViewModel;
using GalaSoft.MvvmLight.Threading;
using log4net.Appender;
using log4net.Core;
using Microsoft.Practices.ServiceLocation;
using System;
using System.Collections;

namespace BSS.App.Appenders
{
    internal class LogViewModelAppender : AppenderSkeleton
    {
        #region Private Fields

        private IList target;

        #endregion Private Fields

        #region Public Properties

        /// <summary>
        /// Gets or sets the log target.
        /// </summary>
        /// <value>
        /// The log target.
        /// </value>
        /// <exception cref="System.ArgumentNullException">value</exception>
        public IList Target
        {
            get
            {
                if (target == null)
                {
                    target = ServiceLocator.Current.GetInstance<MainViewModel>().LogViewModel.Log;
                }

                return target;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                target = value;
            }
        }

        #endregion Public Properties

        #region Protected Methods

        /// <summary>
        /// Performs actual logging.
        /// </summary>
        /// <param name="loggingEvent">The event to append.</param>
        /// <remarks>
        /// <para>
        /// A subclass must implement this method to perform
        /// logging of the <paramref name="loggingEvent" />.
        /// </para>
        /// <para>This method will be called by <see cref="M:DoAppend(LoggingEvent)" />
        /// if all the conditions listed for that method are met.
        /// </para>
        /// <para>
        /// To restrict the logging of events in the appender
        /// override the <see cref="M:PreAppendCheck()" /> method.
        /// </para>
        /// </remarks>
        protected override void Append(LoggingEvent loggingEvent)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    Target.Add(loggingEvent);
                });
        }

        #endregion Protected Methods
    }
}