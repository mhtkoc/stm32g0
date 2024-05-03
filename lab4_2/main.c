#include "stm32g0xx.h"

void SystemInitial();
void InitTimer();
void LedInit();


uint32_t rise_down=0 ;
uint8_t flag_set=0;

int main(void) {
	SystemInitial();


    while(1) {
    }

    return 0;
}


void SystemInitial(){

    LedInit();
	InitTimer();
}
void LedInit(){
	 /* GPIOB ve GPIOA enable ediliyor*/
	    RCC->IOPENR |= (3U << 0);
	 /* PA6 alternate function olarak set ediliyor*/

		GPIOA->MODER &= ~(3U << 2*6);
		GPIOA->MODER |=  (2<< 2*6) ;

		GPIOA->AFR[0]  &= ~(0xFU<<4*6);
		GPIOA->AFR[0]  |=  (1<<4*6);

}


void InitTimer(){
		RCC->APBENR1 |=(1U<<1); // timer3 clock u enable ediliyor

		TIM3->CR1=0;
		TIM3->CR1 |=(1<<7); //control register set edildi
		TIM3->CNT =0;
		TIM3->PSC=5;/* Frekans bölücü */
		TIM3->ARR=(16000); // PERİYOT miktarı auto reload preload
		TIM3->DIER |=(1<<0); // interrupts enable edildi

		TIM3->CCMR1 |=(1<<2);

		TIM3->CCMR1 &= ~ (1u<<16); //0111  bit   PWM mode 1 ayarlamak için
		TIM3->CCMR1 |=(1u<<6);
		TIM3->CCMR1 |= (1u<<5);
		TIM3->CCMR1 &= ~(1u<<4);

		TIM3->CCER |=(1<<0);//capture compare enable register


		TIM3->CCR1 =0x0;//capture compare register


		TIM3->EGR |= (1<<0);//güncelleme yaparken açılması gerekiyor.
		TIM3->CR1 |= (1<<0);

		NVIC_SetPriority(TIM3_IRQn,1);
		NVIC_EnableIRQ(TIM3_IRQn);

}
void TIM3_IRQHandler(){


	if(rise_down==16000)
		flag_set=1;//parlaklık azaltılırken falg 1 e set ediliyor
	else if(rise_down==0)
		flag_set=0;//parlaklık arttırılırken falg 0 a set ediliyor

	if (!flag_set)
		rise_down=rise_down+160;//duty cycle arttırılıyor
	else if(flag_set)
		rise_down=rise_down-160;//duty cycle azaltılıyor

	TIM3->CCR1 =rise_down;//pwm'in çalışma hızı belirleniyor

	TIM3->SR &=  (1U<<1);//set register resetleniyor

}
