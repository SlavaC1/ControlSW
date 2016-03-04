using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    /// <summary>
    /// Provides PLC device functionality.
    /// </summary>
    public interface IPlc
    {
        #region Public Events

        /// <summary>
        /// Occurs when PLC becomes ready or not ready.
        /// </summary>
        event EventHandler IsReadyChanged;

        #endregion Public Events

        #region Public Methods

        /// <summary>
        /// Gets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <returns>The traffic light color.</returns>
        Task<TrafficLightColor> GetTrafficLightColor(byte trafficLightNumber);

        /// <summary>
        /// Initializes connection to PLC.
        /// </summary>
        /// <returns><c>true</c> if connection was established successfully; otherwise, <c>false</c>.</returns>
        Task<bool> Init();

        /// <summary>
        /// Turns off all traffic lights and buzzer.
        /// </summary>
        Task<bool> Reset();

        /// <summary>
        /// Determines whether connection to PLC is ready.
        /// </summary>
        /// <returns><c>true</c> if connection was established successfully; otherwise, <c>false</c>.</returns>
        bool IsReady();

        /// <summary>
        /// Sets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="color">The color.</param>
        Task<bool> SetTrafficLightColor(byte trafficLightNumber, TrafficLightColor color);

        #endregion Public Methods
    }
}