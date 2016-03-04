using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    /// <summary>
    /// A lock for async tasks.
    /// </summary>
    public class AsyncLock
    {
        #region Private Fields

        private readonly Task<Releaser> releaser;

        private readonly AsyncSemaphore semaphore;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="AsyncLock"/> class.
        /// </summary>
        public AsyncLock()
        {
            semaphore = new AsyncSemaphore(1);
            releaser = Task.FromResult(new Releaser(this));
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Locks an asynch task.
        /// </summary>
        /// <returns></returns>
        public Task<Releaser> LockAsync()
        {
            var wait = semaphore.WaitAsync();
            return wait.IsCompleted ?
                releaser :
                wait.ContinueWith((t, state) => new Releaser((AsyncLock)state),
                    this, CancellationToken.None,
                    TaskContinuationOptions.ExecuteSynchronously, TaskScheduler.Default);
        }

        #endregion Public Methods

        #region Public Structs

        /// <summary>
        /// An object holding a semaphore to release.
        /// </summary>
        public struct Releaser : IDisposable
        {
            #region Private Fields

            private readonly AsyncLock toRelease;

            #endregion Private Fields

            #region Internal Constructors

            internal Releaser(AsyncLock toRelease)
            {
                this.toRelease = toRelease;
            }

            #endregion Internal Constructors

            #region Public Methods

            public void Dispose()
            {
                if (toRelease != null)
                {
                    toRelease.semaphore.Release();
                }
            }

            #endregion Public Methods
        }

        #endregion Public Structs
    }

    /// <summary>
    /// A semaphore for async tasks.
    /// </summary>
    public class AsyncSemaphore
    {
        #region Private Fields

        private readonly static Task completed = Task.FromResult(true);

        private readonly Queue<TaskCompletionSource<bool>> waiters;

        private uint currentCount;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="AsyncSemaphore"/> class.
        /// </summary>
        /// <param name="initialCount">The initial count.</param>
        public AsyncSemaphore(uint initialCount)
        {
            waiters = new Queue<TaskCompletionSource<bool>>();
            currentCount = initialCount;
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Releases an async task from queue.
        /// </summary>
        public void Release()
        {
            TaskCompletionSource<bool> toRelease = null;
            lock (waiters)
            {
                if (waiters.Count > 0)
                {
                    toRelease = waiters.Dequeue();
                }
                else
                {
                    currentCount++;
                }
            }

            if (toRelease != null)
            {
                toRelease.SetResult(result: true);
            }
        }

        /// <summary>
        /// Waits for all other async tasks in queue to complete.
        /// </summary>
        /// <returns>The waiter task.</returns>
        public Task WaitAsync()
        {
            lock (waiters)
            {
                if (currentCount > 0)
                {
                    currentCount--;
                    return completed;
                }
                else
                {
                    var waiter = new TaskCompletionSource<bool>();
                    waiters.Enqueue(waiter);
                    return waiter.Task;
                }
            }
        }

        #endregion Public Methods
    }
}
