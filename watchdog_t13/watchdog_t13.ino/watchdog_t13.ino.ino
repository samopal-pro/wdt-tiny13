#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

boolean pb1_state; 
volatile uint16_t pb1_count;

// Оброботчик прерывания по таймеру TIMER0
ISR(TIM0_OVF_vect){
   pb1_count++;  
}

int main(){
// Устанавливаем выход PB0 
   DDRB |= (1 << PB0);      // pinMode(PB0, OUTPUT); 
   PORTB &= ~(1 << PB0);    // digitalWrite(PB0, LOW);}
// Устанавливаем вход PB1 с подтягиванием 
   DDRB &= ~(1 << PB1);    // pinMode(PB1, INPUT_PULLUP);   
   PORTB |= (1 << PB1);    
// Устанавливаем вход PB2 с подтягиванием 
   DDRB &= ~(1 << PB2);    // pinMode(PB2, INPUT_PULLUP);   
   PORTB |= (1 << PB2);    
// Устанавливаем входы PB3 с подтягиванием 
   DDRB &= ~(1 << PB3);    // pinMode(PB3, INPUT_PULLUP); 
   PORTB |= (1 << PB3);    
// Устанавливаем входы PB4 с подтягиванием 
   DDRB &= ~(1 << PB4);    // pinMode(PB4, INPUT_PULLUP);  
   PORTB |= (1 << PB4);    
// Определяам время срабатывание таймера по входам PB2,PB3,PB4 (перемычки подтягивают к земле) (период, сек = TM/4 )
   uint16_t TM = 0;
   bool pb2 = false;
   bool pb3 = false;
   bool pb4 = false;
   if( PINB & (1 << PINB2) )pb2 = true;
   if( PINB & (1 << PINB3) )pb3 = true;
   if( PINB & (1 << PINB4) )pb4 = true;

   if( pb2 == true  && pb3 == true  && pb4 == true )TM = 4;         //Таймаут 1 сек
   else if( pb2 == false && pb3 == true  && pb4 == true  )TM = 8;   //Таймаут 2 сек
   else if( pb2 == true  && pb3 == false && pb4 == true  )TM = 20;  //Таймаут 5 сек
   else if( pb2 == false && pb3 == false && pb4 == true  )TM = 40;  //Таймаут 10 сек
   else if( pb2 == true  && pb3 == true  && pb4 == false )TM = 80;  //Таймаут 20 сек
   else if( pb2 == false && pb3 == true  && pb4 == false )TM = 120; //Таймаут 30 сек
   else if( pb2 == true  && pb3 == false && pb4 == false )TM = 240; //Таймаут 60 сек
   else if( pb2 == false && pb3 == false && pb4 == false )TM = 480; //Таймаут 120 сек
   pb1_count = 0;
   pb1_state = false;
// Отключаем ADC
   PRR = (1<<PRADC); // shut down ADC
// Настраиваем таймер
   TIMSK0 = (1<<TOIE0);  // Включаем таймер TIMER0
   TCCR0B = (1<<CS02) | (1<<CS00); // Пределитель таймера на 1/1024
// Задаем режим сна
   MCUCR &= ~(1<<SM1); // idle mode
   MCUCR &= ~(1<<SM0); // idle mode
   MCUCR |= (1<<SE);
   sei();
   while(1) {
// Зпсываем до прерывания по таймеру    
     asm("sleep");
// Таймер сработал 
     TIMSK0 &= ~ (1<<TOIE0);  // Останавливаем TIMER0       
// Считываем состояние PB1
    bool pb1 = false;
    if( PINB & (1 << PINB1) )pb1 = true;  
// Если состояние входа инвертировалось, сбрасываем время
    if( pb1 != pb1_state )pb1_count = 0;
    pb1_state = pb1;
// Если превышено время установки таймера
    if( pb1_count >= TM ){
       PORTB |= (1 << PB0);    // digitalWrite(PB0, HIGH);}
       _delay_ms(1000);        // Ждем секунду
       PORTB &= ~(1 << PB0);   // digitalWrite(PB0, LOW);}
       pb1_count = 0;          // Сбрасываем счетчик
    }
    TIMSK0 = (1<<TOIE0);    // Включаем таймер TIMER0
    sei();           
   
  }
  return 0;
}
