// Q2.s
 // Author: Alperen Arslan

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
/////////////////////
ldr r1, =RCC_IOPENR
	ldr r0, [r1]
	movs r4, 0x1
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =GPIOA_MODER
	ldr r3, [r6]

	ldr r4, =[0x7FD55500] //PA 4-5-6-7-8-9-10-15 numaralı pinler çıkış olarak ayarlandı
	ands r3, r3, r4
	str r3, [r6]

	// setup PB0, PB1, PB2 ....PB9 for 01 and PB5 for 00 in MODER
	ldr r6, =GPIOB_MODER //B bus ı input moduna alındı
	ldr r5, [r6]
	ldr r4, =[0x0]
	ands r5, r5, r4
	str r5, [r6]

	//D1 Active
	ldr r6, =GPIOA_ODR
	ldr r5, [r6]
	ldr r4, =[0x0]
	orrs r5, r5, r4 //a nın bütün pinler low
	str r5, [r6]

loop:				//PA8 D9 yakıldı
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x87F0
	orrs r0, r0, r4
	str r0, [r1]


    ldr r6, =#4000000
    bl delay

	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0xFFFF
	bics r0, r0, r4
	str r0, [r1]


    ldr r6, =#4000000
    bl delay
    b loop

 delay:
     subs r6,r6, #1
     bne delay
     bx lr

	b .
	nop

//////////////
