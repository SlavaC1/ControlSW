using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using RaccoonCLI;
using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Responsible for correcting material info into IDT when cartridge is inserted.
    /// </summary>
    public class IdtCorrector : IdtBurner
    {
        #region Private Fields

        /// <summary>
        /// The information of tag being corrected.
        /// </summary>
        private TagInfo tagInfoOnCorrection;

        private bool terminateOperation;

        /// <summary>
        /// The user confirmation predicate.
        /// </summary>
        private Predicate<string> userConfirmationPredicate;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtCorrector"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        public IdtCorrector(ConfigurationParameters configurationParameters, InPlaceManager inPlaceManager, MaterialMonitorWrapper materialMonitor, IPlc plcWrapper)
            : base(configurationParameters, inPlaceManager, materialMonitor, plcWrapper)
        {
            userConfirmationPredicate = (s) => true;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the user confirmation predicate.
        /// </summary>
        /// <value>
        /// The user confirmation predicate.
        /// </value>
        public Predicate<string> UserConfirmationPredicate
        {
            get
            {
                return userConfirmationPredicate;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                userConfirmationPredicate = value;
            }
        }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets a value indicating whether terminate session if operation fails.
        /// </summary>
        /// <value>
        /// <c>true</c> if terminate session if operation fails; otherwise, <c>false</c>.
        /// </value>
        protected override bool TerminateOperation
        {
            get
            {
                return terminateOperation;
            }
            set
            {
            }
        }

        #endregion Protected Properties

        #region Protected Methods

        /// <summary>
        /// Burns material info into cartridge specified by cartridge number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="tagInfo"></param>
        /// <returns>The tag information burned on cartridge.</returns>
        protected async override Task<TagInfo> Burn(byte cartridgeNumber)
        {
            TagInfo tagInfo = null;
            terminateOperation = true;

            try
            {
                tagInfoOnCorrection = null;
                tagInfo = AuthenticateCartridge(cartridgeNumber).Result;
                tagInfoOnCorrection = tagInfo;
                OnTagInfoRead(new TagInfoEventArgs(cartridgeNumber, tagInfo));

                bool ok = UserConfirmationPredicate(Resources.OverrideTagConfirmation);
                if (!ok)
                {
                    terminateOperation = false;
                    return null;
                }

                return await base.Burn(cartridgeNumber);
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
                return tagInfo;
            }
            finally
            {
                tagInfoOnCorrection = null;
            }
        }

        /// <summary>
        /// Gets the actual IDT operation to be executed on cartrdge.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns></returns>
        protected override IdtOperation GetActualOperation(byte cartridgeNumber)
        {
            IdtOperation operation = base.GetActualOperation(cartridgeNumber);
            if (operation == IdtOperation.Burning)
            {
                return IdtOperation.Correction;
            }

            return operation;
        }

        /// <summary>
        /// Gets the name of the IDT operation.
        /// </summary>
        /// <returns></returns>
        protected override string GetOperationName()
        {
            return Resources.CorrectionState;
        }

        /// <summary>
        /// Gets the weight to decrease from tag counter initial value.
        /// </summary>
        /// <param name="serialNumber">The serial number.</param>
        /// <returns>
        /// The weight to decrease if operation succeeded; otherwise, <c>null</c>.
        /// </returns>
        protected override async Task<uint?> GetWeightToDecrease(byte[] serialNumber)
        {
            uint? weightToDecrease = null;
            if (tagInfoOnCorrection != null &&
                LotStatistics.MaterialInfo != null)
            {
                uint totalWeight = LotStatistics.MaterialInfo.GetTotalWeight();
                if (tagInfoOnCorrection.CurrentMaterialWeight < totalWeight)
                {
                    weightToDecrease = 0;
                }
                else
                {
                    weightToDecrease = tagInfoOnCorrection.CurrentMaterialWeight - totalWeight;
                }
            }

            return await Task<uint?>.FromResult(weightToDecrease);
        }

        /// <summary>
        /// Determines whether tag registration is valid.
        /// </summary>
        /// <param name="TagRegistration">The tag registration value.</param>
        /// <returns>
        ///   <c>true</c> if tag registration is new or read; otherwise, <c>false</c>.
        /// </returns>
        protected override bool IsTagRegistrationValid(TagRegistration TagRegistration)
        {
            return TagRegistration != TagRegistration.Unknown;
        }

        #endregion Protected Methods
    }
}