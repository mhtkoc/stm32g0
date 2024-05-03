// Q2.s
// Author: Emre Boy/Muhammet Koc

.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb

// make linker see this
.global Reset_Handler

// get these from linker script
.word _sdata
.word _edata
.word _sbss
.word _ebss

// define clock base and enable addresses
.equ RCC_BASE,         (0x40021000)          // RCC base address
.equ RCC_IOPENR,       (RCC_BASE   + (0x34)) // RCC IOPENR register offset

// define GPIO Base, Moder and ODR pin addresses
.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOB_IDR,        (GPIOB_BASE + (0x10)) // GPIOB IDR register offset
.equ GPIOB_ODR,        (GPIOB_BASE + (0x14)) // GPIOB =DR register offset

.equ GPIOA_BASE,       (0x50000000)          // GPIOA base address
.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOA_ODR,        (GPIOA_BASE + (0x14)) // GPIOA ODR register offset

//Delay Interval
.equ delayInterval, 1000000

// vector table, +1 thumb mode
.section .vectors
vector_table:
	.word _estack             //     Stack pointer
	.word Reset_Handler +1    //     Reset handler
	.word Default_Handler +1  //       NMI handler
	.word Default_Handler +1  // HardFault handler
	// add rest of them here if needed

// reset handler
.section .text
Reset_Handler:
	// set stack pointer
	ldr r0, =_estack
	mov sp, r0

	// initialize data and bss
	// not necessary for rom only code

	bl init_data
	// call main
	bl main
	// trap if returned
	b .

// initialize data and bss sections
.section .text
init_data:

	// copy rom to ram
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	// zero bss
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr

// default handler
.section .text
Default_Handler:
	b Default_Handler

// main function
.section .text
main:
	// enable GPIOB clock, bit1 on IOPENR
	ldr r6, =RCC_IOPENR
	ldr r5, [r6]
	// movs expects imm8, so this should be fine
	movs r4, 0x3
	orrs r5, r5, r4
	str r5, [r6]

	// setup PA8, PA9, PA10 and PA15 for 01 in MODER
	ldr r6, =GPIOA_MODER
	ldr r5, [r6]

	ldr r4, =[0x7FD50000] //All PA pins used define output 8-9-10-15 out
	ands r5, r5, r4
	str r5, [r6]

	// setup PB0, PB1, PB2 ....PB9 for 01 and PB5 for 00 in MODER
	ldr r6, =GPIOB_MODER
	ldr r5, [r6]
	// cannot do with movs, so use pc relative
	ldr r4, =[0x0] //PB5 pin define input, others used pins define output
	ands r5, r5, r4
	str r5, [r6]

	//D1 Active
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =[0x0]
	orrs r5, r5, r4 //a nın bütün pinler high
	str r5, [r6]


//	PB4 e bağlı olan buton PA8 e bağlı olan ledi kontrol eder
	Loop:
	ldr r6, = GPIOB_IDR//input
	ldr r7, [r6] //For PB4, Countdown button
	movs r4, #0x10 //Status switch connected to PB4
	ands r7, r7, r4 //Getting the value of button pressed or not
	lsrs r7, #4 //Shifting to lsb for compare
	cmp r7, #0x1 //Compare IDR Value with 1 bit
	beq light //If equal
	cmp r7, #0x0
	beq lowled
	b Loop

lowled:
	ldr r1, =GPIOA_ODR
	ldr r2, [r1]
	ldr r0, =[0x0000]
	ands r2 , r2, r0
	str r2 , [r1]
	b Loop



light:
	ldr r1, =GPIOA_ODR
	ldr r2, [r1]
	ldr r0, =[0x0100]
	orrs r2 , r2, r0
	str r2 , [r1]
	b Loop


	// this should never get executed
	nop
