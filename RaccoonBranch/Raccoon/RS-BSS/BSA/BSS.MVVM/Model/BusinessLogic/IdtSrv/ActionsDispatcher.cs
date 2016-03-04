using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Stores actions in a queue and dispatches them sequencially.
    /// </summary>
    /// <typeparam name="T">Type of action argument.</typeparam>
    public class ActionsDispatcher<T>
    {
        #region Private Fields

        /// <summary>
        /// The queue consumer thread for dispatching actions.
        /// </summary>
        private Thread _dispatcherThread;

        /// <summary>
        /// The actions inner queue.
        /// </summary>
        private Queue<Command<T>> _innerQueue;

        /// <summary>
        /// A list of obsolete actions.
        /// </summary>
        private List<Command<T>> _obsoleteActions;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ActionsDispatcher{T}"/> class.
        /// </summary>
        public ActionsDispatcher()
        {
            _innerQueue = new Queue<Command<T>>();
            _obsoleteActions = new List<Command<T>>();
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Cancels dispatching the specified action.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="parameter">The parameter.</param>
        public void CancelDispatch(Action<T> action, T parameter)
        {
            lock (_innerQueue)
            {
                _obsoleteActions.Add(new Command<T>(action, parameter));
            }
        }

        /// <summary>
        /// Clears actions queue.
        /// </summary>
        public void Clear()
        {
            lock (_innerQueue)
            {
                _innerQueue.Clear();
                _obsoleteActions.Clear();
            }
        }

        /// <summary>
        /// Dispatches the specified action. If dispatcher is busy, action is enqueued.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="parameter">The parameter.</param>
        public void Dispatch(Action<T> action, T parameter)
        {
            lock (_innerQueue)
            {
                Command<T> command = new Command<T>(action, parameter);
                _innerQueue.Enqueue(command);
                _obsoleteActions.RemoveAll(item => item.Action.Equals(command.Action) && item.Argument.Equals(command.Argument));
                RunDispatcherThread();
            }
        }

        /// <summary>
        /// Determines whether dispatcher queue is busy.
        /// </summary>
        /// <returns><c>true</c> if queue contains commands or command is running; otherwise, <c>false</c>.</returns>
        public bool IsBusy()
        {
            return (_dispatcherThread != null && _dispatcherThread.IsAlive);
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Runs the dispatcher thread.
        /// </summary>
        /// <remarks>
        /// While inner queue is not empty, dispatcher thread dequeues command and executes if it is not marked as obsolete.
        /// </remarks>
        private void RunDispatcherThread()
        {
            if (_dispatcherThread == null ||
                !_dispatcherThread.IsAlive)
            {
                _dispatcherThread = new Thread(() =>
                {
                    bool isObsolete;
                    Command<T> command;
                    while (TryDequeue(out command, out isObsolete))
                    {
                        if (!isObsolete)
                        {
                            command.Action(command.Argument);
                        }
                    }
                });

                _dispatcherThread.Start();
            }
        }

        /// <summary>
        /// Tries to dequeue command from inner queue.
        /// </summary>
        /// <param name="item">The item.</param>
        /// <param name="isObsolete">if set to <c>true</c> item is obsolete.</param>
        /// <returns><c>true</c> is command was dequeued; otherwise, <c>false</c>.</returns>
        private bool TryDequeue(out Command<T> item, out bool isObsolete)
        {
            lock (_innerQueue)
            {
                if (!_innerQueue.Any())
                {
                    item = null;
                    isObsolete = false;
                    return false;
                }

                item = _innerQueue.Dequeue();
                isObsolete = _obsoleteActions.Remove(item);
                return true;
            }
        }

        #endregion Private Methods
    }

    /// <summary>
    /// Holds an action and its argument.
    /// </summary>
    /// <typeparam name="T">Type of argument.</typeparam>
    internal class Command<T> : IEquatable<Command<T>>
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="Command`1"/> class.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="argument">The argument.</param>
        public Command(Action<T> action, T argument)
        {
            Action = action;
            Argument = argument;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the action.
        /// </summary>
        /// <value>
        /// The action.
        /// </value>
        public Action<T> Action { get; private set; }

        /// <summary>
        /// Gets the argument.
        /// </summary>
        /// <value>
        /// The argument.
        /// </value>
        public T Argument { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// true if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public bool Equals(Command<T> other)
        {
            return other != null && Action.Equals(other.Action) && Argument.Equals(other.Argument);
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object" />, is equal to this instance.
        /// </summary>
        /// <param name="obj">The <see cref="System.Object" /> to compare with this instance.</param>
        /// <returns>
        ///   <c>true</c> if the specified <see cref="System.Object" /> is equal to this instance; otherwise, <c>false</c>.
        /// </returns>
        public override bool Equals(object obj)
        {
            return Equals(obj as Command<T>);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode()
        {
            return Action.GetHashCode() ^ Action.GetHashCode();
        }

        #endregion Public Methods
    }
}