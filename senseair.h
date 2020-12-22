#ifndef SENSEAIR_H
#define SENSEAIR_H

#define SENSEAIR_INVALID_RESPONSE 0xFFFF

extern void SenseAir_RequestMeasure(void);
extern void SenseAir_Read(uint16 *ppm);
extern void SenseAir_SetABC(bool isEnabled);
#endif