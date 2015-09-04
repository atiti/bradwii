Bradwii for Hubsan H107L v2.3. 
=======

#### The H107L uses the following hardware
 * Nuvoton MINI54ZDE ARM Cortex-M0
 * AMICCOM A7105 2.5GHz transceiver
 * mCube MC3210 3-Axis Accelerometer
 * InvenSense MPU-3050 3-Axis MEMS Gyroscope

####Also supports JJRC & JXD nRF quadcopters.
Tested on JJRC H6C, JXD 385/388
 
Manual accelerometer calibration:
 * Quadcopter must be on level surface
 * Quadcopter must be in "not armed" state
 * Throttle stick at minimum
 * Move roll stick 3 times left and right
 * LEDs blink in circular pattern to indicate calibration process. When finished, results are stored in data flash.

#### Development issues:

When burning a firmware with new PID control parameters, checkboxconfig or anything else from the usersettings struct make sure to erase the data flash.
Otherwise the firmware will continue to use the old data. 

Serial Port:
http://www.rcgroups.com/forums/showthread.php?t=2278850&page=34
If you can solder real small :)

####Don't forget to edit the config.h for your model. Setup gyro/pid/triggers,etc

Credits
======
 * RCGROUPS!
 * Bradwii was originally coded by Brad Quick for AVR: https://github.com/bradquick/bradwii
 * Trollcop forked and ported to ARM STM32, untested: https://github.com/trollcop/bradwii
 * The Mini54ZAN ARM port to V202/JD385 was done by Victor: https://github.com/victzh/bradwii
 * The Hubsan X4 H107L port was done by Goebish: https://github.com/goebish/bradwii-X4
 * Extra work on the H107L port by TheLastMutt: https://github.com/TheLastMutt/bradwii-x4-gcc
 * silverxxx (imu/flysky) - woogluf (flysky)