#include "machine.h"

/*
 * to-do:
 *      - modularize state sinalization
 *      - 
 *
 */

/**
 * @brief 
 */
void machine_init(void)
{
    TCCR2A  =   (1 << WGM21) | (0 << WGM20)         // Timer 2 in Mode 2 = CTC (clear on compar  e)
            | (0 << COM2A1) | (0 << COM2A0)         // do nothing with OC2A
            | (0 << COM2B1) | (0 << COM2B0);        // do nothing with OC2B
    TCCR2B  =   (0 << WGM22)                        // Timer 0 in Mode 2 = CTC (clear on compar  e)
            | (0 << FOC0A) | (0 << FOC0B)           // dont force outputs
            | (1 << CS22)                           // clock enabled, prescaller = 1024
            | (1 << CS21)
            | (1 << CS20);
    OCR2A   =   80;                                // Valor para igualdade de comparacao A par  a frequencia de 150 Hz
    TIMSK2 |=   (1 << OCIE2A);                      // Ativa a interrupcao na igualdade de comp  aração do TC2 com OCR2A

	
}

/**
 * @brief set error state
 */
inline void set_state_error(void)
{
    state_machine = STATE_ERROR;
}

/**
* @brief set initializing state
*/ 
inline void set_state_initializing(void)
{
    state_machine = STATE_INITIALIZING;
}

/**
* @brief set idle state
*/ 
inline void set_state_idle(void)
{
    state_machine = STATE_IDLE;
}

/**
* @brief set running state
*/ 
inline void set_state_running(void)
{
    state_machine = STATE_RUNNING;
}

/**
* @brief prints the system flags
*/
inline void print_system_flags(void)
{
    
    //VERBOSE_MSG_MACHINE(usart_send_string(" Pot_zero: "));
    //VERBOSE_MSG_MACHINE(usart_send_char(48+system_flags.pot_zero_width)); 
}

/**
* @brief prints the error flags
*/
inline void print_error_flags(void)
{
    //VERBOSE_MSG_MACHINE(usart_send_string(" NOCAN: "));
    //VERBOSE_MSG_MACHINE(usart_send_char(48+error_flags.no_canbus));
    //VERBOSE_MSG_MACHINE(usart_send_char('\n'));
}
 
/**
* @brief prints the error flags
*/
inline void print_control(void)
{
    /*VERBOSE_MSG_MACHINE(usart_send_string(" I: "));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I_raw));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I_raw_target));
    VERBOSE_MSG_MACHINE(usart_send_char(' '));
    VERBOSE_MSG_MACHINE(usart_send_uint16(control.I));
    */
} 

/**
* @brief turns on the mainrealy (a laching relay) by using a pulse of 250ms
*/
void pulse_mainrelay_on(void)
{
    clr_mainrelay_off();     //having shure to not set both coils at the same time
    _delay_ms(10);

    // apply a pulse of 250ms
    set_mainrelay_on();
    _delay_ms(250);
    clr_mainrelay_on();
}

/**
* @brief turns off the mainrealy (a laching relay) by using a pulse of 250ms
*/
void pulse_mainrelay_off(void)
{
    clr_mainrelay_on();     //having shure to not set both coils at the same time
    _delay_ms(10);

    // apply a pulse of 250ms
    set_mainrelay_off();
    _delay_ms(250);
    clr_mainrelay_off();
}

/**
 * @breif turns the boat on, slowly charging the caps before activate the
 * main relay
 */
void turn_boat_on(void)
{
    set_relay1();
    _delay_ms(250);
    set_relay2();
    _delay_ms(250);
    set_relay3();
    _delay_ms(250);
    set_relay4();
    _delay_ms(250);
    set_relay5();
    _delay_ms(250);
    pulse_mainrelay_on();
    _delay_ms(200);
    clr_relay1();
    clr_relay2();
    clr_relay3();
    clr_relay4();
    clr_relay5();
    set_relay0();
}


/**
 * @brief turns the boat off.
 */
void turn_boat_off(void)
{
    clr_relay0();
    clr_relay1();
    clr_relay2();
    clr_relay3();
    clr_relay4();
    clr_relay5();
    pulse_mainrelay_off();           
    _delay_ms(50);
    pulse_mainrelay_off();           
    _delay_ms(50);
    pulse_mainrelay_off();           
    _delay_ms(50);
}
 
/**
 * @brief Checks if the system is OK to run.
 */
inline void task_initializing(void)
{
    set_led();
    system_flags.boat_on = 0;
    turn_boat_off();

    if(!error_flags.all){
        VERBOSE_MSG_INIT(usart_send_string("System initialized without errors.\n"));
        set_state_idle();
    }
    else{
        VERBOSE_MSG_ERROR(usart_send_string("Sorry. I have have found errors in the initialilation process. \n\nI will begin to process it...\n"));
        set_state_error();
    }
}

/**
 * @brief waits for commands while Checks the system.
 */
inline void task_idle(void)
{
    if(led_clk_div++ >= 50){
        cpl_led();
        led_clk_div = 0;
    }

    if(system_flags.boat_on){
        VERBOSE_MSG_MACHINE(usart_send_string("\t\tRUNNING STATE ===> BOAT ON!!!\n"));
        turn_boat_on();
        set_state_running();
    }
}

/**
 * @brief running task checks the system and apply the control of relays
 */
inline void task_running(void)
{
    if(led_clk_div++ >= 10){
        cpl_led();
        led_clk_div = 0;
    }

    if(!system_flags.boat_on){
        VERBOSE_MSG_MACHINE(usart_send_string("\t\tIDLE STATE ===> BOAT OFF!!!\n"));
        //turn_boat_off();
        //set_state_idle();
    }
    
}

/**
 * @brief error task checks the system and tries to medicine it.
 */
inline void task_error(void)
{
    if(led_clk_div++ >= 5){
        cpl_led();
        led_clk_div = 0;
    }

    total_errors++;         // incrementa a contagem de erros
    VERBOSE_MSG_ERROR(usart_send_string("The error code is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(error_flags.all));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));

    /*if(error_flags.overcurrent)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-current!\n"));
    if(error_flags.overvoltage)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-voltage!\n"));
    if(error_flags.overheat)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Motor over-heat!\n"));
    if(error_flags.fault)
        VERBOSE_MSG_ERROR(usart_send_string("\t - IR2127 FAULT!\n"));
    if(error_flags.no_canbus)
        VERBOSE_MSG_ERROR(usart_send_string("\t - No canbus communication with MIC17!\n"));
    if(!error_flags.all)
        VERBOSE_MSG_ERROR(usart_send_string("\t - Oh no, it was some unknown error.\n"));
    */
 
    VERBOSE_MSG_ERROR(usart_send_string("The error level is: "));
    VERBOSE_MSG_ERROR(usart_send_uint16(total_errors));
    VERBOSE_MSG_ERROR(usart_send_char('\n'));
    
    if(total_errors < 2){
        VERBOSE_MSG_ERROR(usart_send_string("I will reset the machine state.\n"));
    }
    if(total_errors >= 20){
        VERBOSE_MSG_ERROR(usart_send_string("The watchdog will reset the whole system.\n"));
        for(;;);    // waits the watchdog to reset.
    }
    
    cpl_led();
    set_state_initializing();
}


/**
 * @brief this is the machine state itself.
 */
inline void machine_run(void)
{
    can_app_task();
    print_system_flags();
    print_error_flags();
    print_control();

    if(machine_clk){
        machine_clk = 0;
        switch(state_machine){
            case STATE_INITIALIZING:
                task_initializing();

                break;
            case STATE_IDLE:
                task_idle();

                break;
            case STATE_RUNNING:
                task_running();

                break;
            case STATE_ERROR:
                task_error();

            default:
                break;
        }
    }
}

/**
* @brief ISR para ações de controle
*/
ISR(TIMER2_COMPA_vect)
{
    //VERBOSE_MSG_ERROR(if(machine_clk) usart_send_string("\nERROR: CLOCK CONFLICT!!!\n"));
	machine_clk = 1;
}

