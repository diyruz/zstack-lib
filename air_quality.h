#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#define AIR_QUALITY_INVALID_RESPONSE 0xFFFF
#define AIR_QUALITY_ABC_RESPONSE 0xFFF0

typedef void (*request_measure_t)(void);
typedef uint16 (*read_t)(uint8 *);
typedef void (*set_ABC_t)(bool isEnabled);

typedef struct {
  request_measure_t RequestMeasure;
  read_t Read;
  set_ABC_t SetABC;
} zclAirSensor_t;

#endif //AIR_QUALITY_H