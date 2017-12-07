/**
 * @file conf.h
 *
 * @defgroup CONF Configurations 
 *
 * @brief General configuration of the system.
 *
 */

#ifndef CONF_H
#define CONF_H

// CONFIGURACOES DE COMPILACAO
//#define DEBUG_ON
//#define VERBOSE_ON
//#define VERBOSE_ON_CAN_APP
//#define VERBOSE_ON_MACHINE
//#define VERBOSE_ON_ADC
#define VERBOSE_ON_INIT
#define VERBOSE_ON_ERROR


// MODULES ACTIVATION
#define USART_ON
#define CAN_ON
//#define ADC_ON
#define MACHINE_ON
#define WATCHDOG_ON
#define SLEEP_ON

// tempo em que o potenciometro deve ficar zerado para que seja permitido ligar 
#define MIN_ZERO_WIDTH_TIMES        10     
// number of checks before reset the pwm fault counter.
#define CHECKS_BEFORE_RESET_FAULT_COUNTER 100
// maximum of consecutive faults before state an error
#define FAULT_COUNT_LIMIT           50


// INPUT PINS DEFINITIONS
#define     RELAYS_1_5_PORT         PORTD
#define     RELAYS_1_5_PIN          PIND
#define     RELAYS_1_5_DDR          DDRD
#define     RELAY1                  PD3
#define     set_relay1()            set_bit(RELAYS_1_5_PORT, RELAY1);
#define     clr_relay1()            clr_bit(RELAYS_1_5_PORT, RELAY1);
#define     RELAY2                  PD4
#define     set_relay2()            set_bit(RELAYS_1_5_PORT, RELAY2);
#define     clr_relay2()            clr_bit(RELAYS_1_5_PORT, RELAY2);
#define     RELAY3                  PD7
#define     set_relay3()            set_bit(RELAYS_1_5_PORT, RELAY3);
#define     clr_relay3()            clr_bit(RELAYS_1_5_PORT, RELAY3);
#define     RELAY4                  PD2
#define     set_relay4()            set_bit(RELAYS_1_5_PORT, RELAY4);
#define     clr_relay4()            clr_bit(RELAYS_1_5_PORT, RELAY4);
#define     RELAY5                  PD5
#define     set_relay5()            set_bit(RELAYS_1_5_PORT, RELAY5);
#define     clr_relay5()            clr_bit(RELAYS_1_5_PORT, RELAY5);

#define     LED_PORT                PORTD
#define     LED_PIN                 PIND
#define     LED_DDR                 DDRD
#define     LED                     PD6
#define     cpl_led()               cpl_bit(LED_PORT, LED)
#define     set_led()               set_bit(LED_PORT, LED)
#define     clr_led()               clr_bit(LED_PORT, LED)


// CANBUS DEFINITONS
// ----------------------------------------------------------------------------
/* Global settings for building the can-lib and application program.
 *
 * The following two #defines must be set identically for the can-lib and
 * your application program. They control the underlying CAN struct. If the
 * settings disagree, the underlying CAN struct will be broken, with
 * unpredictable results.
 * If can.h detects that any of the #defines is not defined, it will set them
 * to the default values shown here, so it is in your own interest to have a
 * consistent setting. Ommiting the #defines in both can-lib and application
 * program will apply the defaults in a consistent way too.
 *
 * Select if you want to use 29 bit identifiers.
 */
#define	SUPPORT_EXTENDED_CANID	0

/* Select if you want to use timestamps.
 * Timestamps are sourced from a register internal to the AT90CAN.
 * Selecting them on any other controller will have no effect, they will
 * be 0 all the time.
 */
#define	SUPPORT_TIMESTAMPS		0



#endif /* ifndef CONF_H */
