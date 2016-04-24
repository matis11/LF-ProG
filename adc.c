#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

int main()
 { 
    ADCSRA = (1<<ADEN) //ADEN: ADC Enable (uruchomienie przetwornika)  
      |(1<<ADPS0)
      |(1<<ADPS1)
      |(1<<ADPS2); //ADPS2:0: ustawienie preskalera, preskaler= 128
    
    ADMUX = (1<<REFS1) | (1<<REFS0) //REFS1:0: Reference Selection Bits
    //Internal 2.56V Voltage Reference with external capacitor at AREF pin
    |(1<<MUX2) | (1<<MUX0); //Input Channel Selections (ADC5 - Pin 5 )
    
    //ustawienie wejœc/wyjœæ
    DDRA = 0x00; //Ustawienie pinów ADC jako wejœcia
    DDRB = 0xFF; //Ustawienie pinów steruj¹cych diodami jako wyjœcia
    
    
   while (1) {
      ADCSRA |= (1<<ADSC); //ADSC: uruchomienie pojedynczej konwersji
            
      while(ADCSRA & (1<<ADSC)) {
	 //czeka na zakoñczenie konwersji
      }
      
      
   }
   return 0;
 }