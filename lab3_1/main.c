#include "stm32g0xx.h"

int global_t = 0;

void Delay_ms(uint32_t time);
void LedInit(void);					//led pinini giriş olarak tanımlayan fonksiyon
void LedSet(void);					//led pinine güç veriliyor
void LedClear(void);				//led pininden güç kesiliyor
void SystemInital();				//sistem ayarları yapılıyor

int main(void) {
	SystemInital();
	while(1){
	/* Delay using SysTick_Handler */
		LedSet();
		Delay_ms(1000);
		LedClear();
		Delay_ms(1000);
	}

	return 0;
}

void SystemInital(){
// Systick, SystemCoreClock/1000 olarak 1 ms de bi çalışması için set edildi

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
	LedInit();
}

void Delay_ms(uint32_t t){
	global_t = t;			//gloabal_t değerine fonksiyonun parametresi atanıyor
	while(global_t);		//while ın içi t 0 olana kadar döngüye giriyor
}

void SysTick_Handler(){		//interrupt fonksiyonu
	if(global_t != 0){		//global t değeri 0 dan farklı ise bir azaltıyor
		global_t --;
	}
}

void LedInit(void){			//led pininin konfigürasyonları yapılıyor
//GPIOA
	RCC->IOPENR |= (3U << 0);
//PA6 as output
	GPIOA->MODER &= ~(3U << 2*6);
	GPIOA->MODER |= (1U << 2*6);
/* Clear PA6 */
	GPIOA->BRR |= (1U << 6);
}

void LedSet(void){
	GPIOA->ODR |= (1U << 6);//PA6 pinine güç veriliyor
}

void LedClear(void){
	GPIOA->BRR |= (1U << 6);//PA6 pinindeki güç kesiliyor
}
