#include "stm32g0xx.h"


void UartInit(uint32_t baud);
void PrintChar(uint8_t value);
uint8_t uart_rx(void);
void uart_tx (uint8_t data);

int main(void) {

UartInit(9600);


	while(1){
	uart_tx(uart_rx());//while içerisinde uart iletişimi başlatılıyor
	}


    return 0;

}


uint8_t prev_data = 0;
void PrintChar(uint8_t value){
	USART2-> TDR = (uint16_t)value;//transmit data register değerine value değeri aktarılıyor
	prev_data = value;			   //prev dataya eski değerin kaydı yapılıyor
	while (!(USART2 -> ISR & (1U << 6))); //Sinyalin gelip gelmediğini kontrol ediyor

}


uint8_t uart_rx(void){
	uint8_t data = (uint8_t)USART2 -> RDR;//uart hattı okunuyor

	return data;
}
void uart_tx (uint8_t data){//uart hattına bilgi gönderiliyor
	//USART2-> TDR = (uint16_t)data;



	if(prev_data != data)//yeni alınan veri eski veriyle karşılaştırılıyor
	PrintChar(data);//eğer data farklı ise yazdırlıyor


}


void UartInit(uint32_t baud){
	//Enable GPIOA
	RCC->IOPENR |= (1U << 0);
	RCC->APBENR1 |= (1U << 17);

	//PA3 Alternate Function olarak set ediliyor
	GPIOA-> MODER &= ~(3U << 2*2);
	GPIOA-> MODER |=  (2U << 2*2);

	// AF1 seçiliyor
	GPIOA-> AFR[0] &= ~(0xFU << 4*2);
	GPIOA-> AFR[0] |=  (1U << 4*2);

	//PA3 Alternate Function olarak set ediliyor
	GPIOA-> MODER &= ~(3U << 2*3);
	GPIOA-> MODER |=  (2U << 2*3);

	// AF1 seçiliyor
	GPIOA-> AFR[0] &= ~(0xFU << 4*3);
	GPIOA-> AFR[0] |=  (1U << 4*3);

	USART2 -> CR1 = 0;			//kontrol register
	USART2 -> CR1 |= (1U << 3); // Transmitter
	USART2 -> CR1 |= (1U << 2); // Receiver

	USART2 -> CR1 |= (1U << 5);

	USART2 -> BRR = (uint16_t)(SystemCoreClock / baud);//baud rate ayarlanıyor(sembol oran birimi)

	USART2 -> CR1 |= (1U << 0); // Usart Enable edildi


}
