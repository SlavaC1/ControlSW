using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Properties;
using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    /// <summary>
    /// Provides basic implementation of <see cref="BSS.MVVM.Model.BusinessLogic.Plc.IPlc"/> interface.
    /// </summary>
    public abstract class PlcBase : IPlc
    {
        #region Private Fields

        /// <summary>
        /// <c>true</c> if PLC device is ready.
        /// </summary>
        private bool isDeviceReady;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="PlcBase"/> class.
        /// </summary>
        public PlcBase()
        {
            ChassisCount = ConfigurationParameters.StandaloneStationChassisCount;
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when HW initialization is completed.
        /// </summary>
        public event EventHandler IsReadyChanged;

        #endregion Public Events

        #region Public Properties

        /// <summary>
        /// Gets or sets the chassis count.
        /// </summary>
        /// <value>
        /// The chassis count.
        /// </value>
        public byte ChassisCount { get; set; }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets or sets a value indicating whether PLC device is ready.
        /// </summary>
        /// <value>
        ///   <c>true</c> if PLC device is ready; otherwise, <c>false</c>.
        /// </value>
        protected virtual bool IsDeviceReady
        {
            get
            {
                return isDeviceReady;
            }
            set
            {
                if (isDeviceReady != value)
                {
                    isDeviceReady = value;
                    if (isDeviceReady)
                    {
                        MessengerUtils.SendInfoMessage(Resources.PlcReady);
                    }
                    else
                    {
                        MessengerUtils.SendErrorMessage(Resources.PlcNotReady);
                    }

                    OnIsReadyChanged(EventArgs.Empty);
                }
            }
        }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// When implemented in derived class, gets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <returns>
        /// The traffic light color.
        /// </returns>
        public abstract Task<TrafficLightColor> GetTrafficLightColor(byte trafficLightNumber);

        /// <summary>
        /// When implemented in derived class, initializes connection to PLC.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if connection was established successfully; otherwise, <c>false</c>.
        /// </returns>
        public abstract Task<bool> Init();

        /// <summary>
        /// Determines whether connection to PLC is ready.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if connection was established successfully; otherwise, <c>false</c>.
        /// </returns>
        public bool IsReady()
        {
            return IsDeviceReady;
        }

        /// <summary>
        /// Turns off all traffic lights and buzzer.
        /// </summary>
        public abstract Task<bool> Reset();
        /// <summary>
        /// When implemented in derived class, sets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="color">The color.</param>
        public abstract Task<bool> SetTrafficLightColor(byte trafficLightNumber, TrafficLightColor color);

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:IsReadyChanged"/> event.
        /// </summary>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        protected virtual void OnIsReadyChanged(EventArgs e)
        {
            EventHandler temp = IsReadyChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Tests if PLC device is ready ready.
        /// </summary>
        protected void TestReady()
        {
            if (!isDeviceReady)
            {
                throw new InvalidOperationException(Resources.PlcNotReady);
            }
        }

        /// <summary>
        /// Tests if traffic light number is in range.
        /// </summary>
        protected void TestTrafficLightNumber(byte trafficLightNumber)
        {
            bool inRange = (trafficLightNumber > 0) && (trafficLightNumber <= ChassisCount);
            if (!inRange)
            {
                throw new ArgumentOutOfRangeException(
                    "trafficLightNumber",
                    ChassisCount,
                    String.Format(Resources.ArgumentMustNotBeGreaterMessage, trafficLightNumber, ChassisCount));
            }
        }

        #endregion Protected Methods
    }
}