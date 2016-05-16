#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include<timercounter0.h>
#include<timercounter2.h>

#define TRESHOLD 250 // 0.85V from 5.00V

#define LEFT 0
#define MID_LEFT 1
#define MID 2
#define MID_RIGHT 3
#define RIGHT 4

#define MAX 255
#define STOP 0
#define OPT 66

int czujniki[5] = {0};
int prev_err = 0, przestrzelony = 0;
int blad, pop_blad = 0, Kp = 20, Kd = 20;
int V_zad = 200;

int PD() {
    int rozniczka = blad - pop_blad;
    pop_blad = blad;
    return Kp * blad + Kd * rozniczka;
}

void forward_left() {
    PORTC &= ~(1 << PC6);
    PORTC |= (1 << PC7);
}

void back_left() {
    PORTC |= (1 << PC6);
    PORTC &= ~(1 << PC7);
}

void forward_right() {
    PORTC &= ~(1 << PC0);
    PORTC |= (1 << PC1);
}

void back_right() {
    PORTC |= (1 << PC0);
    PORTC &= ~(1 << PC1);
}

int licz_blad() {
    int err = 0;
    int ilosc = 0;

    int waga[] = {-15, -5, 0, 5, 15};
	
	if (przestrzelony)                    // zmniejszenie wag czujników w przypadku przestrzelenia zakrętu 
        waga[0] = -3;
		waga[1] = -2;
		waga[2] = 0;
		waga[3] = 2;
		waga[4] = 3;

	int i;
    for (i = 0; i < 5; i++) {
        err += czujniki[i] * waga[i];
        ilosc += czujniki[i];
    }

    if (ilosc) {
        err /= ilosc;
        prev_err = err;
    } else {
        if (prev_err < 5) {
            err = -15;
            przestrzelony = 1;                // ustawienie flagi - przestrzelony, linia po lewej 
        } else if (prev_err > 5) {
            err = 15;
            przestrzelony = 2;                // ustawienie flagi - przestrzelony, linia po prawej 
        } else {
            err = 0;
        }
    }

    if (przestrzelony == 1 && err >= 0)        // zerowanie flagi przestrzelenia zakrętu po powrocie na środek linii 
        przestrzelony = 0;
    else if (przestrzelony == 2 && err <= 0)
        przestrzelony = 0;

    return err;
}

void PWM(int set_left, int set_right) {
    int left = set_left;
    int right = set_right;

    forward_left();

    if (left > 255) {
        left = 255;
    }
	
	if (left < -255) {
        left = -255;
    }

    if (left < 0) {
        back_left();
        left *= -1;
    }

    forward_right();

    if (right > 255) {
        right = 255;
    }
	
	if (right < -255) {
        right = -255;
    }

    if (right < 0) {
        back_right();
        right *= -1;
    }

    set_timercounter0_compare_value(right);
    set_timercounter2_compare_value(left);
}


void petla_LF() {
    blad = licz_blad();
    int regulacja = PD();
    PWM(V_zad + regulacja, V_zad - regulacja);
}

void performConvertingADC() {
    ADCSRA |= (1 << ADSC); //ADSC: single convertion
    while (ADCSRA & (1 << ADSC)) {
        //wait for convertion finish
    }
}

void handleADCInputs() {
    // ADC0
    ADMUX &= ~(1 << MUX0);
    ADMUX &= ~(1 << MUX1);
    ADMUX &= ~(1 << MUX2);
    performConvertingADC();

    if (ADC < TRESHOLD) {
        czujniki[LEFT] = 1;
    } else {
        czujniki[LEFT] = 0;
    }

    // ADC1
    ADMUX |= (1 << MUX0);
    ADMUX &= ~(1 << MUX1);
    ADMUX &= ~(1 << MUX2);
    performConvertingADC();

    if (ADC < TRESHOLD) {
        czujniki[MID_LEFT] = 1;
    } else {
        czujniki[MID_LEFT] = 0;
    }

    // ADC2
    ADMUX &= ~(1 << MUX0);
    ADMUX |= (1 << MUX1);
    ADMUX &= ~(1 << MUX2);
    performConvertingADC();

    if (ADC < TRESHOLD) {
        czujniki[MID] = 1;
    } else {
        czujniki[MID] = 0;
    }

    // ADC3
    ADMUX |= (1 << MUX0);
    ADMUX |= (1 << MUX1);
    ADMUX &= ~(1 << MUX2);
    performConvertingADC();

    if (ADC < TRESHOLD) {
        czujniki[MID_RIGHT] = 1;
    } else {
        czujniki[MID_RIGHT] = 0;
    }

    // ADC4
    ADMUX &= ~(1 << MUX0);
    ADMUX &= ~(1 << MUX1);
    ADMUX |= (1 << MUX2);
    performConvertingADC();

    if (ADC < TRESHOLD) {
        czujniki[RIGHT] = 1;
    } else {
        czujniki[RIGHT] = 0;
    }
}

int isHovered() {
    int i;
    for (i = 0; i < 5; i++) {
        if (czujniki[i] != 1) {
            return 0;
        }
    }

    return 1;
}

int main() {
    ADCSRA = (1 << ADEN) //ADEN: ADC Enable
             | (1 << ADPS0)
             | (1 << ADPS1)
             | (1 << ADPS2); //ADPS2:0: prescaler at 128

    //IO settings
    DDRA = 0x00; //All A pins as an input
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;

    ADMUX = (1 << REFS1) | (1 << REFS0); //Init ADMUX with REFS

    set_timercounter0_mode(3);
    /*Timer counter 0 is set to fast pwm mode*/

    set_timercounter0_prescaler(4);
    /*Timer counter 0 frequency is set to 3.90625KHz*/

    set_timercounter0_output_mode(2);
    /*Timer counter 0 output mode is set  to non-inverting mode*/

    set_timercounter2_mode(3);
    /*Timer counter 2 is set to fast pwm mode*/

    set_timercounter2_prescaler(4);
    /*Timer counter 2 frequency is set to 3.90625KHz*/

    set_timercounter2_output_mode(2);
    /*Timer counter 2 output mode is set to non-inverting mode*/


    while (1) {

        handleADCInputs();

        petla_LF();

        //debugczujniki();
    }

    return 0;
}
