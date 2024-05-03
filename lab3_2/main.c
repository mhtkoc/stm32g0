#include "stm32g0xx.h"

int PSC_val = 1;				//zamanı ayarlamak için değişken oluşturuldu
uint8_t flag = 1;				//butona kaç kere basıldığını anlamak için flag oluşturuldu

void delay(uint32_t);			//delay fonku tanımlandı
void LedInit(void);
void LedToggle(void);
void ButtonInit(void);
int ButtonRead(void);
void InitTimer(void);
void EXTI0_1_IRQHandler(void);	//

int main(void) {

	LedInit();					//led pini çıkış tanımlandı
	ButtonInit();				//buton pini giriş olarak tanımlandı
	InitTimer();				//Timer ayarlamaları yapıldı

	TIM3->PSC = 1000*PSC_val;	//başlangıç olarak delay süresi 1sn ayarlandı

	while(1){
		if(ButtonRead()){		//buton okundu
			flag++;				//flag değeri bir arttırıldı

			if(flag){
				PSC_val++;		//delay süresini arttırmak için değer arttırıldı
				delay(2222222);
				TIM3->PSC = 1000*PSC_val;//delay istenilen oranda arttırıldı
			/*
			When the button is pressed, a delay is added so that b will only drop once, and it is ensured to wait in if block
			*/
				if(flag == 11){	//flag değeri 11 ise döngünün başına dönmek için değerler 1 olarak set edildi
					PSC_val = 1;
					flag = 1;
				}

			}
		}
	}
	return 0;
}

void InitTimer(void){	//timer ayarlamaları yapıldı

	RCC->APBENR1 |= (1U<<1);


	TIM3->CR1 = 0;		//control register 0 landı
	TIM3->CR1 |= (1<<7);//7. bit 1 olarak set edildi
	TIM3->CNT = 0;		//register counter ı 0 landı

	TIM3->PSC = 1000;	//frkans değeri
	TIM3->ARR = 16000;	//periyot değeri

	TIM3->DIER |= (1<<0);//Interrupt enable register
	TIM3->CR1 |= (1<<0); //kontrol register 1 e set edildi

	NVIC_SetPriority(TIM3_IRQn,0);//Interruptın priority si oluşturuldu
	NVIC_EnableIRQ(TIM3_IRQn);	  //interrupt vektörüne tanımlandı
}

void TIM3_IRQHandler (){ //led toggle interruptı
	LedToggle();		 //LedToggle fonkuna çağrı yapıldı
	TIM3->SR &= ~(1U << 0);//status register güncellendi
}

void delay(uint32_t time){//delay fonksiyonumuz
	for(; time>0 ; time--);
}


void LedInit(void){			//Led için PA6 pini out olarak set edildi
	//GPIOA ve GPIOB
    RCC->IOPENR |= (3U << 0);

    //PA6
    GPIOA->MODER &= ~(3U << 2*6);
    GPIOA->MODER |= (1U << 2*6);

    //PA6
    GPIOA->BRR |= (1U << 6);

}

void LedToggle(void){		//led toggle edildi
	GPIOA->ODR ^= (1U << 6);
}


void EXTI0_1_IRQHandler(void){//flag interruptı
	flag=1;
	EXTI->FPR1 |= (1<<0);	  //Falling Pending Register 1
}

void ButtonInit(){		//buton için PB0 pini input olarak set edildi
	 //GPIOA ve GPIOB
	 RCC->IOPENR |= (3U << 0);
     //PB0
    GPIOB->MODER &= ~(3U << 2*7);
}


int ButtonRead(void){	//buton okundu
	int x = ((GPIOB->IDR >> 7) & 0x01);

	if(x) return 1;		//butona basılmışsa 1 değerini döndürdü
	else return 0;		//butona basılmamışsa 0 değerini döndürdü
}
