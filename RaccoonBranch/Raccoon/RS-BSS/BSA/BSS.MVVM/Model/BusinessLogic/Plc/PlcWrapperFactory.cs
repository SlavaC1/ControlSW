using System;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    public static class PlcWrapperFactory
    {
        #region Public Methods

        /// <summary>
        /// Creates the PLC wrapper.
        /// </summary>
        /// <param name="plcConfigurationParameters"></param>
        /// <returns></returns>
        public static IPlc CreatePlcWrapper(ConfigurationParameters plcConfigurationParameters)
        {
            PlcBase plcWrapper;
            if (plcConfigurationParameters.StationType == BssStation.Inline ||
                plcConfigurationParameters.UsePlcSimulator)
            {
                plcWrapper = new PlcSimulator();
            }
            else
            {
                plcWrapper = new PlcDeviceWrapper
                {
                    CallTimeout = plcConfigurationParameters.PlcCallTimeout,
                    BuzzTime = plcConfigurationParameters.PlcBuzzDuration,
                    Port = plcConfigurationParameters.PlcPort
                };
            }

            plcWrapper.ChassisCount = Convert.ToByte(plcConfigurationParameters.ChassisCount);
            return plcWrapper;
        }

        #endregion Public Methods
    }
}