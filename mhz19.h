#ifndef mhz19_h
#define mhz19_h

#define MHZ18_INVALID_RESPONSE 0XFFFF

extern void MHZ19_RequestMeasure(void);
extern void MHZ19_Read(uint16 *ppm);
extern void MHZ19_SetABC(bool isEnabled);
#endif