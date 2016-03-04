using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Threading;

namespace RSTesterUI.ViewModels
{
    class AsynchronousCommand : Command
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="AsynchronousCommand"/> class.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="canExecute">if set
        /// to <c>true</c> the command can execute.</param>
        public AsynchronousCommand(Action action, bool canExecute = true)
            : base(action, canExecute)
        {
            //  Initialise the command.
            Initialise();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="AsynchronousCommand"/> class.
        /// </summary>
        /// <param name="parameterizedAction">The parameterized action.</param>
        /// <param name="canExecute">if set to <c>true</c> [can execute].</param>
        public AsynchronousCommand(Action<object> parameterizedAction, bool canExecute = true)
            : base(parameterizedAction, canExecute)
        {

            //  Initialise the command.
            Initialise();
        }

        Dispatcher callingDispatcher;

        /// <summary>
        /// The cancel command.
        /// </summary>
        private Command cancelCommand;

        /// <summary>
        /// Gets the cancel command.
        /// </summary>
        public Command CancelCommand
        {
            get { return cancelCommand; }
        }

        private bool isCancellationRequested;

        /// <summary>
        /// Gets or sets a value indicating whether this instance is cancellation requested.
        /// </summary>
        /// <value>
        ///     <c>true</c> if this instance is cancellation requested; otherwise, <c>false</c>.
        /// </value>
        public bool IsCancellationRequested
        {
            get
            {
                return isCancellationRequested;
            }
            set
            {
                if (isCancellationRequested != value)
                {
                    isCancellationRequested = value;
                    NotifyPropertyChanged("IsCancellationRequested");
                }
            }
        }

        /// <summary>
        /// Initialises this instance.
        /// </summary>
        private void Initialise()
        {
            //  Construct the cancel command.
            cancelCommand = new Command(
              () =>
              {
                  //  Set the Is Cancellation Requested flag.
                  IsCancellationRequested = true;
              }, true);
        }

        /// <summary>
        /// Flag indicating that the command is executing.
        /// </summary>
        private bool isExecuting = false;

        /// <summary>
        /// Gets or sets a value indicating whether this instance is executing.
        /// </summary>
        /// <value>
        ///     <c>true</c> if this instance is executing; otherwise, <c>false</c>.
        /// </value>
        public bool IsExecuting
        {
            get
            {
                return isExecuting;
            }
            set
            {
                if (isExecuting != value)
                {
                    isExecuting = value;
                    NotifyPropertyChanged("IsExecuting");
                }
            }
        }

        /// <summary>
        /// Executes the command.
        /// </summary>
        /// <param name="param">The param.</param>
        public override void DoExecute(object param)
        {
            //  If we are already executing, do not continue.
            if (IsExecuting)
                return;

            //  Invoke the executing command, allowing the command to be cancelled.
            CancelCommandEventArgs args =
               new CancelCommandEventArgs() { Parameter = param, Cancel = false };
            InvokeExecuting(args);

            //  If the event has been cancelled, bail now.
            if (args.Cancel)
                return;

            //  We are executing.
            IsExecuting = true;

            callingDispatcher = Dispatcher.CurrentDispatcher;

            ThreadPool.QueueUserWorkItem(
            (state) =>
            {
                //  Invoke the action.
                InvokeAction(param);

                //  Fire the executed event and set the executing state.
                ReportProgress(
                  () =>
                  {
                      //  We are no longer executing.
                      IsExecuting = false;

                      //  If we were cancelled,
                      //  invoke the cancelled event - otherwise invoke executed.
                      if (IsCancellationRequested)
                          InvokeCancelled(new CommandEventArgs() { Parameter = param });
                      else
                          InvokeExecuted(new CommandEventArgs() { Parameter = param });

                      //  We are no longer requesting cancellation.
                      IsCancellationRequested = false;
                  }
                );
            }
            );
        }

        /// <summary>
        /// Reports progress on the thread which invoked the command.
        /// </summary>
        /// <param name="action">The action.</param>
        public void ReportProgress(Action action)
        {
            if (IsExecuting)
            {
                if (callingDispatcher.CheckAccess())
                    action();
                else
                    callingDispatcher.BeginInvoke(((Action)(() => { action(); })));
            }
        }

        /// <summary>
        /// Occurs when the command cancelled.
        /// </summary>
        public event CommandEventHandler Cancelled;

        protected void InvokeCancelled(CommandEventArgs args)
        {
            CommandEventHandler cancelled = Cancelled;

            //  Call the executed event.
            if (cancelled != null)
                cancelled(this, args);
        }
    }
}
