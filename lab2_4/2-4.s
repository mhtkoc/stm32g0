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
.equ GPIOA_IDR,        (GPIOA_BASE + (0X10)) // GPIOA IDR offset


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
	ldr r0, [r1]
	movs r4, 0x1
	orrs r0, r0, r4
	str r0, [r1]



	ldr r1, =GPIOA_MODER
	ldr r0, [r1]
	ldr r4, =0xffff00 //maske oluşturuldu
	mvns r4, r4	//maskenin tersi alındı
	ands r0, r0, r4	//maske ile moder bilgisi andlendi
	ldr r4, =0x555500
	orrs r0, r0, r4
	str r0, [r1] // Led'ler output moduna alındı.4,5,6,7,8,9,10,11 pinleri yapıldı.

	//clear bit
	ldr r0, =GPIOA_MODER
	ldr r1, [r0]
	movs r2,#0x11
	lsls r2,r2,#24
	mvns r2,r2
	ands r1,r1,r2//12. pinin mod ayarı input yapıldı
	str r1,[r0]

	movs r3,#0x0		//döngü yönünü kontrol etmek için

	t2:
	//t2 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0xe00 // pinlere güç verildi
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay




	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay			//butona basma süresi için delay atadık
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1			//alınan değer karşılaştırıldı
	beq	change_t2 //eğer butona basıldıysa cahnge_t2 fonkuna gidildi
	cmp r1 ,#0x0
	beq jump_t2	  //butona basılmadıysa jump_t2 fonkuna gidildi

	change_t2:
	cmp r3 , #0x1	//r3 registerı karşılaştırıldı
	bne r3_high_t2//r3 1 e eşit değilse r3_high_t2 fonkuna gidildi
	cmp r3 , #0x0
	bne r3_low_t2//aynı adım

	r3_high_t2:	//aynı adım
	movs r3 , #0x1//r3 değeri 1 olarak değiştirildi
	bx lr			//önceki adıma dönüldü

	r3_low_t2:	//aynı adım
	movs r3 , #0x0//r3 değeri 0 olarak değiştirildi
	bx lr			//önceki adıma dönüldü

	jump_t2:
	cmp r3, 0x1
	beq t3			//eğer r3 1 ise bir sonraki adıma geçildi
	cmp r3, 0x0
	bl t9			//eğer r3 0 ise bir önceki adıma dönüldü

	t3:
	//t3 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x700 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay



	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t3 //burası aynı adım
	cmp r1 ,#0x0
	beq jump_t3	  //burasıda aynı adım

	change_t3:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t3//aynı adım
	cmp r3 , #0x0
	bne r3_low_t3//aynı adım

	r3_high_t3:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t3:	//aynı adım
	movs r3 , #0x0
	bx lr

	jump_t3:		//aynı adım
	cmp r3, 0x1
	beq t4			//next step
	cmp r3, 0x0
	beq t2			//revers step


	t4:
   	//t4 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x380 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay



	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t4 //burası aynı adım
	cmp r1 ,#0x0
	beq jump_t4	  //burasıda aynı adım

	change_t4:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t4//aynı adım
	cmp r3 , #0x0
	bne r3_low_t4//aynı adım

	r3_high_t4:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t4:	//aynı adım
	movs r3 , #0x0
	bx lr

	jump_t4:	//aynı adım
	cmp r3, 0x1
	beq t5			//next step
	cmp r3, 0x0
	beq t3			//revers step

	t5:
	//t5 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x1c0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay





	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t5 //burası aynı adım
	cmp r1 ,#0x0
	beq jump_t5	  //burasıda aynı adım

	change_t5:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t5//aynı adım
	cmp r3 , #0x0
	bne r3_low_t5//aynı adım

	r3_high_t5:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t5:	//aynı adım
	movs r3 , #0x0
	bx lr

	jump_t5:	//aynı adım
	cmp r3, 0x1
	beq t6			//next step
	cmp r3, 0x0
	beq t4			//revers step


	t6:
    //t6 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x0e0 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay



	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t6 //burası aynı adım
	cmp r1 ,#0x0
	beq jump_t6	 //burasıda aynı adım

	change_t6:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t6//aynı adım
	cmp r3 , #0x0
	bne r3_low_t6//aynı adım

	r3_high_t6:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t6:	//aynı adım
	movs r3 , #0x0
	bx lr

	jump_t6:	//aynı adım
	cmp r3, 0x1
	beq t7			//next step
	cmp r3, 0x0
	beq t5			//revers step


	t7:
    //t7 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x070 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay



	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t7 //burası aynı adım
	cmp r1 ,#0x0
	beq step7	  //burasıda aynı adım

	change_t7:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t7//aynı adım
	cmp r3 , #0x0
	bne r3_low_t7//aynı adım

	r3_high_t7:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t7:	//aynı adım
	movs r3 , #0x0
	bx lr

	step7:	//aynı adım
	cmp r3, 0x1
	beq t8			//next step
	cmp r3, 0x0
	beq t6			//revers step


	t8:
    //t8 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0x830 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay


	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	change_t8 //burası aynı adım
	cmp r1 ,#0x0
	beq step8	  //burasıda aynı adım

	change_t8:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t8//aynı adım
	cmp r3 , #0x0
	bne r3_low_t8//aynı adım

	r3_high_t8:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t8:	//aynı adım
	movs r3 , #0x0
	bx lr

	step8:	//aynı adım
	cmp r3, 0x1
	beq t9			//next step
	cmp r3, 0x0
	beq t7			//revers step


	t9:
    //t9 ledlerini yaktık
	ldr r1, =GPIOA_ODR
	ldr r0, [r1]
	ldr r4 ,=0x0000
	ands r0,r0,r4
	ldr r4,=0xc10 // Arrange the pin enabled.
	orrs r0, r0, r4
	str r0, [r1]

	ldr r6, =#500000
    bl delay



	ldr  r0, =GPIOA_IDR	// GPIOA_IDR değerini R0'a yükle
	ldr  r1, [r0]		//GPIOA_IDR harici değişkenini R1 dahili değişkenine yükle
	ldr r6, =#500000
    bl delay
	ldr r2, =0x1000
	ands r1,r1,r2
	lsrs r1,#12
	cmp r1,#0x1
	beq	step9 //burası aynı adım
	cmp r1 ,#0x0
	beq jump_t9	  //burasıda aynı adım

	step9:    //aynı adım
	cmp r3 , #0x1
	bne r3_high_t9//aynı adım
	cmp r3 , #0x0
	bne r3_low_t9//aynı adım

	r3_high_t9:	//aynı adım
	movs r3 , #0x1
	bx lr

	r3_low_t9:	//aynı adım
	movs r3 , #0x0
	bx lr

	jump_t9:	//aynı adım
	cmp r3, 0x0
	beq t8			//revers step
	cmp r3, 0x1
	bl t2





 delay:
     subs r6,r6, #1
     bne delay
     bx lr


	/* for(;;); */
	b .
	/* this should never get executed */
	nop
