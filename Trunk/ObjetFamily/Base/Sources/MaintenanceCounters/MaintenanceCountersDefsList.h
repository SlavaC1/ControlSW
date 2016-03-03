DEFINE_MAINTENANCE_COUNTER(TOTAL_PRINTING_TIME,"Total printing time","",-1,false,false)

//OBJET_MACHINE config

#ifdef OBJET_1000
	DEFINE_MAINTENANCE_COUNTER(WASTE_PUMP_LEFT,"Waste Pump Left","General Pumps",3500,false,false)
	DEFINE_MAINTENANCE_COUNTER(MODEL_1_PUMP,"Model 1 Left Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(MODEL_2_PUMP,"Model 1 Right Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(MODEL_3_PUMP,"Model 2 Left Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(MODEL_4_PUMP,"Model 2 Right Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(MODEL_5_PUMP,"Model 3 Left Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(MODEL_6_PUMP,"Model 3 Right Pump","Container Pumps",2500,false,true)
#else
	  DEFINE_MAINTENANCE_COUNTER(WASTE_PUMP_LEFT,"Purge Unit Pump","General Pumps",3500,false,false)
	  DEFINE_MAINTENANCE_COUNTER(MODEL_1_PUMP,"Model 1 Left Pump","Container Pumps",300,false,true)
	  DEFINE_MAINTENANCE_COUNTER(MODEL_2_PUMP,"Model 1 Right Pump","Container Pumps",300,false,true)
	  DEFINE_MAINTENANCE_COUNTER(MODEL_3_PUMP,"Model 2 Left Pump","Container Pumps",300,false,true)
	  DEFINE_MAINTENANCE_COUNTER(MODEL_4_PUMP,"Model 2 Right Pump","Container Pumps",300,false,true)
	  DEFINE_MAINTENANCE_COUNTER(MODEL_5_PUMP,"Model 3 Left Pump","Container Pumps",300,false,true)
      DEFINE_MAINTENANCE_COUNTER(MODEL_6_PUMP,"Model 3 Right Pump","Container Pumps",300,false,true)
#endif

#ifdef OBJET_MACHINE_KESHET
DEFINE_MAINTENANCE_COUNTER(MODEL_7_PUMP,"Model 4 Left Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_8_PUMP,"Model 4 Right Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_9_PUMP,"Model 5 Left Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_10_PUMP,"Model 5 Right Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_11_PUMP,"Model 6 Left Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_12_PUMP,"Model 6 Right Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_13_PUMP,"Support 3 Pump","Container Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_14_PUMP,"Support 4 Pump","Container Pumps",300,false,true)
#endif

#ifdef OBJET_1000
	DEFINE_MAINTENANCE_COUNTER(SUPPORT_1_PUMP,"Support 1 Pump","Container Pumps",2500,false,true)
	DEFINE_MAINTENANCE_COUNTER(SUPPORT_2_PUMP,"Support 2 Pump","Container Pumps",2500,false,true)
#else
	DEFINE_MAINTENANCE_COUNTER(SUPPORT_1_PUMP,"Support 1 Pump","Container Pumps",300,false,true)
	DEFINE_MAINTENANCE_COUNTER(SUPPORT_2_PUMP,"Support 2 Pump","Container Pumps",300,false,true)
#endif
DEFINE_MAINTENANCE_COUNTER(CARBON_FILTER,"Carbon Filter","Filters",3000,false,true)
DEFINE_MAINTENANCE_COUNTER(RIGHT_UV_LAMP,"Right UV Lamp","UV Lamps",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(LEFT_UV_LAMP,"Left UV Lamp","UV Lamps",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(VACUUM_FILTER,"Vacuum Filter","Filters",12000,false,true)

DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HEAD_S0,"H0","Print Heads",600,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HEAD_S1,"H1","Print Heads",600,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER(HEAD_M4,"H2","Print Heads",1200,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M5,"H3","Print Heads",1200,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M3,"H4","Print Heads",1200,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M2,"H5","Print Heads",1200,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M1,"H6","Print Heads",1200,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M0,"H7","Print Heads",1200,false,false)

#ifdef OBJET_1000
	DEFINE_MAINTENANCE_COUNTER(ROLLER_DIRT_PUMP,"Roller dirt Pump","General Pumps",3500,false,false)
	DEFINE_MAINTENANCE_COUNTER(DIRT_PUMPS_TUBES,"Purge And Roller Tubes","General Pumps",1500,true,false)
#else
	DEFINE_MAINTENANCE_COUNTER(ROLLER_DIRT_PUMP,"Roller dirt Pump","General Pumps",3500,false,false)
	DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(DIRT_PUMPS_TUBES,"Purge Unit Pump Tube","General Pumps",1500,true,false,"Maintenance Required")
#endif
DEFINE_MAINTENANCE_COUNTER(VACUUM_PUMP,"Vacuum Pump","General Pumps",12000,false,true)
#ifdef OBJET_1000
	DEFINE_MAINTENANCE_COUNTER(WIPER_BLADE,"Wiper Blade","Blade Replacement",500,true,false)
	DEFINE_MAINTENANCE_COUNTER(ROLLER_BLADE,"Roller Blade","Blade Replacement",1000,true,false)
#else
	DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(WIPER_BLADE,"Wiper Blade","Blade Replacement",500,true,false,"Maintenance Required")
	DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(ROLLER_BLADE,"Roller Blade","Blade Replacement",1000,true,false,"Maintenance Required")
#endif

DEFINE_MAINTENANCE_COUNTER(MODEL_1_RESIN_FILTER,"Model 1 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_2_RESIN_FILTER,"Model 1 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_3_RESIN_FILTER,"Model 2 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_4_RESIN_FILTER,"Model 2 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_5_RESIN_FILTER,"Model 3 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_6_RESIN_FILTER,"Model 3 Right Filter","Filters",300,false,true)

#ifdef OBJET_MACHINE_KESHET
DEFINE_MAINTENANCE_COUNTER(MODEL_7_RESIN_FILTER,"Model 4 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_8_RESIN_FILTER,"Model 4 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_9_RESIN_FILTER,"Model 5 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_10_RESIN_FILTER,"Model 5 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_11_RESIN_FILTER,"Model 6 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_12_RESIN_FILTER,"Model 6 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_13_RESIN_FILTER,"Support 3 L Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_14_RESIN_FILTER,"Support 4 R Filter","Filters",300,false,true)
#endif

DEFINE_MAINTENANCE_COUNTER(SUPPORT_1_RESIN_FILTER,"Support 1 L Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_2_RESIN_FILTER,"Support 2 R Filter","Filters",300,false,true)

DEFINE_MAINTENANCE_COUNTER(HEAD_FAN_FILTER,"Head Fan Filter","Filters",4800,false,true)
DEFINE_MAINTENANCE_COUNTER(RIGHT_UV_LAMP_IGNITION,"UV Lamp Ignition (R)","UV Lamps",-1,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(SERVICE,"PM is Due","",3500,true,false,"Maintenance Required")
/*DEFINE_MAINTENANCE_COUNTER(MODEL_DRAIN_PUMP_1,"Model 1 Drain Tube","Container Pumps",300,false,false)
DEFINE_MAINTENANCE_COUNTER(MODEL_DRAIN_PUMP_2,"Model 2 Drain Tube","Container Pumps",300,false,false)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_DRAIN_PUMP_1,"Support 1 Drain Tube","Container Pumps",300,false,false)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_DRAIN_PUMP_2,"Support 2 Drain Tube","Container Pumps",300,false,false)         */
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(UV_LAMPS_CALIBRATION,"UV Calibration"," Operator Maintenance",300,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER(LEFT_UV_LAMP_IGNITION,"UV Lamp Ignition (L)","UV Lamps",-1,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(LAST_HCW_ACTIVATION_TIME,"Cleaning wizard"," Operator Maintenance",170,true,false,"Maintenance Required") //itamar, Super purge
//DEFINE_MAINTENANCE_COUNTER(ROLLER_SUCTION_PUMP,"Roller Suction Pump","General Pumps",3500,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_OPTIMIZATION_WIZARD,"","",-1,false,true)

#ifdef OBJET_1000
DEFINE_MAINTENANCE_COUNTER(WASTE_PUMP_RIGHT,"Waste Pump Right","General Pumps",3500,false,true)
#endif
DEFINE_MAINTENANCE_COUNTER(SUP705_TIME_AFTER_HSW,""," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,false,true)
DEFINE_MAINTENANCE_COUNTER(SUP706_TIME_AFTER_HSW,""," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,false,true)

DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(PATTERN_TEST,"Pattern Test"," Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(ROLLER_BATH,"Roller Bath Inspection"," Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HEAD_ALIGNMENT,"Head Alignment"," Operator Maintenance",300,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(LOAD_CELL_CALIBRATION,"Load Cell Calibration"," Operator Maintenance",500,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(RESTART_COMPUTER,"Restart Computer"," Operator Maintenance",720,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HOW_HS_HM,"HOW HS/HM"," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HOW_HQ,"HOW HQ"," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(ROLLER_PUMP_TUBES,"Roller Dirt Pump Tube","General Pumps",1500,true,false,"Maintenance Required")

DEFINE_MAINTENANCE_COUNTER(HEAD_S0_DC,"H0_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_S1_DC,"H1_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M4_DC,"H2_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M5_DC,"H3_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M3_DC,"H4_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M2_DC,"H5_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M1_DC,"H6_DropCounter","Print Heads",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_M0_DC,"H7_DropCounter","Print Heads",-1,false,true)

DEFINE_MAINTENANCE_COUNTER(CHANNEL_M1_SLEEPING,"Channel M1 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(CHANNEL_M2_SLEEPING,"Channel M2 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(CHANNEL_M3_SLEEPING,"Channel M3 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(CHANNEL_M4_SLEEPING,"Channel M4 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(CHANNEL_M5_SLEEPING,"Channel M5 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(CHANNEL_M6_SLEEPING,"Channel M6 Is Asleep"," Operator Maintenance",-1,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(CHANNEL_SLEEPING_MAX_TIME,"Max Sleep Chnl Time"," Operator Maintenance",-1,true,false,"Maintenance Required")
