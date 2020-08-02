#include "Debug.h"
#include "battery.h"
#include "hal_adc.h"
#include "utils.h"
// (( 3 * 1.15 ) / (( 2^14 / 2 ) - 1 )) * 1000 (not correct)
// #define MULTI (float) 0.4211939934
// this coefficient calculated using
// https://docs.google.com/spreadsheets/d/1qrFdMTo0ZrqtlGUoafeB3hplhU3GzDnVWuUK4M9OgNo/edit?usp=sharing
#define MULTI (float)0.443

#define VOLTAGE_MIN 2.0
#define VOLTAGE_MAX 3.3

uint8 getBatteryVoltageZCL(uint16 millivolts) {
    uint8 volt8 = (uint8)(millivolts / 100);
    if ((millivolts - (volt8 * 100)) > 50) {
        return volt8 + 1;
    } else {
        return volt8;
    }
}
// return millivolts
uint16 getBatteryVoltage(void) {
    HalAdcSetReference(HAL_ADC_REF_125V);
    double rawADC = adcReadSampled(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_125V, 10);
    return (uint16)(rawADC * MULTI);
}

uint8 getBatteryRemainingPercentageZCL(uint16 millivolts) { return (uint8) mapRange(VOLTAGE_MIN, VOLTAGE_MAX, 0.0, 200.0, millivolts); }


uint8 getBatteryRemainingPercentageZCLCR2032(uint16 volt16) {
    float battery_level;
    if (volt16 >= 3000) {
        battery_level = 100;
    } else if (volt16 > 2900) {
        battery_level = 100 - ((3000 - volt16) * 58) / 100;
    } else if (volt16 > 2740) {
        battery_level = 42 - ((2900 - volt16) * 24) / 160;
    } else if (volt16 > 2440) {
        battery_level = 18 - ((2740 - volt16) * 12) / 300;
    } else if (volt16 > 2100) {
        battery_level = 6 - ((2440 - volt16) * 6) / 340;
    } else {
        battery_level = 0;
    }
    return (uint8)(battery_level * 2);
}