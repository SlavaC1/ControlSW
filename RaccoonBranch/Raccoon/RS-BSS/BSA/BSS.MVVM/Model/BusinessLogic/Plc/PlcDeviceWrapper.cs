using BSS.MVVM.Model.BusinessLogic.Messages;
using Modbus.Device;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.Plc
{
    /// <summary>
    /// Provides functionality of PLC device.
    /// </summary>
    internal class PlcDeviceWrapper : PlcBase, IDisposable
    {
        #region Private Fields

        private static readonly ushort BuzzerRegisterAddress = 160;

        private static readonly ushort TrafficLightsRegisterAddress = 0;

        private AsyncLock asyncLock = new AsyncLock();

        /// <summary>
        /// Holds the modbus serial master.
        /// </summary>
        private IModbusSerialMaster modbusSerialMaster;

        /// <summary>
        /// The modbus slave address.
        /// </summary>
        private byte slaveAddress = 1;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="PlcDeviceWrapper"/> class.
        /// </summary>
        /// <param name="chassisCount">The chassis count.</param>
        public PlcDeviceWrapper()
        {
            CallTimeout = TimeSpan.FromMilliseconds(500);
            BuzzTime = TimeSpan.FromMilliseconds(500);
            Port = "COM1";
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the buzz time.
        /// </summary>
        /// <value>
        /// The buzz time.
        /// </value>
        public TimeSpan BuzzTime
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the timeout for calls to PLC.
        /// </summary>
        /// <value>
        /// The call timeout.
        /// </value>
        public TimeSpan CallTimeout
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the PLC port.
        /// </summary>
        /// <value>
        /// The PLC port.
        /// </value>
        public string Port
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets a value indicating whether PLC device is ready.
        /// </summary>
        /// <value>
        ///   <c>true</c> if PLC device is ready; otherwise, <c>false</c>.
        /// </value>
        protected override bool IsDeviceReady
        {
            get
            {
                return base.IsDeviceReady;
            }
            set
            {
                if (value == false)
                {
                    DisposeModbusSerialMaster();
                }

                base.IsDeviceReady = value;
            }
        }

        #endregion Public Properties

        #region Private Methods

        /// <summary>
        /// Turns buzzer on for time determined by BuzzTime property.
        /// </summary>
        /// <param name="buzzTime">Buzz time.</param>
        /// <returns></returns>
        private bool Buzz(TimeSpan buzzTime)
        {
            modbusSerialMaster.WriteSingleCoil(slaveAddress, BuzzerRegisterAddress, true);
            Thread.Sleep(buzzTime);
            modbusSerialMaster.WriteSingleCoil(slaveAddress, BuzzerRegisterAddress, false);

            return true;
        }

        /// <summary>
        /// Disposes the modbus serial master.
        /// </summary>
        private void DisposeModbusSerialMaster()
        {
            if (modbusSerialMaster != null)
            {
                try
                {
                    modbusSerialMaster.Dispose();
                }
                catch
                {
                }

                modbusSerialMaster = null;
            }
        }

        /// <summary>
        /// Invokes a PLC internal function.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="TResult">The type of the result.</typeparam>
        /// <param name="plcInternalFunc">The PLC internal function.</param>
        /// <param name="arg">The argument.</param>
        /// <returns></returns>
        private TResult InvokePlcInternal<T, TResult>(Func<T, TResult> plcInternalFunc, T arg)
        {
            TResult result = default(TResult);

            try
            {
                result = plcInternalFunc(arg);
            }
            catch (NullReferenceException nullEx)
            {
                // ignore logging this exception after call timeout was reached
                // and consequently modbusSerialMaster is disposed:
                if (modbusSerialMaster != null)
                {
                    MessengerUtils.SendException(nullEx);
                }

                IsDeviceReady = false;
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
                IsDeviceReady = false;
            }

            return result;
        }

        /// <summary>
        /// Invokes the PLC task.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="TResult">The type of the result.</typeparam>
        /// <param name="plcInternalFunc">The PLC internal function.</param>
        /// <param name="arg">The argument.</param>
        /// <returns></returns>
        private async Task<TResult> InvokePlcTask<T, TResult>(Func<T, TResult> plcInternalFunc, T arg)
        {
            using (var releaser = await asyncLock.LockAsync())
            {
                Task<TResult> invokePlcInternalTask = Task<TResult>.Run(() =>
                    {
                        return InvokePlcInternal(plcInternalFunc, arg);
                    });

                var completedTask = await Task.WhenAny(
                    invokePlcInternalTask,
                    Task.Delay(CallTimeout));

                if (completedTask == invokePlcInternalTask)
                {
                    return invokePlcInternalTask.Result;
                }
                else
                {
                    IsDeviceReady = false;
                    return default(TResult);
                }
            }
        }

        #endregion Private Methods

        #region IPlc Members

        /// <summary>
        /// Gets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <returns>
        /// The traffic light color.
        /// </returns>
        /// <exception cref="System.ArgumentOutOfRangeException"><paramref name="trafficLightNumber"/> is greater then or equal to number of chassis.</exception>
        /// <exception cref="System.InvalidOperationException">PLC is not ready</exception>
        public override async Task<TrafficLightColor> GetTrafficLightColor(byte trafficLightNumber)
        {
            TestTrafficLightNumber(trafficLightNumber);
            TestReady();

            return await InvokePlcTask(GetTrafficLightColorInternal, trafficLightNumber);
        }

        /// <summary>
        /// Initializes connection to PLC.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if connection was established successfully; otherwise, <c>false</c>.
        /// </returns>
        public override async Task<bool> Init()
        {
            int colorsCount = Enum.GetValues(typeof(TrafficLightColor)).Length - 1;
            return await InvokePlcTask(InitInternal, colorsCount);
        }

        /// <summary>
        /// Turns off all traffic lights and buzzer.
        /// </summary>
        /// <returns></returns>
        public override async Task<bool> Reset()
        {
            int colorsCount = Enum.GetValues(typeof(TrafficLightColor)).Length - 1;
            return await InvokePlcTask(ResetInternal, colorsCount);
        }

        /// <summary>
        /// Sets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="color">The color.</param>
        /// <exception cref="System.ArgumentOutOfRangeException"><paramref name="trafficLightNumber"/> is greater then or equal to number of chassis.</exception>
        /// <exception cref="System.InvalidOperationException">PLC is not ready</exception>
        public override async Task<bool> SetTrafficLightColor(byte trafficLightNumber, TrafficLightColor color)
        {
            TestTrafficLightNumber(trafficLightNumber);
            TestReady();

            return await InvokePlcTask(
                SetTrafficLightColorInternal,
                new SetTrafficLightColorArgs { TrafficLightNumber = trafficLightNumber, Color = color });
        }

        #endregion IPlc Members

        #region Plc Internal Methods

        /// <summary>
        /// Gets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <returns>
        /// The traffic light color.
        /// </returns>
        private TrafficLightColor GetTrafficLightColorInternal(byte trafficLightNumber)
        {
            byte colorsCount = (byte)(Enum.GetValues(typeof(TrafficLightColor)).Length - 1);
            bool[] trafficLightStatus = modbusSerialMaster.ReadCoils(
                slaveAddress,
                Convert.ToUInt16(TrafficLightsRegisterAddress + (trafficLightNumber - 1) * colorsCount),
                colorsCount);

            int colorValue = 1 + Array.IndexOf(trafficLightStatus, true);
            return (TrafficLightColor)colorValue;
        }

        /// <summary>
        /// Initializes connection to PLC.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if connection was established successfully; otherwise, <c>false</c>.
        /// </returns>
        private bool InitInternal(int colorsCount)
        {
            IsDeviceReady = false;

            SerialPort serialPort = new SerialPort
            {
                PortName = Port,
                BaudRate = 9600,
                DataBits = 8,
                Parity = Parity.None,
                StopBits = StopBits.One,
            };

            serialPort.Open();
            slaveAddress = Byte.Parse(serialPort.PortName.Replace("COM", String.Empty));
            modbusSerialMaster = ModbusSerialMaster.CreateRtu(serialPort);
            ResetInternal(colorsCount);
            IsDeviceReady = true;

            return IsDeviceReady;
        }

        /// <summary>
        /// Turns off all traffic lights and buzzer.
        /// </summary>
        private bool ResetInternal(int colorsCount)
        {
            //modbusSerialMaster.WriteMultipleCoils(SlaveAddress, RegisterAddress, new bool[ChassisCount * colorsCount]);
            for (int i = 0; i < ChassisCount * colorsCount; i++)
            {
                modbusSerialMaster.WriteSingleCoil(slaveAddress, (ushort)(TrafficLightsRegisterAddress + i), false);
            }

            modbusSerialMaster.WriteSingleCoil(slaveAddress, BuzzerRegisterAddress, false);
            return true;
        }

        /// <summary>
        /// Sets the color of the traffic light specified by its number.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="color">The color.</param>
        private bool SetTrafficLightColorInternal(SetTrafficLightColorArgs args)
        {
            byte trafficLightNumber = args.TrafficLightNumber;
            TrafficLightColor color = args.Color;

            modbusSerialMaster.WriteSingleCoil(slaveAddress, BuzzerRegisterAddress, false);

            int colorsCount = Enum.GetValues(typeof(TrafficLightColor)).Length - 1;
            ushort offset = Convert.ToUInt16(TrafficLightsRegisterAddress + (trafficLightNumber - 1) * colorsCount);
            for (int i = 0; i < colorsCount; i++)
            {
                modbusSerialMaster.WriteSingleCoil(slaveAddress, (ushort)(offset + i), (TrafficLightColor)(i + 1) == color);
            }

            if (color == TrafficLightColor.Red)
            {
                Task.Run(() =>
                    {
                        InvokePlcInternal(Buzz, BuzzTime);
                    });
            }

            return true;
        }

        #endregion Plc Internal Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                DisposeModbusSerialMaster();
            }
        }

        #endregion IDisposable Members

        #region Private Classes

        private struct SetTrafficLightColorArgs
        {
            #region Public Properties

            public TrafficLightColor Color { get; set; }

            public byte TrafficLightNumber { get; set; }

            #endregion Public Properties
        }

        #endregion Private Classes
    }
}