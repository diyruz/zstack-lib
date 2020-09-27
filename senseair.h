#ifndef SENSEAIR_H
#define SENSEAIR_H
extern void zclApp_SenseAirRequestMeasure(void);
extern uint16 zclApp_SenseAirRead(void);
extern void zclApp_SenseAirSetABC(bool isEnabled);
#endif