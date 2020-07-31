/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "hal_key.h"
#include "Debug.h"
#include "OnBoard.h"
#include "utils.h"

#include "hal_adc.h"
#include "hal_defs.h"
#include "hal_drivers.h"
#include "hal_led.h"
#include "hal_mcu.h"
#include "hal_types.h"
#include "osal.h"

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/





#define HAL_KEY_P0_EDGE_BITS HAL_KEY_BIT0
#define HAL_KEY_P1_EDGE_BITS (HAL_KEY_BIT1 | HAL_KEY_BIT2)
#define HAL_KEY_P2_EDGE_BITS HAL_KEY_BIT3



#if defined(HAL_BOARD_FLOWER)
#define HAL_KEY_P0_INPUT_PINS 0x00
#define HAL_KEY_P1_INPUT_PINS 0x00
#define HAL_KEY_P2_INPUT_PINS (HAL_KEY_BIT0)

#elif defined(HAL_BOARD_CHDTECH_DEV)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT1 | HAL_KEY_BIT6)
#define HAL_KEY_P0_INPUT_PINS_EDGE HAL_KEY_FALLING_EDGE

#define HAL_KEY_P1_INPUT_PINS 0x00
#define HAL_KEY_P1_INPUT_PINS_EDGE HAL_KEY_FALLING_EDGE

#define HAL_KEY_P2_INPUT_PINS (HAL_KEY_BIT0)
#define HAL_KEY_P2_INPUT_PINS_EDGE HAL_KEY_FALLING_EDGE

#endif

/**************************************************************************************************
 *                                            TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/
static halKeyCBack_t pHalKeyProcessFunction;
bool Hal_KeyIntEnable;
/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
void halProcessKeyInterrupt(uint8 portNum);

void HalKeyPoll(void) {}
void HalKeyInit(void) {
#if HAL_KEY_P0_INPUT_PINS
    P0SEL &= ~HAL_KEY_P0_INPUT_PINS;
    P0DIR &= ~(HAL_KEY_P0_INPUT_PINS);
#endif

#if HAL_KEY_P1_INPUT_PINS
    P1SEL &= ~HAL_KEY_P1_INPUT_PINS;
    P1DIR &= ~(HAL_KEY_P1_INPUT_PINS);
#endif

#if HAL_KEY_P2_INPUT_PINS
    P2SEL &= ~HAL_KEY_P2_INPUT_PINS;
    P2DIR &= ~(HAL_KEY_P2_INPUT_PINS);
#endif

    pHalKeyProcessFunction = NULL;
}

void HalKeyConfig(bool interruptEnable, halKeyCBack_t cback) {
    Hal_KeyIntEnable = true;
    pHalKeyProcessFunction = cback;
#if HAL_KEY_P0_INPUT_PINS
    P0IEN |= HAL_KEY_P0_INPUT_PINS;
    IEN1 |= HAL_KEY_BIT5;            // enable port0 int
    P0INP &= ~HAL_KEY_P0_INPUT_PINS; // Pullup/pulldown

#if (HAL_KEY_P0_INPUT_PINS_EDGE == HAL_KEY_FALLING_EDGE)
    P2INP &= ~HAL_KEY_BIT5; // pull up
    MicroWait(50);
    PICTL |= HAL_KEY_P0_EDGE_BITS; // set falling edge on port
#else
    P2INP |= HAL_KEY_BIT5; // pull down
    MicroWait(50);
    PICTL &= ~(HAL_KEY_P0_EDGE_BITS);
#endif

#endif

#if HAL_KEY_P1_INPUT_PINS
    P1IEN |= HAL_KEY_P1_INPUT_PINS;
    IEN2 |= HAL_KEY_BIT4; // enable port1 int
#if (HAL_KEY_P1_INPUT_PINS_EDGE == HAL_KEY_FALLING_EDGE)
    PICTL |= HAL_KEY_P1_EDGE_BITS; // set falling edge on port
#else
    PICTL &= ~HAL_KEY_P1_EDGE_BITS;
#endif

#endif

#if HAL_KEY_P2_INPUT_PINS
    P2IEN |= HAL_KEY_P2_INPUT_PINS;
    IEN2 |= HAL_KEY_BIT1; // enable port2 int
#if (HAL_KEY_P2_INPUT_PINS_EDGE == HAL_KEY_FALLING_EDGE)
    PICTL |= HAL_KEY_P2_EDGE_BITS; // set falling edge on port
#else
    PICTL &= ~HAL_KEY_P2_EDGE_BITS;
#endif

#endif
}

void halProcessKeyInterrupt(uint8 portNum) {
    uint8 pressedPin = 0;
    bool isPressed = false;
    Onboard_wait(50);
    switch (portNum) {
    case HAL_KEY_PORT0:
        pressedPin = P0IFG & HAL_KEY_P0_INPUT_PINS;
        isPressed = HAL_KEY_P0_INPUT_PINS_EDGE != !!(P0 & HAL_KEY_P0_INPUT_PINS & pressedPin);
        PICTL ^= HAL_KEY_P0_EDGE_BITS; // flip edge bit
        break;

    case HAL_KEY_PORT1:
        pressedPin = P1IFG & HAL_KEY_P1_INPUT_PINS;
        isPressed = HAL_KEY_P1_INPUT_PINS_EDGE != !!(P1 & HAL_KEY_P1_INPUT_PINS & pressedPin);
        PICTL ^= HAL_KEY_P1_EDGE_BITS; // flip edge bit
        break;

    case HAL_KEY_PORT2:
        pressedPin = P2IFG & HAL_KEY_P2_INPUT_PINS;
        isPressed = HAL_KEY_P2_INPUT_PINS_EDGE != !!(P2 & HAL_KEY_P2_INPUT_PINS & pressedPin);
        PICTL ^= HAL_KEY_P2_EDGE_BITS; // flip edge bit
        break;
    default:
        break;
    }

    LREP("portNum=0x%X pressedPin=0x%X isPressed=%d\r\n", portNum, pressedPin, isPressed);
    LREP("pin=" BYTE_TO_BINARY_PATTERN "\r\n", BYTE_TO_BINARY(pressedPin));
    LREP("P0=" BYTE_TO_BINARY_PATTERN "\r\n", BYTE_TO_BINARY(P0));
    LREP("P1=" BYTE_TO_BINARY_PATTERN "\r\n", BYTE_TO_BINARY(P1));
    LREP("P2=" BYTE_TO_BINARY_PATTERN "\r\n", BYTE_TO_BINARY(P2));

    if (pHalKeyProcessFunction) {
        (pHalKeyProcessFunction)(pressedPin, portNum | isPressed);
    }
}

void HalKeyEnterSleep(void) {
    uint8 clkcmd = CLKCONCMD;
    uint8 clksta = CLKCONSTA;
    // Switch to 16MHz before setting the DC/DC to bypass to reduce risk of flash corruption
    CLKCONCMD = (CLKCONCMD_16MHZ | OSC_32KHZ);
    // wait till clock speed stablizes
    while (CLKCONSTA != (CLKCONCMD_16MHZ | OSC_32KHZ))
        ;

    CLKCONCMD = clkcmd;
    while (CLKCONSTA != (clksta))
        ;
}

uint8 HalKeyExitSleep(void) {
    uint8 clkcmd = CLKCONCMD;
    // Switch to 16MHz before setting the DC/DC to on to reduce risk of flash corruption
    CLKCONCMD = (CLKCONCMD_16MHZ | OSC_32KHZ);
    // wait till clock speed stablizes
    while (CLKCONSTA != (CLKCONCMD_16MHZ | OSC_32KHZ))
        ;

    CLKCONCMD = clkcmd;

    // /* Wake up and read keys */
    return (HalKeyRead());
}

#if HAL_KEY_P0_INPUT_PINS
HAL_ISR_FUNCTION(halKeyPort0Isr, P0INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P0IFG & HAL_KEY_P0_INPUT_PINS) {
        halProcessKeyInterrupt(HAL_KEY_PORT0);
    }

    P0IFG &= ~HAL_KEY_P0_INPUT_PINS;
    P0IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}
#endif

#if HAL_KEY_P1_INPUT_PINS
HAL_ISR_FUNCTION(halKeyPort1Isr, P1INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P1IFG & HAL_KEY_P1_INPUT_PINS) {
        halProcessKeyInterrupt(HAL_KEY_PORT1);
    }

    P1IFG &= ~HAL_KEY_P1_INPUT_PINS;
    P1IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}
#endif

#if HAL_KEY_P2_INPUT_PINS
HAL_ISR_FUNCTION(halKeyPort2Isr, P2INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P2IFG & HAL_KEY_P2_INPUT_PINS) {
        halProcessKeyInterrupt(HAL_KEY_PORT2);
    }

    P2IFG &= ~HAL_KEY_P2_INPUT_PINS;
    P2IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}
#endif