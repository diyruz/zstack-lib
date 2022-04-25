#include "Debug.h"
#include "us100.h"
#include "OnBoard.h"

#ifndef US100_UART_PORT
#define US100_UART_PORT HAL_UART_PORT_0
#endif


uint8 readDistance[] = {0x55};
uint8 readTemp[] = {0x50};

void US100_RequestMeasureDistance(void) {
    current_phase = DISTANCE_MEASURMENT;
    HalUARTWrite(US100_UART_PORT, readDistance, sizeof(readDistance) / sizeof(readDistance[0])); 
}
void US100_RequestMeasureTemp(void) {
    current_phase = TEMPERATURE_MEASURMENT;
    HalUARTWrite(US100_UART_PORT, readTemp, sizeof(readTemp) / sizeof(readTemp[0])); 
}

uint16 US100_ReadDistance(void)
{

  uint8 response[2];
  HalUARTRead(US100_UART_PORT, (uint8 *)&response, sizeof(response) / sizeof(response[0]));
  uint16 mmDist  = response[0] * 256 + response[1]; 
  if((mmDist > 1) && (mmDist < 10000)) 
    {
      printf("Distance=%d\r\n", mmDist);
      LREP("Distance=%d \r\n", 0);
      return mmDist;
    }
  return 0;
}

uint16 US100_ReadTemp(void){
  uint8 response[1];
  HalUARTRead(US100_UART_PORT, (uint8 *)&response, sizeof(response) / sizeof(response[0]));
  
  uint16 temp = response[0];
  if((temp > 1) && (temp < 130)) // temprature is in range
    {
      temp -= 45; // correct 45º offset
      printf("Temperature=%d\r\n", temp);
    }
return temp;  
}