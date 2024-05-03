.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss


/* define peripheral addresses from RM0444 page 57, Tables 3-4 */
.equ RCC_BASE,         (0x40021000)          // RCC base address
.equ RCC_IOPENR,       (RCC_BASE   + (0x34)) // RCC IOPENR register offset

.equ GPIOA_BASE,       (0x50000000)          // GPIOA base address
.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOA_ODR,        (GPIOA_BASE + (0x14)) // GPIOA ODR register offset


/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss
	 * not necessary for rom only code
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
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

	/* zero bss */
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


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:
	ldr r1, =RCC_IOPENR
	ldr r0, [r1]   // content r0'a atandı.
	movs r4, 0x1   // sadece A portu açıldı.
	orrs r0, r0, r4  // r0 değeri ile r4 or yapıldı.
	str r0, [r1]     // A portunun clock açıldı.

//MODER butonlar için 00 olmalı
	/* For PA8 change the 16-17 bits into the GPIOx_MODER*/
	ldr r1, =GPIOA_MODER
	ldr r0, [r1]
	ldr r4, =0xffff00 // 3 instructions in below, clears the relevant bits
	mvns r4, r4
	ands r0, r0, r4
	ldr r4, =0x555500 // Arrange the relevant bits as general purpose output mode.
	orrs r0, r0, r4
	str r0, [r1] // Led'ler output moduna alındı.4,5,6,7,8,9,10,11 pinleri yapıldı.



loop:

 	// sağdan sola led yakma

	//t2 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0xe00 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t2 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0xe00 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


	//t3 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x700 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t3 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x700 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


   	//t4 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x380 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t4 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x380 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


	//t5 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x1c0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t5 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x1c0 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


    //t6 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x0e0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t6 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x0e0 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


    //t7 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x070 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t7 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x070 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]




	 //soldan sağa led yakma

	//t2 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x070 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t2 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x070 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


	//t3 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x0e0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t3 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x0e0 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


   	//t4 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x1c0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t4 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x1c0 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


	//t5 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x380 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t5 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x380 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


    //t6 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x700 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t6 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0x700 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


    //t7 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0xe00 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay

    //t7 ledlerini kapattık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4,=0xe00 // Arrange the pin enabled.
	bics r0, r0, r4
	str r0, [r1]


	b loop


 delay:
     subs r6,r6, #1
     bne delay
     bx lr


	/* for(;;); */
	b .
	/* this should never get executed */
	nop
