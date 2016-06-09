Bradwii for the Micro Drone 3.0. 
=======

#### The MD3.0 uses the following hardware
 * STM32F031K4 ARM Cortex-M0
 * XN297 2.4GHz transceiver
 * InvenSense MPU-6050 6-Axis MEMS Gyroscope & Accelerometer


Manual accelerometer calibration:
 * Quadcopter must be on level surface
 * Quadcopter must be in "not armed" state
 * Throttle stick at minimum
 * Move roll stick 3 times left and right
 * LEDs blink in circular pattern to indicate calibration process. When finished, results are stored in data flash.

#### Development issues:

When burning a firmware with new PID control parameters, checkboxconfig or anything else from the usersettings struct make sure to erase the data flash.
Otherwise the firmware will continue to use the old data. 


Credits
======
 * RCGROUPS!
 * Bradwii was originally coded by Brad Quick for AVR: https://github.com/bradquick/bradwii
 * Trollcop forked and ported to ARM STM32, untested: https://github.com/trollcop/bradwii
 * The Mini54ZAN ARM port to V202/JD385 was done by Victor: https://github.com/victzh/bradwii
 * The Hubsan X4 H107L port was done by Goebish: https://github.com/goebish/bradwii-X4
 * Extra work on the H107L port by TheLastMutt: https://github.com/TheLastMutt/bradwii-x4-gcc
