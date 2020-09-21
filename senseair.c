#include "senseair.h"
#include "Debug.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_uart.h"
#include "hal_led.h"

#ifndef SENSEAIR_UART_PORT
    #define SENSEAIR_UART_PORT HAL_UART_PORT_1
#endif

#define SENSEAIR_RESPONSE_LENGTH 13

uint8 readCO2[] =    {0xFE, 0x04, 0x00, 0x00, 0x00, 0x04, 0xE5, 0xC6};
uint8 disableABC[] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xAC, 0x03};
uint8 enableABC[] =  {0xFE, 0x60, 0x00, 0x1F, 0x00, 0xB4, 0xAC, 0x74};

static void flushUart(void);
static uint16 senseair_checksum(uint8 *ptr, uint8 length);

static void flushUart(void) {
    uint8 response;
    while (Hal_UART_RxBufLen(SENSEAIR_UART_PORT)) {
        HalUARTRead(SENSEAIR_UART_PORT, &response, 1);
    }
}

void zclApp_SenseAirSetABC(bool isDisable) {
    flushUart();
    if (isDisable) {
        HalUARTWrite(SENSEAIR_UART_PORT, disableABC, sizeof(disableABC) / sizeof(disableABC[0]));
    } else {
        HalUARTWrite(SENSEAIR_UART_PORT, enableABC, sizeof(enableABC) / sizeof(enableABC[0]));
    }
}

void zclApp_SenseAirInit(void) {
    halUARTCfg_t halUARTConfig;
    halUARTConfig.configured = TRUE;
    halUARTConfig.baudRate = HAL_UART_BR_9600;
    halUARTConfig.flowControl = FALSE;
    halUARTConfig.flowControlThreshold = 48; // this parameter indicates number of bytes left before Rx Buffer
                                             // reaches maxRxBufSize
    halUARTConfig.idleTimeout = 10;          // this parameter indicates rx timeout period in millisecond
    halUARTConfig.rx.maxBufSize = SENSEAIR_RESPONSE_LENGTH;
    halUARTConfig.tx.maxBufSize = SENSEAIR_RESPONSE_LENGTH;
    halUARTConfig.intEnable = TRUE;
    HalUARTInit();
    if (HalUARTOpen(SENSEAIR_UART_PORT, &halUARTConfig) == HAL_UART_SUCCESS) {
        LREPMaster("Initialized sensair \r\n");
    }
    flushUart();
}
void zclApp_SenseAirRequestMeasure(void) {
    flushUart();
    HalUARTWrite(SENSEAIR_UART_PORT, readCO2, sizeof(readCO2) / sizeof(readCO2[0]));
}
uint16 zclApp_SenseAirRead(void) {

    uint8 response[SENSEAIR_RESPONSE_LENGTH];
    HalUARTRead(SENSEAIR_UART_PORT, (uint8 *)&response, sizeof(response) / sizeof(response[0]));

    if (response[0] != 0xFE || response[1] != 0x04) {
        LREPMaster("Invalid response\r\n");
        HalLedSet(HAL_LED_ALL, HAL_LED_MODE_FLASH);
        return 0;
    }

    uint16 calc_checksum = senseair_checksum(response, 11);
    uint16 resp_checksum = (((uint16)response[12]) << 8) | response[11];
    if (resp_checksum != calc_checksum) {
        LREPMaster("Wrong checksum\r\n");
        HalLedSet(HAL_LED_ALL, HAL_LED_MODE_FLASH);
        return 0;
    }

    const uint8 length = response[2];
    const uint16 status = (((uint16)response[3]) << 8) | response[4];
    const uint16 ppm = (((uint16)response[length + 1]) << 8) | response[length + 2];

    LREP("SenseAir Received CO₂=%d ppm Status=0x%X\r\n", ppm, status);

    return ppm;
}


static uint16 senseair_checksum(uint8 *ptr, uint8 length) {
  uint16 crc = 0xFFFF;
  uint8 i;
  while (length--) {
    crc ^= *ptr++;
    for (i = 0; i < 8; i++) {
      if ((crc & 0x01) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}