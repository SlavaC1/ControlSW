using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Properties;
using System;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// A base class representing burning, verification or correction system state.
    /// </summary>
    /// <typeparam name="T">Type of IDT operator.</typeparam>
    public abstract class IdtOperationState : BssState
    {
        #region Private Fields

        private CommunicationsManager communicationsManager;

        private IdtOperator idtOperator;

        #endregion Private Fields

        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtOperationState{T}"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="idtOperator">The idt operator.</param>
        /// <param name="communicationsManager">The communications manager.</param>
        /// <exception cref="ArgumentNullException">
        /// idtBurner
        /// or
        /// communicationsManager
        /// </exception>
        protected IdtOperationState(ConfigurationParameters configurationParameters, IdtOperator idtOperator, CommunicationsManager communicationsManager)
            : base(configurationParameters)
        {
            if (idtOperator == null)
            {
                throw new ArgumentNullException("idtOperator");
            }

            if (communicationsManager == null)
            {
                throw new ArgumentNullException("communicationsManager");
            }

            this.communicationsManager = communicationsManager;
            this.idtOperator = idtOperator;
        }

        #endregion Protected Constructors

        #region Public Properties

        public IdtOperator IdtOperator
        {
            get
            {
                return idtOperator;
            }
            protected set
            {
                idtOperator = value;
            }
        }

        #endregion Public Properties

        #region Public Events

        /// <summary>
        /// Occurs when state is termintated.
        /// </summary>
        public event EventHandler Terminated;

        /// <summary>
        /// Occurs before state is terminated; user may cancel termination.
        /// </summary>
        public event EventHandler<CancelEventArgs> Terminating;

        #endregion Public Events

        #region Public Methods

        /// <summary>
        /// Determines whether transition to the specified state requires all cartridges to be removed.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsCartridgeRemovalRequired()
        {
            return true;
        }

        /// <summary>
        /// Determines whether is hardware prepared for transition to a the specified new state.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsHWPrepared()
        {
            return Enum.GetValues(typeof(BssPeripheral))
                .OfType<BssPeripheral>()
                .All(device => communicationsManager.GetDeviceConnectionStatus(device));
        }

        /// <summary>
        /// Performs a set of actions before state activation.
        /// </summary>
        public override async Task Prepare()
        {
            await Task.Run(() =>
                {
                    idtOperator.StartListenInPlaceStatusChanges();
                });
        }

        /// <summary>
        /// Performs a set of actions after state deactivation.
        /// </summary>
        public override async Task Shutdown()
        {
            if (idtOperator.IsBusy())
            {
                throw new InvalidStateTransitionException(String.Format(Resources.OperationInProcess, StateName));
            }

            await Task.Run(() =>
                {
                    idtOperator.StopListenInPlaceStatusChanges();
                });
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:Terminated" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnTerminated(EventArgs e)
        {
            EventHandler temp = Terminated;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:Terminating" /> event.
        /// </summary>
        /// <param name="e">The <see cref="CancelEventArgs"/> instance containing the event data.</param>
        protected virtual void OnTerminating(CancelEventArgs e)
        {
            EventHandler<CancelEventArgs> temp = Terminating;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion Protected Methods
    }

    /// <summary>
    /// A base class representing burning, verification or correction system state.
    /// </summary>
    /// <typeparam name="T">Type of IDT operator.</typeparam>
    public abstract class IdtOperationState<T> : IdtOperationState
        where T : IdtOperator
    {
        #region Protected Constructors

        protected IdtOperationState(ConfigurationParameters configurationParameters, T idtOperator, CommunicationsManager communicationsManager)
            : base(configurationParameters, idtOperator, communicationsManager)
        {
        }

        #endregion Protected Constructors

        #region Public Properties

        public new T IdtOperator
        {
            get
            {
                return base.IdtOperator as T;
            }
            private set
            {
                base.IdtOperator = value;
            }
        }

        #endregion Public Properties
    }
}