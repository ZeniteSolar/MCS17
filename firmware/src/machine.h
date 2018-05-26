/**
 * @file machine.h
 *
 * @defgroup MACHINE State Machine Module
 *
 * @brief Implements the main state machine of the system.
 *
 */

#ifndef MACHINE_H
#define MACHINE_H 

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "conf.h"

#ifdef ADC_ON
#include "adc.h"
#endif
#ifdef USART_ON
#include "usart.h"
#endif
#include "dbg_vrb.h"
#ifdef CAN_ON
#include "can.h"
#include "can_app.h"
extern const uint8_t can_filter[];
#endif

typedef enum state_machine{
    STATE_INITIALIZING,
    STATE_RUNNING,
    STATE_IDLE,
    STATE_ERROR,
} state_machine_t;

typedef union system_flags{
    struct{
        uint8_t     boat_on        :1;
    };
    uint8_t   all;
} system_flags_t;

typedef union error_flags{
    struct{
        uint8_t     no_canbus   :1;
    };
    uint8_t   all;
}error_flags_t;

typedef struct control{
    uint8_t     battery_voltage;          
    uint8_t     capacitor_voltage;          
}
control_t;

control_t control;

// machine checks
void check_switches(void);
void check_buffers(void);
void check_motor_dutycycle(void);
void check_motor_increment(void);
void check_mppts_power(void);
void check_capacitor_voltage(void);
void check_battery_voltage(void);

// debug functions
void print_system_flags(void);
void print_error_flags(void);
void print_control(void);

// machine tasks
void task_initializing(void);
void task_idle(void);
void task_running(void);
void task_error(void);

// the machine itself
void machine_init(void);
void machine_run(void);
void set_state_error(void);
void set_state_initializing(void);
void set_state_idle(void);
void set_state_running(void);

// machine variables
state_machine_t state_machine;
system_flags_t system_flags;
error_flags_t error_flags;
uint8_t total_errors;   // Contagem de ERROS
volatile uint8_t machine_clk;

// other variables
uint8_t led_clk_div;

// ISRs
ISR(TIMER2_COMPA_vect);
ISR(PCINT2_vect);

#endif /* ifndef MACHINE_H */
