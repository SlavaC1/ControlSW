using GalaSoft.MvvmLight.Messaging;
using System;

namespace BSS.MVVM.Model.BusinessLogic.Messages
{
    /// <summary>
    /// Provides utilities for sending messages via messenger.
    /// </summary>
    public static class MessengerUtils
    {
        /// <summary>
        /// Sends a debug message via messenger.
        /// </summary>
        /// <param name="message">The message.</param>
        public static void SendDebugMessage(string message)
        {
            Messenger.Default.Send(new LogMessage
            {
                LogLevel = Level.Debug,
                Message = message
            });
        }

        /// <summary>
        /// Sends an info message via messenger.
        /// </summary>
        /// <param name="message">The message.</param>
        public static void SendInfoMessage(string message)
        {
            Messenger.Default.Send(new LogMessage
            {
                Message = message
            });
        }

        /// <summary>
        /// Sends an error message via messenger.
        /// </summary>
        /// <param name="message">The message.</param>
        public static void SendErrorMessage(string message)
        {
            Messenger.Default.Send(new LogMessage
            {
                LogLevel = Level.Error,
                Message = message
            });
        }

        /// <summary>
        /// Sends an exception via messenger.
        /// </summary>
        /// <param name="message">The message.</param>
        public static void SendException(Exception ex)
        {
            AggregateException aggEx = ex as AggregateException;
            if (aggEx != null)
            {
                foreach (var innerException in aggEx.InnerExceptions)
                {
                    SendException(innerException);
                }
            }
            else
            {
                Messenger.Default.Send(new ExceptionMessage { Exception = ex });
            }
        }
    }
}
