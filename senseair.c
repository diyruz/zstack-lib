#include "senseair.h"
#include "Debug.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_led.h"
#include "hal_uart.h"

#ifndef CO2_UART_PORT
#define CO2_UART_PORT HAL_UART_PORT_0
#endif

static void SenseAir_RequestMeasure(void);
static uint16 SenseAir_Read(uint8 *);
static void SenseAir_SetABC(bool isEnabled);

extern zclAirSensor_t sense_air_dev = {&SenseAir_RequestMeasure, &SenseAir_Read, &SenseAir_SetABC};

#define SENSEAIR_RESPONSE_LENGTH 13

uint8 readCO2[] = {0xFE, 0x04, 0x00, 0x00, 0x00, 0x04, 0xE5, 0xC6};
uint8 disableABC[] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xAC, 0x03};
uint8 enableABC[] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0xB4, 0xAC, 0x74};

void SenseAir_SetABC(bool isEnabled) {
    if (isEnabled) {
        HalUARTWrite(CO2_UART_PORT, enableABC, sizeof(enableABC) / sizeof(enableABC[0]));
    } else {
        HalUARTWrite(CO2_UART_PORT, disableABC, sizeof(disableABC) / sizeof(disableABC[0]));
    }
}

void SenseAir_RequestMeasure(void) {
    HalUARTWrite(CO2_UART_PORT, readCO2, sizeof(readCO2) / sizeof(readCO2[0])); 
}

uint16 SenseAir_Read(uint8 *response) {
    
    LREPMaster("Read startet \r\n");
 
    uint8 is_equal = 1;
    for (uint8 i=0; i<5; ++i) {
      if (response[i] != enableABC[i]) {
          is_equal = 0;
          break;
        }
    }
    
    if (is_equal)
      return AIR_QUALITY_ABC_RESPONSE;
       
    if (response[0] != 0xFE || response[1] != 0x04) {
        LREPMaster("Invalid response\r\n");
        return AIR_QUALITY_INVALID_RESPONSE;
    }

    const uint8 length = response[2];
    //const uint16 status = (((uint16)response[3]) << 8) | response[4];
    uint16 ppm = (((uint16)response[length + 1]) << 8) | response[length + 2];
    //LREP("SenseAir Received CO2=%d ppm Status=0x%X\r\n", ppm, status);
    //printf("SenseAir Received CO2=%d ppm Status=0x%X\r\n", ppm, status);
    return ppm;
}