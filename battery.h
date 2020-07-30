#ifndef _BATTERY_H
#define _BATTERY_H
extern uint16 getBatteryVoltage(void);
extern uint8 getBatteryVoltageZCL(uint16 millivolts);
extern uint8 getBatteryRemainingPercentageZCL(uint16 millivolts);
#endif