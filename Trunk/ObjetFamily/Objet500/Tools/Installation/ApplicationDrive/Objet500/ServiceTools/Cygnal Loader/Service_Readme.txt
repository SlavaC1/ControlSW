		---------------------------------------------------------
                 Cygnal Loader Instructions version 3.0 Readme File
                     Instructions For Objet Applications Support 
                                    11/2005 AVIC 
                ---------------------------------------------------------



This document provides information with regards to how to load new software into OHDB card.


 ----------------------------------------------------------------
| Important Note:                                                |
|                                                                | 
| The loading prodecure should be as follows:                    | 
|                                                                | 
| 1. Load OHDB_A2D.HEX while connector is plugged to J8          |
|                                                                |
| 2. Load OHDB.HEX while connector is plugged to J10             |
|                                                                | 
 ----------------------------------------------------------------


 ----------------------------------------------------------------------------------
| Attention:                                                                       |
|                                                                                  | 
| The file Load.Bat is a batch file includes the COM3 as default.                  | 
|                                                                                  | 
| The file includes the following line: FlashUtilCl download -RE %1 3 0            |
|                                                                                  |
| If you wish to run only from COM2 instaed of COM3, update the line as follows:   |
|                                                                                  |
|  FlashUtilCl download -RE %1 2 0                                                 |
|                                                                                  | 
 ----------------------------------------------------------------------------------


 

   How to load, and loading instructions:

1. Connect the Cygnal COM cable into a nearby computer (Host or laptop).
2. Note to connect the serial cable into COM1 (if you use other COM, please refer to section 14 below).
3. Make sure that the machine is ON and the Eden application is activated.
4. In order to read you current OHDB version, you may refer to 'Help' 'About' screen, or to 'QMonitor' first lines.
5. Connect the other side of the 'Cygnal Loader' into connector J10 (or J8, depends on the order listed above) on the OHDB front. 
6. Important Note: There are TWO similar connectors for Cygnal adapator on the OHDB. J8 and J10; 
7. After connecting the Cygnal adpator to COM1 at one end and to J10 on the OHDB card - 
the power LED in the Cygnal unit will turn ON.
8. In the selected computer (Host or laptop) - go to 'Start', 'Run', and type 'CMD'. The DOS prompt screen will appear.
9. Go to the directory where the Loader files are located. (i.e. If the loader files are at C:\Eden\Loader\; 
type CD Eden\Loader etc.).
10. Type the activation line: Load OHDB_A2D.HEX (where the OHDB_A2D.HEX is the file name in this case that you want to load).
11. Loading will take about 1 mintue, where the 'Run\Stop' LED on the Cygnal device will also turn ON.
12. When done, remove the cables from the OHDB and the computer.
13. Restart the Eden application and ensure having the right OHDB version in the 'Help' 'About' screen.












Updated on November 15, 2005 by Avi Cohen.

Copyright © by Objet Geometries Ltd. All rights reserved Ò. 
All rights reserved. No part of this file may be used or reproduced in any form or by any means, nor stored in a database 
or retrieval system, without prior permission in writing from Objet Geometries Ltd. Avi_Cohen@2objet.com
All the terms mentioned in this file that are known to be trademarks or service marks have been appropriately capitalized.  
Use of a term in this file should not be regarded as affecting the validity of any trademark or service mark.  
All other product names and services identified throughout this file are trademarks or registered trademarks of their 
respective companies.
