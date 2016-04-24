#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define TRESHOLD 350 // 0.85V from 5.00V

void performConvertingADC() {
    ADCSRA |= (1<<ADSC); //ADSC: single convertion
    while(ADCSRA & (1<<ADSC)) {
        //wait for convertion finish
    }
}

void handleADCInputs() {
    // ADC0
    ADMUX &=~ (1<<MUX0);
    ADMUX &=~ (1<<MUX1);
    ADMUX &=~ (1<<MUX2);
    performConvertingADC();
    
    if (ADC < TRESHOLD) {
        PORTB  &=~ (1<<PB0);
    } else {
        PORTB  |= (1<<PB0);
    }
    
    // ADC1
    ADMUX |= (1<<MUX0);
    ADMUX &=~ (1<<MUX1);
    ADMUX &=~ (1<<MUX2);
    performConvertingADC();
    
    if (ADC < TRESHOLD) {
        PORTB  &=~ (1<<PB1);
    } else {
        PORTB  |= (1<<PB1);
    }
    
    // ADC2
    ADMUX &=~ (1<<MUX0);
    ADMUX |= (1<<MUX1);
    ADMUX &=~ (1<<MUX2);
    performConvertingADC();
    
    if (ADC < TRESHOLD) {
        PORTB  &=~ (1<<PB2);
    } else {
        PORTB  |= (1<<PB2);
    }
    
    // ADC3
    ADMUX |= (1<<MUX0);
    ADMUX |= (1<<MUX1);
    ADMUX &=~ (1<<MUX2);
    performConvertingADC();
    
    if (ADC < TRESHOLD) {
        PORTB  &=~ (1<<PB3);
    } else {
        PORTB  |= (1<<PB3);
    }
    
    // ADC4
    ADMUX &=~ (1<<MUX0);
    ADMUX &=~ (1<<MUX1);
    ADMUX |= (1<<MUX2);
    performConvertingADC();
    
    if (ADC < TRESHOLD) {
        PORTB  &=~ (1<<PB4);
    } else {
        PORTB  |= (1<<PB4);
    }
}


int main()
{
    ADCSRA = (1<<ADEN) //ADEN: ADC Enable
    |(1<<ADPS0)
    |(1<<ADPS1)
    |(1<<ADPS2); //ADPS2:0: prescaler at 128
    
    //io settings
    DDRA = 0x00; //All A pins as an input
    DDRB = 0xFF; //All B pins as an output
    
    ADMUX = (1<<REFS1) | (1<<REFS0); //Init ADMUX with REFS
    
    while (1) {
        handleADCInputs();
    }
    
    return 0;
}