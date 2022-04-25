#ifndef us100_h
void US100_RequestMeasureDistance(void);
void US100_RequestMeasureTemp(void);
uint16 US100_ReadDistance(void);
uint16 US100_ReadTemp(void);

enum {
  DISTANCE_MEASURMENT = 0,
  TEMPERATURE_MEASURMENT = 1,
  NOTHING = 2
};

extern uint8 current_phase; 

#define us100_h
#endif
