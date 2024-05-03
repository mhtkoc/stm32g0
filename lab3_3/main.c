//emre boy && muhammetkoc
#include "stm32g0xx.h"

int SSDnum = 0;


/* LED Functions */
void LedInit(void);
void LedSet(void);
void LedClear(void);
void LedToggle(void);


/* System Function */
void SystemInitial(void);
void delay(volatile uint32_t s);

/* Button Functions */
void ButtonInit(void);
int ButtonRead(void);

/* SSD Functions */
void SetDigit1 (void);
void SetDigit2 (void);
void SetDigit3 (void);
void SetDigit4 (void);
void PrintOne(void);
void PrintTwo(void);
void PrintThree(void);
void PrintFour(void);
void PrintFive(void);
void PrintSix(void);
void PrintSeven(void);
void PrintEight(void);
void PrintNine(void);
void PrintZero(void);
void PrintFullZeros(void);


void InitTimer(void);
void FindNumberPlaces(int);
void LedControl (int);



int main(void) {


	SystemInitial();
	InitTimer();
	PrintFullZeros();


    while(1){


    }

    return 0;

}

void InitTimer(void){

	RCC->APBENR1 |= (1U<<1);


	TIM3->CR1 = 0;
	TIM3->CR1 |= (1<<7);
	TIM3->CNT = 0;

	TIM3->PSC = 999;
	TIM3->ARR = 16000;

	TIM3->DIER |= (1<<0);
	TIM3->CR1 |= (1<<0);

	NVIC_SetPriority(TIM3_IRQn,0);
	NVIC_EnableIRQ(TIM3_IRQn);


}
void TIM3_IRQHandler (){
	TIM3->SR &= ~(1U << 0);

	if((SSDnum > 0) || ButtonRead()){	//butona basıldığında yada SSDnum 0 dan büyük olduğunda
		for(int i = 0; i<1000 ; i++){	// 1000 e kadar say
			SSDnum ++;					//SSDnum ı 1 arttır

			if(SSDnum == 9999){			//SSDnum 9999 olunca 0 a set et
				SSDnum = 0;

				break;
			}
			FindNumberPlaces(SSDnum);	//numaraların hanesini bulacağımız kısıma gidiyoruz
		}
	}
	else{

		for(int i=0; i<50; i++){
		PrintFullZeros();
		for(int j=0; j<100; j++){
		LedSet();
		}

		}

	}

}

void FindNumberPlaces(int SSD_num){

	int thousand,hundred,ten,one;

	for(int i = 0; i<2; i++){
	thousand = SSD_num/1000;	//SSDnum değerinin binler basamğı alındı
	delay(150);
	SetDigit1();				//1.digit set
	LedControl(thousand);		//digite gidecek olan rakam için çıkışlar ayarlanıyor
	delay(150);

	hundred = (SSD_num - thousand*1000)/100;
	SetDigit2();				//2.digit set
	LedControl(hundred);		//digite gidecek olan rakam için çıkışlar ayarlanıyor
	delay(150);

	ten = (SSD_num - thousand*1000 - hundred*100)/10;
	SetDigit3();				//3.digit set
	LedControl(ten);			//digite gidecek olan rakam için çıkışlar ayarlanıyor
	delay(150);

	one = (SSD_num - thousand*1000 - hundred*100 - ten*10);
	SetDigit4();				//4.digit set
	LedControl(one);			//digite gidecek olan rakam için çıkışlar ayarlanıyor
	delay(150);

	}
}

/* SSD FUNCTIONS */

void SetDigit1 (void){
	GPIOA->ODR = (1U << 0); //PA0	//digit 1 set
}
void SetDigit2 (void){
	GPIOA->ODR = (1U << 1);//PA1	//digit 2 set
}
void SetDigit3 (void){
	GPIOA->ODR = (1U << 4);//PA4	//digit 3 set
}
void SetDigit4 (void){
	GPIOA->ODR = (1U << 5);//PA5	//digit 4 set
}
void PrintOne(void){
	// B=C=0, A=D=E=F=G=1
	GPIOB->ODR = 0x3C8;	//PB3-6-7-8-9 0011_1100_1000 bir yazmak için gerekli olan çıkışlar
}

void PrintTwo(void){
	 // A=B=G=E=D=0, C=F=1
	GPIOB->ODR = 0x120; // PB5-8  0001_0010_0000 iki yazmak için gerekli olan çıkışlar
}

void PrintThree(void){
	// A=B=G=C=D=0, E=F=1
	GPIOB->ODR = 0x180; // PB7-8  0001_1000_0000 üç yazmak için gerekli olan çıkışlar
}

void PrintFour(void){
	// F=B=G=C=0 A=D=E=1
	GPIOB->ODR = 0xC8; // PB3-6-7 0000_1100_1000 dört yazmak için gerekli olan çıkışlar
}

void PrintFive(void){
	//A=F=G=C=D=0 B=E=1
	GPIOB->ODR = 0x90; // PB4-7 0000_1001_0000 beş yazmak için gerekli olan çıkışlar
}

void PrintSix(void){
	//A=F=G=C=E=D=0 B=1
	GPIOB->ODR = 0x10; // PB4 0000_0001_0000 altı yazmak için gerekli olan çıkışlar
}

void PrintSeven(void){
	//A=B=C=0 D=E=F=G=1
	GPIOB->ODR = 0x3C0; // PB6-7-8-9 0011_1100_0000 yedi yazmak için gerekli olan çıkışlar
}

void PrintEight(void){
	//A=B=C=D=E=F=G=0
	GPIOB->ODR = 0x0; // All of them sekiz yazmak için gerekli olan çıkışlar
}

void PrintNine(void){
	//A=B=C=D=F=G=0 E=1
	GPIOB->ODR = 0x80; // PB7 0000_1000_0000 dokuz yazmak için gerekli olan çıkışlar
}

void PrintZero(void){
	//A=B=C=D=E=F=0 G=1
	GPIOB->ODR = 0x200; // PB9 0010_0000_0000_0000 sıfır yazmak için gerekli olan çıkışlar
}
void PrintFullZeros(void){
	for(int i = 0; i<100; i++){


    SetDigit1();		//digit bir set edilecek
    PrintZero();		//aktif edilen digite 0 yazdırılacak
    delay(100);

    SetDigit2();		//digit iki set edilecek
    PrintZero();		//aktif edilen digite 0 yazdırılacak
    delay(100);

    SetDigit3();		//digit üç set edilecek
    PrintZero();		//aktif edilen digite 0 yazdırılacak
    delay(100);

    SetDigit4();		//digit dört set edilecek
    PrintZero();		//aktif edilen digite 0 yazdırılacak
    delay(100);


	}
}

void delay(volatile uint32_t s){//for 0 olana kadar bekliyoruz delay görevi görüyor
	for(; s>0 ; s--);
}

void LedControl (int x){//hangi rakam geldiğine göre ABCDEFG pinleri set edilecek

	switch (x){

	case 0:
		PrintZero();
		break;
	case 1:
		PrintOne();
		break;
	case 2:
		PrintTwo();
		break;
	case 3:
		PrintThree();
		break;
	case 4:
		PrintFour();
		break;
	case 5:
		PrintFive();
		break;
	case 6:
		PrintSix();
		break;
	case 7:
		PrintSeven();
		break;
	case 8:
		PrintEight();
		break;
	case 9:
		PrintNine();
		break;
	default:
		break;
	}


}
void LedInit(void){			//Led input konfigürasyonları yapılıyor
	//GPIOA ve GPIOB aktif edildi
    RCC->IOPENR |= (3U << 0);

    // PA6 out olarak set edildi
    GPIOA->MODER &= ~(3U << 2*6);
    GPIOA->MODER |= (1U << 2*6);

}
void ButtonInit(void){		//buton konfigürasyonu yapılacak
	//GPIOA ve GPIOB aktif edildi
	 RCC->IOPENR |= (3U << 0);
    //PB0 input olarak set edildi
    GPIOA->MODER &= ~(3U << 2*7);
}
int ButtonRead(void){
	int x = ((GPIOA->IDR >> 7) & 0x01);//butonun okunduğı okunup 7 bit kaydırılarak durumu kontrol ediliyor

	if(x) return 1;					   //eğer buton high ise fonksiyon 1 değeri döndürüyor
	else return 0;					   //eğer buton low ise fonksiyon 0 değeri döndürüyor


}
void LedSet(void){//harici led set ediliyor

	GPIOA->ODR |= (1U << 6);

}
void LedClear(void){//harici led reset ediliyor
    /* Clear PA6 */
    GPIOA->BRR |= (1U << 6);
}


void SSDInit(void){
	RCC->IOPENR |= (3U << 0);

    //PA0 çıkış olarak ayarlanıyor
    GPIOA->MODER &= ~(3U << 2*0);
    GPIOA->MODER |= (1U << 2*0);

    //PA1 çıkış olarak ayarlanıyor
    GPIOA->MODER &= ~(3U << 2*1);
    GPIOA->MODER |= (1U << 2*1);

    //PA4 çıkış olarak ayarlanıyor
    GPIOA->MODER &= ~(3U << 2*4);
    GPIOA->MODER |= (1U << 2*4);

    //PA5 çıkış olarak ayarlanıyor
    GPIOA->MODER &= ~(3U << 2*5);
    GPIOA->MODER |= (1U << 2*5);

    //PB3 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*3);
    GPIOB->MODER |= (1U << 2*3);

    //PB4 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*4);
    GPIOB->MODER |= (1U << 2*4);

    //PB5 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*5);
    GPIOB->MODER |= (1U << 2*5);

    //PB6 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*6);
    GPIOB->MODER |= (1U << 2*6);

    //PB7 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*7);
    GPIOB->MODER |= (1U << 2*7);

    //PB8 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*8);
    GPIOB->MODER |= (1U << 2*8);

    //PB9 çıkış olarak ayarlanıyor
    GPIOB->MODER &= ~(3U << 2*9);
    GPIOB->MODER |= (1U << 2*9);


}


void SystemInitial(){
	SSDInit();
	ButtonInit();
	LedInit();

}
