using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Properties;
using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Globalization;
using System.IO;
using System.Linq;

namespace BSS.MVVM.Model.BusinessLogic
{
    public enum BssStation
    {
        Standalone,
        Inline
    }

    public enum ExecutionMode
    {
        Normal,
        Debug
    }

    public class ChassisPrivilege : ObservableObject, IEquatable<ChassisPrivilege>
    {
        #region Private Fields

        private MaterialKind materialGroup;

        private IdtOperationPrivileges operation;

        #endregion Private Fields

        #region Public Properties

        /// <summary>
        /// Gets or sets the material group.
        /// </summary>
        /// <value>
        /// The material group.
        /// </value>
        public MaterialKind MaterialKind
        {
            get
            {
                return materialGroup;
            }
            set
            {
                materialGroup = value;
                RaisePropertyChanged(() => MaterialKind);
            }
        }

        /// <summary>
        /// Gets or sets the operation.
        /// </summary>
        /// <value>
        /// The operation.
        /// </value>
        public IdtOperationPrivileges Operation
        {
            get
            {
                return operation;
            }
            set
            {
                operation = value;
                RaisePropertyChanged(() => Operation);
            }
        }

        #endregion Public Properties

        #region IEquatable<ChassisPrivilege> Members

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// true if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public bool Equals(ChassisPrivilege other)
        {
            if (other == null)
            {
                return false;
            }

            return materialGroup == other.materialGroup &&
                operation == other.operation;
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
            return Equals(obj as ChassisPrivilege);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode()
        {
            return materialGroup.GetHashCode() ^ operation.GetHashCode();
        }

        #endregion IEquatable<ChassisPrivilege> Members
    }

    /// <summary>
    /// Holds BSS app configuration parameters.
    /// </summary>
    public class ConfigurationParameters : ObservableObject, IEquatable<ConfigurationParameters>
    {
        #region Private Fields

        /// <summary>
        /// The default number of chassis in inline station.
        /// </summary>
        public static readonly byte InlineStationChassisCount = 2;

        public static readonly byte MaxChassisCount = 8;

        /// <summary>
        /// The default number of chassis in standalone station.
        /// </summary>
        public static readonly byte StandaloneStationChassisCount = 4;

        private ChassisPrivilege[] chassisPriviliges;

        private CultureInfo culture;

        private ExecutionMode executionMode;

        private string exportFileFormat;

        /// <summary>
        /// The interval between two successive calls to get in-place status.
        /// </summary>
        private TimeSpan inPlaceStatusInterval;

        /// <summary>
        /// The PLC buzz duration.
        /// </summary>
        private TimeSpan plcBuzzDuration;

        /// <summary>
        /// The timeout for a PLC call.
        /// </summary>
        private TimeSpan plcCallTimeout;

        private string plcPort;

        private string stationName;

        private BssStation stationType;

        /// <summary>
        /// <c>true</c> if use PLC simulator; otherwise, <c>false</c>.
        /// </summary>
        private bool usePlcSimulator;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ConfigurationParameters"/> class.
        /// </summary>
        public ConfigurationParameters()
        {
            Reset();
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the chassis count.
        /// </summary>
        /// <value>
        /// The chassis count.
        /// </value>
        public int ChassisCount
        {
            get
            {
                return chassisPriviliges.Length;
            }
            set
            {
                if (value < 0 || value > MaxChassisCount)
                {
                    throw new ArgumentOutOfRangeException(
                        "value",
                        value,
                        String.Format(Resources.ArgumentMustNotBeGreaterMessage, "value", MaxChassisCount));
                }

                if (ChassisCount != value)
                {
                    int oldChassisCount = ChassisCount;
                    Array.Resize(ref chassisPriviliges, value);
                    for (int i = oldChassisCount; i < chassisPriviliges.Length; i++)
                    {
                        chassisPriviliges[i] = new ChassisPrivilege();
                    }

                    RaisePropertyChanged(() => ChassisCount);
                }
            }
        }

        public CultureInfo Culture
        {
            get
            {
                return culture;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                culture = value;
                RaisePropertyChanged(() => Culture);
            }
        }

        public byte[] DevicePassword { get; private set; }

        public string Domain { get; private set; }

        public ExecutionMode ExecutionMode
        {
            get
            {
                return executionMode;
            }
            set
            {
                executionMode = value;
                RaisePropertyChanged(() => ExecutionMode);
            }
        }

        public string ExportFileFormat
        {
            get
            {
                return exportFileFormat;
            }
            set
            {
                exportFileFormat = value;
                RaisePropertyChanged(() => ExportFileFormat);
            }
        }

        /// <summary>
        /// Gets or sets the interval between two successive calls to get in-place status.
        /// </summary>
        /// <value>
        /// The interval between two successive calls to get in-place status.
        /// </value>
        public TimeSpan InPlaceStatusInterval
        {
            get
            {
                return inPlaceStatusInterval;
            }
            set
            {
                if (value < TimeSpan.FromMilliseconds(100))
                {
                    throw new ArgumentOutOfRangeException("value", value, String.Format(Resources.ArgumentMustNotBeSmallerMessage, "value", 100));
                }

                inPlaceStatusInterval = value;
                RaisePropertyChanged(() => InPlaceStatusInterval);
            }
        }

        /// <summary>
        /// Gets or the duration of active buzzer.
        /// </summary>
        /// <value>
        /// The duration of active buzzer.
        /// </value>
        public TimeSpan PlcBuzzDuration
        {
            get
            {
                return plcBuzzDuration;
            }
            set
            {
                plcBuzzDuration = value;
                RaisePropertyChanged(() => PlcBuzzDuration);
            }
        }

        /// <summary>
        /// Gets or sets the timeout of a PLC call.
        /// </summary>
        /// <value>
        /// The PLC call timeout.
        /// </value>
        public TimeSpan PlcCallTimeout
        {
            get
            {
                return plcCallTimeout;
            }
            set
            {
                plcCallTimeout = value;
                RaisePropertyChanged(() => PlcCallTimeout);
            }
        }

        /// <summary>
        /// Gets or sets the PLC port.
        /// </summary>
        /// <value>
        /// The PLC port.
        /// </value>
        public string PlcPort
        {
            get
            {
                return plcPort;
            }
            set
            {
                plcPort = value;
                RaisePropertyChanged(() => PlcPort);
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether reset tags.
        /// </summary>
        /// <value>
        ///   <c>true</c> if reset tags; otherwise, <c>false</c>.
        /// </value>
        public bool ResetTags { get; private set; }

        /// <summary>
        /// Gets or sets the name of the station.
        /// </summary>
        /// <value>
        /// The name of the station.
        /// </value>
        public string StationName
        {
            get
            {
                return stationName;
            }
            set
            {
                stationName = value;
                RaisePropertyChanged(() => StationName);
            }
        }

        /// <summary>
        /// Gets the station type.
        /// </summary>
        /// <value>
        /// The station type.
        /// </value>
        public BssStation StationType
        {
            get
            {
                return stationType;
            }
            set
            {
                if (stationType != value)
                {
                    stationType = value;
                    SetChassis();
                    RaisePropertyChanged(() => StationType);
                    RaisePropertyChanged(() => ChassisCount);
                    RaisePropertyChanged(() => UsePlcSimulator);
                }
            }
        }

        /// <summary>
        /// Gets a value indicating whether use PLC simulator.
        /// </summary>
        /// <value>
        ///   <c>true</c> if use PLC simulator; otherwise, <c>false</c>.
        /// </value>
        public bool UsePlcSimulator
        {
            get
            {
                return usePlcSimulator || stationType == BssStation.Inline;
            }
            set
            {
                usePlcSimulator = value;
                RaisePropertyChanged(() => UsePlcSimulator);
            }
        }

        #endregion Public Properties

        #region Public Methods

        public static CultureInfo[] GetSupportedCultures()
        {
            return new[]
            {
                CultureInfo.GetCultureInfo("en-US"),
                CultureInfo.GetCultureInfo("he-IL"),
            };
        }

        /// <summary>
        /// Gets the allowed material group on a specified chassis.
        /// </summary>
        /// <param name="chassisNumber">The chassis number.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentOutOfRangeException">chassisNumber</exception>
        public MaterialKind GetChassisMaterialGroup(byte chassisNumber)
        {
            if (chassisNumber > ChassisCount)
            {
                throw new ArgumentOutOfRangeException(
                    "chassisNumber",
                    ChassisCount,
                    String.Format(Resources.ArgumentMustNotBeGreaterMessage, chassisNumber, ChassisCount));
            }

            return chassisPriviliges[chassisNumber - 1].MaterialKind;
        }

        /// <summary>
        /// Gets the allowed operations on a specified chassis.
        /// </summary>
        /// <param name="chassisNumber">The chassis number.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentOutOfRangeException">chassisNumber</exception>
        public IdtOperationPrivileges GetChassisOperations(byte chassisNumber)
        {
            if (chassisNumber > ChassisCount)
            {
                throw new ArgumentOutOfRangeException(
                    "chassisNumber",
                    ChassisCount,
                    String.Format(Resources.ArgumentMustNotBeGreaterMessage, chassisNumber, ChassisCount));
            }

            return chassisPriviliges[chassisNumber - 1].Operation;
        }

        /// <summary>
        /// Loads configuration parameters from config file.
        /// </summary>
        public void Load()
        {
            bool ok;
            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement;

                configElement = config.AppSettings.Settings["Culture"];
                if (configElement != null)
                {
                    CultureInfo newCulture = CultureInfo.GetCultureInfo(configElement.Value);
                    if (newCulture != null)
                    {
                        Culture = newCulture;
                    }
                }

                configElement = config.AppSettings.Settings["StationType"];
                if (configElement != null)
                {
                    BssStation configStationType;
                    ok = Enum.TryParse(configElement.Value, out configStationType);
                    if (ok)
                    {
                        StationType = configStationType;
                    }
                }

                configElement = config.AppSettings.Settings["StationName"];
                if (configElement != null)
                {
                    StationName = configElement.Value;
                }

                List<ChassisPrivilege> chassisPrivilegesList = new List<ChassisPrivilege>();
                while ((configElement = config.AppSettings.Settings["Chassis_" + chassisPrivilegesList.Count]) != null)
                {
                    ChassisPrivilege newChassisPrivilege = new ChassisPrivilege();
                    string rawPrivileges = configElement.Value;
                    newChassisPrivilege.MaterialKind = MaterialKind.None;
                    if (rawPrivileges.Contains("M"))
                    {
                        newChassisPrivilege.MaterialKind |= MaterialKind.M;
                    }
                    if (rawPrivileges.Contains("S"))
                    {
                        newChassisPrivilege.MaterialKind |= MaterialKind.S;
                    }

                    newChassisPrivilege.Operation = IdtOperationPrivileges.None;
                    if (rawPrivileges.Contains('B'))
                    {
                        newChassisPrivilege.Operation |= IdtOperationPrivileges.B;
                    }
                    if (rawPrivileges.Contains('V'))
                    {
                        newChassisPrivilege.Operation |= IdtOperationPrivileges.V;
                    }

                    chassisPrivilegesList.Add(newChassisPrivilege);
                }

                chassisPriviliges = chassisPrivilegesList.ToArray();
                RaisePropertyChanged(() => ChassisCount);

                //configElement = config.AppSettings.Settings["ResetTags"];
                //if (configElement != null)
                //{
                //    bool newResetTags;
                //    ok = Boolean.TryParse(configElement.Value, out newResetTags);
                //    if (ok)
                //    {
                //        ResetTags = newResetTags;
                //    }
                //}

                configElement = config.AppSettings.Settings["DevicePassword"];
                if (configElement != null)
                {
                    DevicePassword = configElement.Value.ToByteArray();
                }

                configElement = config.AppSettings.Settings["UsePlcSimulator"];
                if (configElement != null)
                {
                    bool newUsePlcSimulator;
                    ok = Boolean.TryParse(configElement.Value, out newUsePlcSimulator);
                    if (ok)
                    {
                        UsePlcSimulator = newUsePlcSimulator;
                    }
                }

                configElement = config.AppSettings.Settings["PlcPort"];
                if (configElement != null)
                {
                    PlcPort = configElement.Value;
                }

                configElement = config.AppSettings.Settings["InPlaceStatusInterval"];
                if (configElement != null)
                {
                    double newInPlaceStatusInterval;
                    ok = Double.TryParse(configElement.Value, out newInPlaceStatusInterval);
                    if (ok)
                    {
                        InPlaceStatusInterval = TimeSpan.FromMilliseconds(newInPlaceStatusInterval);
                    }
                }

                configElement = config.AppSettings.Settings["BuzzDuration"];
                if (configElement != null)
                {
                    ushort newPlcBuzzDuration;
                    ok = UInt16.TryParse(configElement.Value, out newPlcBuzzDuration);
                    if (ok)
                    {
                        PlcBuzzDuration = TimeSpan.FromSeconds(newPlcBuzzDuration);
                    }
                }

                configElement = config.AppSettings.Settings["PlcCallTimeout"];
                if (configElement != null)
                {
                    ushort newPlcCallTimeout;
                    ok = UInt16.TryParse(configElement.Value, out newPlcCallTimeout);
                    if (ok)
                    {
                        PlcCallTimeout = TimeSpan.FromMilliseconds(newPlcCallTimeout);
                    }
                }

                configElement = config.AppSettings.Settings["ExecutionMode"];
                if (configElement != null)
                {
                    ExecutionMode newExecutionMode;
                    ok = Enum.TryParse(configElement.Value, out newExecutionMode);
                    if (ok)
                    {
                        ExecutionMode = newExecutionMode;
                    }
                }

                configElement = config.AppSettings.Settings["ExportFileFormat"];
                if (configElement != null)
                {
                    ExportFileFormat = configElement.Value;
                }

                configElement = config.AppSettings.Settings["Domain"];
                if (configElement != null)
                {
                    Domain = configElement.Value;
                }
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
        }

        /// <summary>
        /// Restores the default configuration.
        /// </summary>
        public void RestoreDefaults()
        {
            Reset();
            foreach (var pi in this.GetType().GetProperties())
            {
                RaisePropertyChanged(pi.Name);
            }
        }

        /// <summary>
        /// Saves configuration parameters from config file.
        /// </summary>
        public void Save()
        {
            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement;

                configElement = config.AppSettings.Settings["Culture"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("Culture", Culture.DisplayName);
                }
                else
                {
                    configElement.Value = Culture.Name;
                }

                configElement = config.AppSettings.Settings["StationType"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("StationType", StationType.ToString());
                }
                else
                {
                    configElement.Value = StationType.ToString();
                }

                configElement = config.AppSettings.Settings["StationName"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("StationName", StationName);
                }
                else
                {
                    configElement.Value = StationName;
                }

                for (int i = 0; i < chassisPriviliges.Length; i++)
                {
                    configElement = config.AppSettings.Settings["Chassis_" + i];
                    string value = chassisPriviliges[i].MaterialKind.ToString() + chassisPriviliges[i].Operation.ToString();
                    if (configElement == null)
                    {
                        config.AppSettings.Settings.Add("Chassis_" + i, value);
                    }
                    else
                    {
                        configElement.Value = value;
                    }
                }

                int index = chassisPriviliges.Length;
                while (config.AppSettings.Settings["Chassis_" + index] != null)
                {
                    config.AppSettings.Settings.Remove("Chassis_" + index);
                    index++;
                }

                //configElement = config.AppSettings.Settings["ResetTags"];
                //if (configElement == null)
                //{
                //    config.AppSettings.Settings.Add("ResetTags", ResetTags.ToString());
                //}
                //else
                //{
                //    configElement.Value = ResetTags.ToString();
                //}

                configElement = config.AppSettings.Settings["UsePlcSimulator"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("UsePlcSimulator", UsePlcSimulator.ToString());
                }
                else
                {
                    configElement.Value = UsePlcSimulator.ToString();
                }

                configElement = config.AppSettings.Settings["PlcPort"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("PlcPort", PlcPort);
                }
                else
                {
                    configElement.Value = PlcPort;
                }

                configElement = config.AppSettings.Settings["InPlaceStatusInterval"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("InPlaceStatusInterval", InPlaceStatusInterval.TotalMilliseconds.ToString());
                }
                else
                {
                    configElement.Value = InPlaceStatusInterval.TotalMilliseconds.ToString();
                }

                configElement = config.AppSettings.Settings["BuzzDuration"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("BuzzDuration", PlcBuzzDuration.TotalSeconds.ToString());
                }
                else
                {
                    configElement.Value = PlcBuzzDuration.TotalSeconds.ToString();
                }

                configElement = config.AppSettings.Settings["PlcCallTimeout"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("PlcCallTimeout", PlcCallTimeout.TotalMilliseconds.ToString());
                }
                else
                {
                    configElement.Value = PlcCallTimeout.TotalMilliseconds.ToString();
                }

                configElement = config.AppSettings.Settings["ExecutionMode"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("ExecutionMode", ExecutionMode.ToString());
                }
                else
                {
                    configElement.Value = ExecutionMode.ToString();
                }

                configElement = config.AppSettings.Settings["ExportFileFormat"];
                if (configElement == null)
                {
                    config.AppSettings.Settings.Add("ExportFileFormat", ExportFileFormat);
                }
                else
                {
                    configElement.Value = ExportFileFormat;
                }

                config.Save();
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
        }

        /// <summary>
        /// Gets the allowed material group on a specified chassis.
        /// </summary>
        /// <param name="chassisNumber">The chassis number.</param>
        /// <param name="materialGroup"></param>
        /// <exception cref="System.ArgumentOutOfRangeException">chassisNumber</exception>
        public void SetChassisMaterialGroup(byte chassisNumber, MaterialKind materialGroup)
        {
            if (chassisNumber > ChassisCount)
            {
                throw new ArgumentOutOfRangeException(
                    "chassisNumber",
                    ChassisCount,
                    String.Format(Resources.ArgumentMustNotBeGreaterMessage, chassisNumber, ChassisCount));
            }

            chassisPriviliges[chassisNumber - 1].MaterialKind = materialGroup;
        }

        /// <summary>
        /// Gets the allowed operations on a specified chassis.
        /// </summary>
        /// <param name="chassisNumber">The chassis number.</param>
        /// <param name="operation"></param>
        /// <exception cref="System.ArgumentOutOfRangeException">chassisNumber</exception>
        public void SetChassisOperations(byte chassisNumber, IdtOperationPrivileges operation)
        {
            if (chassisNumber > ChassisCount)
            {
                throw new ArgumentOutOfRangeException(
                    "chassisNumber",
                    ChassisCount,
                    String.Format(Resources.ArgumentMustNotBeGreaterMessage, chassisNumber, ChassisCount));
            }

            chassisPriviliges[chassisNumber - 1].Operation = operation;
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Resets configuration parameters to their default values.
        /// </summary>
        private void Reset()
        {
            bool ok = Enum.TryParse(Settings.Default.StationType, out stationType);
            if (ok)
            {
                SetChassis();
            }

            culture = CultureInfo.CurrentCulture;
            stationName = Environment.MachineName;
            inPlaceStatusInterval = Settings.Default.InPlaceStatusInterval;
            string drive = "C" + Path.VolumeSeparatorChar + Path.DirectorySeparatorChar;
            usePlcSimulator = Settings.Default.UsePlcSimulator;
            plcCallTimeout = Settings.Default.PlcCallTimeout;
            plcBuzzDuration = Settings.Default.PlcBuzzTime;
            plcPort = Settings.Default.PlcPort;
            executionMode = ExecutionMode.Normal;
            exportFileFormat = "xslx";
        }

        /// <summary>
        /// Sets the chassis configuration according to station type.
        /// </summary>
        private void SetChassis()
        {
            switch (stationType)
            {
                case BssStation.Standalone:
                    chassisPriviliges = new ChassisPrivilege[StandaloneStationChassisCount];
                    for (int i = 0; i < chassisPriviliges.Length; i++)
                    {
                        chassisPriviliges[i] = new ChassisPrivilege
                        {
                            Operation = IdtOperationPrivileges.B | IdtOperationPrivileges.V,
                            MaterialKind = (i >= chassisPriviliges.Length / 2) ? MaterialKind.S : MaterialKind.M
                        };
                    }

                    break;

                case BssStation.Inline:
                    chassisPriviliges = new ChassisPrivilege[InlineStationChassisCount];
                    for (int i = 0; i < chassisPriviliges.Length; i++)
                    {
                        chassisPriviliges[i] = new ChassisPrivilege
                        {
                            Operation = (i >= chassisPriviliges.Length / 2) ? IdtOperationPrivileges.V : IdtOperationPrivileges.B,
                            MaterialKind = MaterialKind.M | MaterialKind.S
                        };
                    }

                    break;

                default:
                    break;
            }
        }

        #endregion Private Methods

        #region IEquatable<ConfigurationParameters> Members

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// true if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public bool Equals(ConfigurationParameters other)
        {
            if (other == null)
            {
                return false;
            }

            return chassisPriviliges.SequenceEqual(other.chassisPriviliges) &&
                inPlaceStatusInterval.Equals(other.inPlaceStatusInterval) &&
                plcBuzzDuration.Equals(other.plcBuzzDuration) &&
                plcCallTimeout.Equals(other.plcCallTimeout) &&
                String.Equals(plcPort, other.plcPort) &&
                String.Equals(stationName, other.stationName) &&
                stationType.Equals(other.stationType) &&
                usePlcSimulator.Equals(other.usePlcSimulator) &&
                executionMode.Equals(other.executionMode) &&
                String.Equals(exportFileFormat, other.exportFileFormat) &&
                culture.Equals(other.culture);
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
            return Equals(obj as ConfigurationParameters);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode()
        {
            int hashCode = 0;

            hashCode ^= chassisPriviliges.GetHashCode();
            hashCode ^= inPlaceStatusInterval.GetHashCode();
            hashCode ^= plcBuzzDuration.GetHashCode();
            hashCode ^= plcCallTimeout.GetHashCode();
            if (plcPort != null)
            {
                hashCode ^= plcPort.GetHashCode();
            }
            if (stationName != null)
            {
                hashCode ^= stationName.GetHashCode();
            }
            hashCode ^= stationType.GetHashCode();
            hashCode ^= usePlcSimulator.GetHashCode();
            hashCode ^= executionMode.GetHashCode();
            if (exportFileFormat != null)
            {
                hashCode ^= exportFileFormat.GetHashCode();
            }
            hashCode ^= culture.GetHashCode();

            return hashCode;
        }

        #endregion IEquatable<ConfigurationParameters> Members
    }
}