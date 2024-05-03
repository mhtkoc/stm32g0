#include "stm32g0xx.h"

uint32_t ten_thousands,thousands,hundreds,tens;
uint32_t  number;
uint32_t c=0 ;
uint32_t flag_for_tens_or_units=1;
uint32_t uart_display_counter=0;
uint32_t duty_cycle =0;
uint8_t unit_digit =0;
uint8_t tens_digit =0;

void SystemInitial();
uint8_t ConvertNumberToAsci(uint8_t);
void InitTimer();
void UartInit(uint32_t);
void LedInit(void);
void delay(volatile uint32_t);
void KeypadInitForOutput(void);
void KeypadInitForInput(void);
void SetKeypad(void);
void ResetKeypad(void);
void PrintChar(uint8_t);

volatile int interrupt_check=0;




int main(void) {

	SystemInitial();

	SetKeypad();
    while(1) {


    }

    return 0;
}



void SystemInitial(){


    LedInit();
    KeypadInitForOutput();
    ResetKeypad();
    KeypadInitForInput();
	InitTimer();
	UartInit(9600);
}

void InitTimer(){

		RCC->APBENR1 |=(1U<<1);//clock enable
		TIM3->CR1=0;
		TIM3->CR1 |=(1<<7);//control register
		TIM3->CNT =0;//counter 0'lanıyor
		TIM3->PSC=10; //frekans bölücü
		TIM3->ARR=(16000);//periyot miktarı
		TIM3->DIER |=(1<<0);
		TIM3->CCMR1 |=(1<<3);//pwm mode 1 ayarlamaları yapılıyor

		TIM3->CCMR1 &= ~ (1u<<16); //0
		TIM3->CCMR1 |=(1u<<6); //1
		TIM3->CCMR1 |= (1u<<5); //1
		TIM3->CCMR1 &= ~(1u<<4);  // 0
		TIM3->CCER |=(1<<0);
		TIM3->CCR1 =0x0;


		TIM3->EGR |= (1<<0);
		TIM3->CR1 |= (1<<0);

		NVIC_SetPriority(TIM3_IRQn,3);
		NVIC_EnableIRQ(TIM3_IRQn);

}
uint8_t ConvertNumberToAsci(uint8_t a){
	if(a==0)
		return 48 ; // ASCII equivalent of zero

	else if(a==1) // ASCII equivalent of one
		return 49;
	else if(a==2)// ASCII equivalent of two
		return 50;
	else if(a==3) // ASCII equivalent of three
		return 51;
	else if (a==4)// ASCII equivalent of four
		return 52;
	else if(a==5)// ASCII equivalent of five
		return 53;
	else if (a==6)// ASCII equivalent of six
		return 54;
	else if(a==7)// ASCII equivalent of seven
		return 55;
	else if(a==8)// ASCII equivalent of eight
		return 56;
	else
		return 57; // // ASCII equivalent of nine
}
void TIM3_IRQHandler(){
	uart_display_counter=uart_display_counter+10; // her iki saniyede bi yazmak için değişken oluşturuyoruz
	//en yüksek duty on değerimiz 16000

	if(uart_display_counter==2000){
				delay(2000);
				number=TIM3->CCR1;


				ten_thousands=(number)/10000; //on binlik değeri elde ediyoruz
				PrintChar(ConvertNumberToAsci(ten_thousands)); //konsola on binlik değeri yazdırıyoruz


				number -= (ten_thousands * 10000);
				thousands=number/1000; 		//binlik değeri buluyoruz
				PrintChar(ConvertNumberToAsci(thousands)); //consola binlik değeri yazıyoruz



				number -= (thousands * 1000);
				hundreds=(number /100);			//yüzlük değeri buluyoruz
				PrintChar(ConvertNumberToAsci(hundreds)); //yüzlük değeri konsola yazıyoruz


				number -= (hundreds * 100);
				tens=(number/10);			//on luk değeri buluyoruz
				PrintChar(ConvertNumberToAsci(tens)); //onluk değeri konsola yazıyoruz

				number -= (tens * 10);   //birlik değeri elde ediyoruz
				PrintChar(ConvertNumberToAsci(number)); //birlik değeri konsola yazdırıyoruz
				uart_display_counter=0;
	}



	TIM3->SR &=  ~(1U<<0);
	TIM3->SR &=  ~(1U<<1);
}



void LedInit(){
	 /* GPIOB ve GPIOA kanalları enable ediliyor*/
	    RCC->IOPENR |= (3U << 0);
	 /* PA6 AF1  olarak set ediliyor*/

		GPIOA->MODER &= ~(3U << 2*6);
		GPIOA->MODER |=  (2<< 2*6) ;

		GPIOA->AFR[0]  &= ~(0xFU<<4*6);
		GPIOA->AFR[0]  |=  (1<<4*6);

}
void EXTI0_1_IRQHandler (){ //c1
	ResetKeypad();

	GPIOA->ODR |=(1<<0);
	if((GPIOB->IDR>>1)&1){
		if(flag_for_tens_or_units==1){ //KEYPAD buton kontrol 1 - 1 matrisi için kontrol ediliyor
			tens_digit=1;
			flag_for_tens_or_units=0;
		}
		else{
			unit_digit=1;
			flag_for_tens_or_units=1;
		}

	}
	else{						//KEYPAD buton kontrol 4 için kontrol ediliyor
		GPIOA->ODR &=  ~(1U <<0);
		GPIOA->ODR |=(1<<1);
		if((GPIOB->IDR>>1)&1){

			if(flag_for_tens_or_units==1){
						tens_digit=4;
						flag_for_tens_or_units=0;
					}
					else{
						unit_digit=4;
						flag_for_tens_or_units=1;
					}

				}

		else {						//KEYPAD buton kontrol 7 için kontrol ediliyor
			GPIOA->ODR &=  ~(1U <<1);
			GPIOA->ODR |=(1<<4);
			if((GPIOB->IDR>>1)&1){
				if(flag_for_tens_or_units==1){
										tens_digit=7;
										flag_for_tens_or_units=0;
									}
									else{
										unit_digit=7;
										flag_for_tens_or_units=1;
									}


			}
			else{
				GPIOA->ODR &=  ~(1U <<4);
				GPIOA->ODR |=(1<<5);
				if((GPIOB->IDR>>1)&1){

				}

			}

		}

	}
	SetKeypad();
	EXTI->RPR1 |=(1<<0);
}

void EXTI2_3_IRQHandler (){
	ResetKeypad();
	if((EXTI->RPR1>>2)&1){ //KEYPAD buton kontrol 2 - 2 matrisi için kontrol ediliyor
		GPIOA->ODR |=(1<<0);
		if((GPIOB->IDR>>2)&1){
			if(flag_for_tens_or_units==1){
						tens_digit=2;
						flag_for_tens_or_units=0;
					}
					else{
						unit_digit=2;
						flag_for_tens_or_units=1;
					}

				}

		else{
			GPIOA->ODR &=  ~(1U <<0); //KEYPAD buton kontrol 5 için kontrol ediliyor
			GPIOA->ODR |=(1<<1);
			if((GPIOB->IDR>>2)&1){
				if(flag_for_tens_or_units==1){
							tens_digit=5;
							flag_for_tens_or_units=0;
						}
						else{
							unit_digit=5;
							flag_for_tens_or_units=1;
						}

					}

			else {					//KEYPAD buton kontrol 8 için kontrol ediliyor
				GPIOA->ODR &=  ~(1U <<1);
				GPIOA->ODR |=(1<<4);
				if((GPIOB->IDR>>2)&1){
					if(flag_for_tens_or_units==1){
								tens_digit=8;
								flag_for_tens_or_units=0;
							}
							else{
								unit_digit=8;
								flag_for_tens_or_units=1;
							}

						}

				else{				//KEYPAD buton kontrol 0 için kontrol ediliyor
					GPIOA->ODR &=  ~(1U <<4);
					GPIOA->ODR |=(1<<5);
					if((GPIOB->IDR>>2)&1){
						if(flag_for_tens_or_units==1){
									tens_digit=0;
									flag_for_tens_or_units=0;
								}
								else{
									unit_digit=0;
									flag_for_tens_or_units=1;
								}


					}

				}

			}

		}
		EXTI->RPR1 |=(1<<2);
	}
	else{						//KEYPAD buton kontrol 3 - 3 matrisi için kontrol ediliyor
		GPIOA->ODR |=(1<<0);
		if((GPIOB->IDR>>3)&1){
			if(flag_for_tens_or_units==1){
						tens_digit=3;
						flag_for_tens_or_units=0;
					}
					else{
						unit_digit=3;
						flag_for_tens_or_units=1;
					}


		}
		else{								//KEYPAD buton kontrol 6 için kontrol ediliyor
				GPIOA->ODR &=  ~(1U <<0);
				GPIOA->ODR |=(1<<1);
				if((GPIOB->IDR>>3)&1){
					if(flag_for_tens_or_units==1){
								tens_digit=6;
								flag_for_tens_or_units=0;
							}
							else{
								unit_digit=6;
								flag_for_tens_or_units=1;
							}

						}

				else {						//KEYPAD buton kontrol 9 için kontrol ediliyor
						GPIOA->ODR &=  ~(1U <<1);
						GPIOA->ODR |=(1<<4);
						if((GPIOB->IDR>>3)&1){
							if(flag_for_tens_or_units==1){
										tens_digit=9;
										flag_for_tens_or_units=0;
									}
									else{
										unit_digit=9;
										flag_for_tens_or_units=1;
									}


						}
						else{			//KEYPAD buton kontrol # için kontrol ediliyor
								GPIOA->ODR &=  ~(1U <<4);
								GPIOA->ODR |=(1<<5);
						if((GPIOB->IDR>>3)&1){
							duty_cycle=(16000*((tens_digit*10)+unit_digit))/100;
							TIM3->CCR1=duty_cycle;
							}

						}

					}

		}

		EXTI->RPR1 |=(1<<3);
	}
	SetKeypad();//keypad in çıkışları düzenleniyor
}
void KeypadInitForOutput(){

	 GPIOA->MODER &=  ~(3U<<2*0); //PA0 çıkış olarak R1 için ayarlanıyor
	 GPIOA->MODER |=  (1U<<0);


	 GPIOA->MODER &=  ~(3U <<2*1);//PA1 çıkış olarak R2 için ayarlanıyor
	 GPIOA->MODER |=  (1 << 2);

	 GPIOA->MODER &=  ~(3U <<2*4); //PA4 çıkış olarak R3 için ayarlanıyor
	 GPIOA->MODER |=  (1 << 8);

	 GPIOA->MODER &=  ~(3U <<2*5); //PA5 çıkış olarak R4 için ayarlanıyor
	 GPIOA->MODER |=  (1 << 10);
}
void SetKeypad(){
	GPIOA->ODR |= (1<<0);  //R1 set
	GPIOA->ODR |= (1<<1);  //R2 set
	GPIOA->ODR |= (1<<4);  //R3 set
	GPIOA->ODR |= (1<<5);  //R4 set

}
void ResetKeypad(){
	GPIOA->ODR &=  ~(1U <<0); //R1 reset
	GPIOA->ODR &=  ~(1U <<1); //R2 reset
	GPIOA->ODR &=  ~(1U <<4); //R3 reset
	GPIOA->ODR &=  ~(1U <<5);  //R4 reset
}

void KeypadInitForInput(){

		GPIOB->MODER &= ~(3U << 2*1);// PB1 input olarak C1 için ayarlandı
		GPIOB->PUPDR |= (2U << 2*1); //pin pull down olarak ayarlandı

		GPIOB->MODER &= ~(3U << 2*2);// PB2 input olarak C2 için ayarlandı
		GPIOB->PUPDR |= (2U << 2*2); //pin pull down olarak ayarlandı

		GPIOB->MODER &= ~(3U << 2*3); // PB3 input olarak C3 için ayarlandı
		GPIOB->PUPDR |= (2U << 2*3); //pin pull down olarak ayarlandı


	    EXTI->RTSR1 |=(1U<<1); // PB1 interrupt'ı
	    EXTI->EXTICR[0] |= (1U<<8*1);
	    EXTI->IMR1 |=(1<<1);
	    NVIC_SetPriority(EXTI0_1_IRQn,1);
	    NVIC_EnableIRQ(EXTI0_1_IRQn);


	    EXTI->RTSR1 |=(1U<<2); // PB2 interrupt'ı
	    EXTI->EXTICR[0] |= (1U<<8*2);
 	    EXTI->IMR1 |=(1<<2);


	    EXTI->RTSR1 |=(1U<<3);// PB3 interrupt'ı
	    EXTI->EXTICR[0] |= (1U<<8*3);
	    EXTI->IMR1 |=(1<<3);
	    NVIC_SetPriority(EXTI2_3_IRQn,0);
	    NVIC_EnableIRQ(EXTI2_3_IRQn);


}

void delay(volatile uint32_t s) {
    for(; s>0; s--);
}
void UartInit(uint32_t baud){
	//Enable GPIOA
	RCC->IOPENR |= (1U << 0);
	RCC->APBENR1 |= (1U << 17);

	//PA2 Alternate Function (AF1) olarak ayarlandı
	GPIOA-> MODER &= ~(3U << 2*2);
	GPIOA-> MODER |=  (2U << 2*2);

	//AF1 seçildi
	GPIOA-> AFR[0] &= ~(0xFU << 4*2);
	GPIOA-> AFR[0] |=  (1U << 4*2);

	//PA3 Alternate Function (AF1) olarak ayarlandı
	GPIOA-> MODER &= ~(3U << 2*3);
	GPIOA-> MODER |=  (2U << 2*3);

	//AF1 seçildi
	GPIOA-> AFR[0] &= ~(0xFU << 4*3);
	GPIOA-> AFR[0] |=  (1U << 4*3);

	USART2 -> CR1 = 0;
	USART2 -> CR1 |= (1U << 3); // Transmitter
	USART2 -> CR1 |= (1U << 2); // Receiver

	USART2 -> CR1 |= (1U << 5);

	USART2 -> BRR = (uint16_t)(SystemCoreClock / baud);

	USART2 -> CR1 |= (1U << 0); // Usart Enable


}

void PrintChar(uint8_t b){
	USART2->TDR =(uint16_t)b;
	while(!(USART2->ISR&(1<<6)));
}
