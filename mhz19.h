#ifndef mhz19_h
#define mhz19_h
extern void MHZ19_RequestMeasure(void);
extern void MHZ19_Read(uint16 *ppm);
extern void MHZ19_SetABC(bool isEnabled);
#endif