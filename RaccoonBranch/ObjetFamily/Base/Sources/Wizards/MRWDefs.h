#ifndef MrwDefsH
#define MrwDefsH

// Tooltips for the summary icons
const QString HINT_OK      = "Cartridge OK.";
const QString HINT_WARNING = "Cartridge is under %d gr. Replace before continuing.";
const QString HINT_ERROR   = "Missing or incorrect cartridge.";
const QString HINT_EXPIRED = "Cartridge has expired.";
const QString HINT_UNAUTHENTICATED = "Cartridge cannot be identified.";

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

const QString WARNING_UNDEFINED_2ND_FLUSHING = "The last material replacement procecudre did not end successfully. "
									  "Please re-execute it in its standard way before attempting to use the second flushing feature";

const QString WARNING_ONE_MATERIAL_DM = "Select at least two different materials in DM mode.";

const QString WARNING_MANUAL_SINGLE   = "Only one material can be selected in this mode.";

const QString WARNING_DM2_INVALID = "In multiple materials printing mode one material must be selected from the ‘Material 3’ drop down list.";

const QString WARNING_CLEANSER        = "Cleaning fluid is not a printable material and the machine won't be able to print until you replace it.\n"
										"Do you wish to continue?";

const QString WARNING_UNDEFINED_SUPPORT = "The support resin cannot be undefined.\n"
										  "Please select a different support resin.";

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


#endif
