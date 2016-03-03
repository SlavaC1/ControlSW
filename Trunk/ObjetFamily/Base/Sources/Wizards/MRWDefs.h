#ifndef MrwDefsH
#define MrwDefsH

// buttons and captions
const QString IDS_ADVANCED_SETTINGS = "Advanced Settings";
const QString IDS_ADVANCED_SETTINGS_BTN = "Advanced Settings...";

// Tooltips for the summary icons
const QString HINT_OK      = "Cartridge OK.";
const QString HINT_WARNING = "Cartridge is under %d gr. Replace before continuing.";
const QString HINT_ERROR   = "Missing or Incorrect cartridge.";
const QString HINT_EXPIRED = "Cartridge has expired.";
const QString HINT_WAITING = "Please wait while the cartridge is stabilizing.";

// Progress bar text messages
const QString ETA_PROGRESS  = "This process will take approximately %d minutes.";
const QString ETA_REMAINING = "Remaining time is approximately %d minutes.";
const QString ETA_COMPLETED = "Successfully completed";

// Pop-up messages
const QString WARNING_START         = "Beyond this point, selecting 'Cancel' or changing/removing any of the material cartridges will abort the wizard and disable printing.\n"
									  "Resuming printing will only be possible after completion of this Wizard.\n"
									  "Do not remove any material cartridges from the printer during this process.";

const QString WARNING_WASTE         = "Empty the printer's material waste container before continuing.";

const QString WARNING_SAME_MATERIAL = "You have selected the same material more than once.\n"
									  "Select a different material to continue.";

const QString WARNING_CANCEL        = "It is not recommended to cancel the material replacement process at this time. "
									  "Cancelling the process will leave the printer in an unknown state, and you will "
									  "need to run this wizard before you print again.\n\nAre you sure you want to cancel?";

const QString WARNING_UNDEFINED_2ND_FLUSHING = "The last material replacement procedure did not end successfully. "
											   "Please re-execute it in its standard way before attempting to use "
											   "the second flushing feature";

const QString WARNING_NUM_MATERIALS = "Select exactly %d different materials in this mode.";

const QString WARNING_CLEANSER        = "Cleaning fluid is not a printable material and the machine cannot print until "
                                        "you replace it.\nAre you sure you want to continue?";
const QString WARNING_UNDEFINED_SUPPORT = "The support material must be defined.\n"
										  "Select at least one support cartridge in SH and SL slots.";
const QString WARNING_MULTIPLE_SUPPORT_RESINS = "Select the same material in SH and SL slots.";

const QString WARNING_RGD515_FROM = "You cannot replace ";

const QString WARNING_RGD515_TO = " with ";

const QString WARNING_ALL_AUTO_PERMS_ARE_ILLEGAL = "You cannot replace the current loaded materials with the selected materials.";

const QString WARNING_SILENT_MODE = "You have selected to replace the materials without running the Material Replacement Wizard.\n"
									"Note that this bypass is not recommended and will not "
									"flush the system with the new materials.\n"
									"Are you sure you want to continue?";

// Welcome screen text
const QString WELCOME_MSG   = "This wizard prepares the printer for producing models with another "
							  "material by first purging the current material, then flushing the printer "
							  "with the new material.\n\nYou can choose between flushing the system using "
							  "the 'economy' cycle or a more thorough cleaning cycle. For more details "
							  "about the options best suited for your needs, click 'Help' in the Material Replacement Options screen.";

const QString MSG_MACHINE_NOT_CALIBRATED  = "Calculated number of washing cycles is less than "
											"number of washing cycles actually performed.\n\n"
											"Please make sure that the machine is calibrated.\n"
											"If the problem persists, there might be a hardware problem.";

// Summary page
const QString ERR_WASTE_CARTRIDGE_NA	= "Important: Insert empty waste cartridge(s) before continuing."; //"Please make sure a waste cartridge is inserted and not full.";
const QString ERR_WASTE_CARTRIDGE_WAIT	= "Please wait while the waste cartridge(s) are stabilizing.";
const QString ERR_WASTE_CARTRIDGE_LOW_LIMIT	= "Important: Empty waste cartridge(s) before continuing."; //"The waste cartridge is lacking %d grams of free space.";

// Troubleshooting
const QString MSG_ERR_INCORRECT_PARAM_VALUE = "An incorrect value was assigned to a parameter.\r\n\r\n"
											  "Please contact customer service for technical support.";

const QString MSG_ERR_CARTRIDGE_CANT_STABILIZE = "The weight of the cartridge located in %s is not stable.\r\n"
												 "Run the Load Cell Calibration Wizard from the\r\n"
												 "Options -> Wizards menu, and then run this wizard again.";
const QString MSG_HOW_WARNING = "To ensure the best printing quality, it is highly recommended to run the Head Optimization Wizard before printing.";
#endif
