DEFINE_MAINTENANCE_COUNTER(TOTAL_PRINTING_TIME,"Total printing time","",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(DIRT_PUMP,"Waste Pump","Pumps",3500,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_1_FILLING_M0_M1,"Model 1 Left Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_2_FILLING_M0_M1,"Model 1 Right Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_3_FILLING_M2_M3,"Model 2 Left Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_4_FILLING_M2_M3,"Model 2 Right Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_5_FILLING_M4_M5,"Model 3 Left Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_6_FILLING_M4_M5,"Model 3 Right Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_1_FILLING,"Support 1 Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_2_FILLING,"Support 2 Pump","Pumps",300,false,true)
DEFINE_MAINTENANCE_COUNTER(CARBON_FILTER,"Carbon Filter","Filters",3000,false,true)
DEFINE_MAINTENANCE_COUNTER(RIGHT_UV_LAMP,"Right UV Lamp","UV Lamps",1500,false,false)
DEFINE_MAINTENANCE_COUNTER(LEFT_UV_LAMP,"Left UV Lamp","UV Lamps",1500,false,false)
DEFINE_MAINTENANCE_COUNTER(VACUUM_FILTER,"Vacuum Filter","Filters",12000,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_S0,"Head 0 (Support)","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_S1,"Head 1","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M4,"Head 2","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M5,"Head 3","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M3,"Head 4","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M2,"Head 5","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M1,"Head 6","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(HEAD_M0,"Head 7 (Model)","Print Heads",-1,false,false)
DEFINE_MAINTENANCE_COUNTER(ROLLER_DIRT_PUMP,"Roller Pump","Pumps",-1,false,True)
DEFINE_MAINTENANCE_COUNTER(DIRT_PUMPS_TUBES,"Waste And Roller Tubes","Pump Tubes",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(VACUUM_PUMP,"Vacuum Pump","Pumps",12000,false,true)
DEFINE_MAINTENANCE_COUNTER(WIPER_BLADE,"Wiper Blade","Blade Replacement",1000,false,false)
DEFINE_MAINTENANCE_COUNTER(ROLLER_BLADE,"Roller Blade","Blade Replacement",1000,false,false)
DEFINE_MAINTENANCE_COUNTER(MODEL_1_RESIN_FILTER,"Model 1 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_2_RESIN_FILTER,"Model 1 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_3_RESIN_FILTER,"Model 2 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_4_RESIN_FILTER,"Model 2 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_5_RESIN_FILTER,"Model 3 Left Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_6_RESIN_FILTER,"Model 3 Right Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_1_RESIN_FILTER,"Support 1 Resin Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_2_RESIN_FILTER,"Support 2 Resin Filter","Filters",300,false,true)
DEFINE_MAINTENANCE_COUNTER(HEAD_FAN_FILTER,"Head Fan Filter","Filters",4800,false,true)
DEFINE_MAINTENANCE_COUNTER(RIGHT_UV_LAMP_IGNITION,"UV Lamp Ignition (R)","UV Lamps",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(SERVICE,"PM is Due","",3500,true,false)
DEFINE_MAINTENANCE_COUNTER(MODEL_DRAIN_PUMP_1,"Model 1 Drain Tube","Pump Tubes",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(MODEL_DRAIN_PUMP_2,"Model 2 Drain Tube","Pump Tubes",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_DRAIN_PUMP_1,"Support 1 Drain Tube","Pump Tubes",-1,false,true)
DEFINE_MAINTENANCE_COUNTER(SUPPORT_DRAIN_PUMP_2,"Support 2 Drain Tube","Pump Tubes",-1,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(UV_LAMPS_CALIBRATION,"UV Calibration"," Operator Maintenance",300,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER(LEFT_UV_LAMP_IGNITION,"UV Lamp Ignition (L)","UV Lamps",-1,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(LAST_HCW_ACTIVATION_TIME,"Heads Cleaning"," Operator Maintenance",170,true,false,"Maintenance Required") //itamar, Super purge
DEFINE_MAINTENANCE_COUNTER(ROLLER_SUCTION_PUMP,"Roller Suction Pump","Pumps",3500,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HS_TIME_SINCE_LAST_HOW,"High Speed Mode","Head Optimization",TIME_AFTER_HSW_WARNING_TIME,false,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER(SUP705_TIME_AFTER_HSW,""," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,false,true)
DEFINE_MAINTENANCE_COUNTER(SUP706_TIME_AFTER_HSW,""," Operator Maintenance",TIME_AFTER_HSW_WARNING_TIME,false,true)
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(PATTERN_TEST,"Pattern Test"," Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(ROLLER_BATH,"Roller Bath Inspection"," Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(WIPER_CLEANING,"Wiper Cleaning", " Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HEAD_ALIGNMENT,"Head Alignment"," Operator Maintenance",300,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(LOAD_CELL_CALIBRATION,"Load Cell Calibration"," Operator Maintenance",500,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(RESTART_COMPUTER,"Restart Computer"," Operator Maintenance",170,true,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(HQ_TIME_SINCE_LAST_HOW,"High Quality Mode","Head Optimization",TIME_AFTER_HSW_WARNING_TIME,false,false,"Maintenance Required")
DEFINE_MAINTENANCE_COUNTER_WITH_DISPLAY_NAME(DM_TIME_SINCE_LAST_HOW,"Digital Materials Mode","Head Optimization",TIME_AFTER_HSW_WARNING_TIME,true,false,"Maintenance Required")