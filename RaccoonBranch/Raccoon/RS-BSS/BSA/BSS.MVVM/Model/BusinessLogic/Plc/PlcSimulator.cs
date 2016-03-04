using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    /// <summary>
    /// Provides simulation of PLC device.
    /// </summary>
    internal class PlcSimulator : PlcBase
    {
        #region Private Fields

        /// <summary>
        /// Holds the traffic light colors.
        /// </summary>
        private TrafficLightColor[] trafficLights;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="PlcSimulator"/> class.
        /// </summary>
        /// <param name="chassisCount">The chassis count.</param>
        public PlcSimulator()
        {
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Gets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <returns>
        /// The traffic light color.
        /// </returns>
        /// <exception cref="System.ArgumentOutOfRangeException">trafficLightNumber is greater than or equal to number of traffic lights.</exception>
        public override async Task<TrafficLightColor> GetTrafficLightColor(byte trafficLightNumber)
        {
            TestTrafficLightNumber(trafficLightNumber);
            TestReady();

            return await Task<bool>.Run(() =>
            {
                return trafficLights[trafficLightNumber - 1];
            });
        }

        /// <summary>
        /// Initializes connection to PLC.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if connection was established successfully; otherwise, <c>false</c>.
        /// </returns>
        public override async Task<bool> Init()
        {
            return await Task<bool>.Run(() =>
                {
                    trafficLights = new TrafficLightColor[ChassisCount];
                    IsDeviceReady = true;
                    return true;
                });
        }

        /// <summary>
        /// Turns off all traffic lights and buzzer.
        /// </summary>
        /// <returns></returns>
        public override async Task<bool> Reset()
        {
            TestReady();

            return await Task<bool>.Run(() =>
            {
                Array.Clear(trafficLights, 0, trafficLights.Length);
                return true;
            });
        }

        /// <summary>
        /// Sets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="color">The color.</param>
        /// <exception cref="System.ArgumentOutOfRangeException">trafficLightNumber is greater than or equal to number of traffic lights.</exception>
        public override async Task<bool> SetTrafficLightColor(byte trafficLightNumber, TrafficLightColor color)
        {
            TestTrafficLightNumber(trafficLightNumber);
            TestReady();

            return await Task<bool>.Run(() =>
                {
                    trafficLights[trafficLightNumber - 1] = color;
                    return true;
                });
        }

        #endregion Public Methods
    }
}