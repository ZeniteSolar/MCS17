#include "adc.h"

/**
 * @brief initializes all adc circular buffers.
 */
void init_buffers(void)
{
    CBUF_Init(cbuf_adc3);
    CBUF_Init(cbuf_adc4);
}

/**
 * @brief computes the average of a given adc channel
 *
 * Ma = (1/N)*Summation of x[i] from i=0 to N, 
 * if N = 2^k, then Ma = (Summation of x[i] from i=0 to N) >> k
 *
 */
uint8_t ma_adc3(void)
{   
    uint16_t sum = 0;
    for(uint8_t i = cbuf_adc3_SIZE; i; i--){
        sum += CBUF_Get(cbuf_adc3, i);
    }
    avg_adc3 = sum >> cbuf_adc3_SIZE_2;
    return avg_adc3;
}

/**
* @brief computes the average of a given adc channel
*
* Ma = (1/N)*Summation of x[i] from i=0 to N, 
* if N = 2^k, then Ma = (Summation of x[i] from i=0 to N) >> k
*
*/
uint8_t ma_adc4(void)
{   
    uint16_t sum = 0;
    for(uint8_t i = cbuf_adc4_SIZE; i; i--){
        sum += CBUF_Get(cbuf_adc4, i);
    }
    avg_adc4 = sum >> cbuf_adc4_SIZE_2;
    return avg_adc4;
}

/**
 * @brief Muda o canal do adc
 * @param __ch is the channel to be switched to
 * @return return the selected channel
 */
uint8_t adc_select_channel(adc_channels_t __ch)
{
    ADC_CHANNEL = __ch;
    ADMUX = (ADMUX & 0xF8) | ADC_CHANNEL; // clears the bottom 3 bits before ORing
    return ADC_CHANNEL;
}

/**
 * @brief inicializa o ADC, configurado para conversão engatilhada com o timer0.
 */
void adc_init(void)
{

    // configuracao do ADC
    //PORTC   =   0b00000000;                         // pull-up for adcs
    //DDRC    =   0b00000000;                         // all adcs as inputs
    //DIDR0   =   0b11111111;                         // ADC0 to ADC2 as adc (digital disable)

    ADMUX   =   (0 << REFS1)                        // AVcc with external capacitor at AREF pin
            | (1 << REFS0)
            | (1 << ADLAR);                         // ADC left adjusted -> using 8bits ADCH only

    ADCSRB  =   (0 << ADTS2)                        // Auto-trigger source: timer0 Compare Match A
            | (1 << ADTS1)
            | (1 << ADTS0);

    adc_select_channel(ADC3);                       // Choose admux
    ADCSRA  =   (1 << ADATE)                        // ADC Auto Trigger Enable
            | (1 << ADIE)                           // ADC Interrupt Enable
            | (1 << ADEN)                           // ADC Enable
            | (1 << ADSC)                           // Do the first Start of Conversion
            | (1 << ADPS2)                          // ADC Prescaller = 128;
            | (1 << ADPS1)
            | (1 << ADPS0);


    // configuracao do Timer TC0 --> TIMER DO ADC
    TCCR0A  =   (1 << WGM01) | (0 << WGM00)         // Timer 0 in Mode 2 = CTC (clear on compare)
            | (0 << COM0A1) | (0 << COM0A0)         // Normal port operation
            | (0 << COM0B1) | (0 << COM0B0);        // do nothing with OC0B
    TCCR0B  =   (0 << WGM02)                        // Timer 0 in Mode 2 = CTC (clear on compare)
            | (0 << FOC0A) | (0 << FOC0B)           // dont force outputs
            | (1 << CS02)                           // clock enabled, prescaller = 256
            | (0 << CS01)
            | (0 << CS00);

	OCR0A  =    20;                                 // Valor para igualdade de comparacao A para frequencia de ~1500 Hz
    TIMSK0 |=   (1 << OCIE0A);                      // Ativa a interrupcao na igualdade de comparação do TC0 com OCR0A

    init_buffers();
}

/**
 * @brief MUX do ADC
 */
ISR(ADC_vect){
    switch(ADC_CHANNEL){
        case ADC3:
            VERBOSE_MSG_ADC(usart_send_string("adc3: "));
            CBUF_Push(cbuf_adc3, ADCH); 
            ADC_CHANNEL++;
            break;
        case ADC4:
            VERBOSE_MSG_ADC(usart_send_string("adc4: "));
            CBUF_Push(cbuf_adc4, ADCH);
			ADC_CHANNEL++;
            //break;
        default:
            ADC_CHANNEL = ADC3;             // recycles
            break;
    }        
    VERBOSE_MSG_ADC(usart_send_uint16(ADCH));
    adc_select_channel(ADC_CHANNEL);
}
 
/**
 * @brief ISR necessária para auto-trigger do ADC. Caso contrário, dispara
 * BADISR_vect.
 */
EMPTY_INTERRUPT(TIMER0_COMPA_vect);

