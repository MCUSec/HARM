GROUP (
  "libgcc.a"
  "libc_nano.a"
  "libm.a"
)

MEMORY
{
  /* Define each memory region */
  PROGRAM_FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 0x1fe00 /* 130560 bytes (alias Flash) */  
  SG_veneer_table (rx) : ORIGIN = 0x1001fe00, LENGTH = 0x200 /* 512 bytes (alias Flash2) */  
  Ram0 (rwx) : ORIGIN = 0x30010000, LENGTH = 0xa000 /* 40K bytes (alias RAM) */  
  SandBox (rwx) : ORIGIN = 0x2001a000, LENGTH = 0x2a000 /* 168K bytes (alias RAM2) */  
  GLOBAL_REGION (rwx) : ORIGIN = 0x14000000, LENGTH = 0x8000 /* 32K bytes (alias RAM3) */  
}

  /* Define a symbol for the top of each memory region */
  __base_PROGRAM_FLASH = 0x10000000  ; /* PROGRAM_FLASH */  
  __base_Flash = 0x10000000 ; /* Flash */  
  __top_PROGRAM_FLASH = 0x10000000 + 0x1fe00 ; /* 130560 bytes */  
  __top_Flash = 0x10000000 + 0x1fe00 ; /* 130560 bytes */  
  __base_SG_veneer_table = 0x1001fe00  ; /* SG_veneer_table */  
  __base_Flash2 = 0x1001fe00 ; /* Flash2 */  
  __top_SG_veneer_table = 0x1001fe00 + 0x200 ; /* 512 bytes */  
  __top_Flash2 = 0x1001fe00 + 0x200 ; /* 512 bytes */  
  __base_Ram0 = 0x30010000  ; /* Ram0 */  
  __base_RAM = 0x30010000 ; /* RAM */  
  __top_Ram0 = 0x30010000 + 0xa000 ; /* 40K bytes */  
  __top_RAM = 0x30010000 + 0xa000 ; /* 40K bytes */  
  __base_SandBox = 0x2001a000  ; /* SandBox */  
  __base_RAM2 = 0x2001a000 ; /* RAM2 */  
  __top_SandBox = 0x2001a000 + 0x2a000 ; /* 168K bytes */  
  __top_RAM2 = 0x2001a000 + 0x2a000 ; /* 168K bytes */  
  __base_GLOBAL_REGION = 0x14000000  ; /* GLOBAL_REGION */  
  __base_RAM3 = 0x14000000 ; /* RAM3 */  
  __top_GLOBAL_REGION = 0x14000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM3 = 0x14000000 + 0x8000 ; /* 32K bytes */  

ENTRY(ResetISR)

SECTIONS
{
 
    /* Place TrustZone Secure Gateway veneers in 'SG_veneer_table' memory */
    .gnu.sgstubs : ALIGN (32)
    {
        . = ALIGN(32);
        _start_sg = .;
        *(.gnu.sgstubs*)
        . = ALIGN(32);
        _end_sg = .;
    } > SG_veneer_table
    .text_Flash2 : ALIGN(4)
    {
       FILL(0xff)
        *(.text_Flash2) /* for compatibility with previous releases */
        *(.text_SG_veneer_table) /* for compatibility with previous releases */
        *(.text.$Flash2)
        *(.text.$SG_veneer_table)
        *(.text_Flash2.*) /* for compatibility with previous releases */
        *(.text_SG_veneer_table.*) /* for compatibility with previous releases */
        *(.text.$Flash2.*)
        *(.text.$SG_veneer_table.*)
        *(.rodata.$Flash2)
        *(.rodata.$SG_veneer_table)
        *(.rodata.$Flash2.*)
        *(.rodata.$SG_veneer_table.*)            } > SG_veneer_table

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

    } > PROGRAM_FLASH

    .text : ALIGN(4)
    {
       *(.text*)
       *(.rodata .rodata.* .constdata .constdata.*)
       . = ALIGN(4);
    } > PROGRAM_FLASH
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > PROGRAM_FLASH

    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > PROGRAM_FLASH
 

    _etext = .;
        
    /* DATA section for SandBox */

    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        PROVIDE(__start_data_SandBox = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$SandBox)
        *(.data.$RAM2)
        *(.data.$SandBox)
        *(.data.$RAM2.*)
        *(.data.$SandBox.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
        PROVIDE(__end_data_SandBox = .) ;
     } > SandBox AT>PROGRAM_FLASH

    /* DATA section for GLOBAL_REGION */

    .data_RAM3 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        PROVIDE(__start_data_GLOBAL_REGION = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$GLOBAL_REGION)
        *(.data.$RAM3)
        *(.data.$GLOBAL_REGION)
        *(.data.$RAM3.*)
        *(.data.$GLOBAL_REGION.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM3 = .) ;
        PROVIDE(__end_data_GLOBAL_REGION = .) ;
     } > GLOBAL_REGION AT>PROGRAM_FLASH

    /* MAIN DATA SECTION */
    .uninit_RESERVED (NOLOAD) : ALIGN(4)
    {
        _start_uninit_RESERVED = .;
        KEEP(*(.bss.$RESERVED*))
       . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > Ram0 AT> Ram0

    /* Main DATA section (Ram0) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       PROVIDE(__start_data_RAM = .) ;
       PROVIDE(__start_data_Ram0 = .) ;
       *(vtable)
       *(.ramfunc*)
       KEEP(*(CodeQuickAccess))
       KEEP(*(DataQuickAccess))
       *(RamFunction)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
       PROVIDE(__end_data_RAM = .) ;
       PROVIDE(__end_data_Ram0 = .) ;
    } > Ram0 AT>PROGRAM_FLASH

    /* BSS section for SandBox */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       PROVIDE(__start_bss_SandBox = .) ;
       *(.bss.$RAM2)
       *(.bss.$SandBox)
       *(.bss.$RAM2.*)
       *(.bss.$SandBox.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
       PROVIDE(__end_bss_SandBox = .) ;
    } > SandBox AT> SandBox

    /* BSS section for GLOBAL_REGION */
    .bss_RAM3 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM3 = .) ;
       PROVIDE(__start_bss_GLOBAL_REGION = .) ;
       *(.bss.$RAM3)
       *(.bss.$GLOBAL_REGION)
       *(.bss.$RAM3.*)
       *(.bss.$GLOBAL_REGION.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
       PROVIDE(__end_bss_GLOBAL_REGION = .) ;
    } > GLOBAL_REGION AT> GLOBAL_REGION

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        PROVIDE(__start_bss_RAM = .) ;
        PROVIDE(__start_bss_Ram0 = .) ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(__end_bss_RAM = .) ;
        PROVIDE(__end_bss_Ram0 = .) ;
        PROVIDE(end = .);
    } > Ram0 AT> Ram0

    /* NOINIT section for SandBox */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_noinit_RAM2 = .) ;
       PROVIDE(__start_noinit_SandBox = .) ;
       *(.noinit.$RAM2)
       *(.noinit.$SandBox)
       *(.noinit.$RAM2.*)
       *(.noinit.$SandBox.*)
       . = ALIGN(4) ;
       PROVIDE(__end_noinit_RAM2 = .) ;
       PROVIDE(__end_noinit_SandBox = .) ;
    } > SandBox AT> SandBox

    /* NOINIT section for GLOBAL_REGION */
    .noinit_RAM3 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_noinit_RAM3 = .) ;
       PROVIDE(__start_noinit_GLOBAL_REGION = .) ;
       *(.noinit.$RAM3)
       *(.noinit.$GLOBAL_REGION)
       *(.noinit.$RAM3.*)
       *(.noinit.$GLOBAL_REGION.*)
       . = ALIGN(4) ;
       PROVIDE(__end_noinit_RAM3 = .) ;
       PROVIDE(__end_noinit_GLOBAL_REGION = .) ;
    } > GLOBAL_REGION AT> GLOBAL_REGION

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        PROVIDE(__start_noinit_Ram0 = .) ;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
       PROVIDE(__end_noinit_RAM = .) ;
       PROVIDE(__end_noinit_Ram0 = .) ;        
    } > Ram0 AT> Ram0

    /* Reserve and place Heap within memory map */
    _HeapSize = 0xa00;
    .heap :  ALIGN(4)
    {
        _pvHeapStart = .;
        . += _HeapSize;
        . = ALIGN(4);
        _pvHeapLimit = .;
    } > Ram0

     _StackSize = 0xa00;
     /* Reserve space in memory for Stack */
    .heap2stackfill  :
    {
        . += _StackSize;
    } > Ram0
    /* Locate actual Stack in memory map */
    .stack ORIGIN(Ram0) + LENGTH(Ram0) - _StackSize - 0:  ALIGN(4)
    {
        _vStackBase = .;
        . = ALIGN(4);
        _vStackTop = . + _StackSize;
    } > Ram0

    /* ## Create checksum value (used in startup) ## */
    PROVIDE(__valid_user_code_checksum = 0 - 
                                         (_vStackTop 
                                         + (ResetISR + 1) 
                                         + (NMI_Handler + 1) 
                                         + (HardFault_Handler + 1) 
                                         + (( DEFINED(MemManage_Handler) ? MemManage_Handler : 0 ) + 1)   /* MemManage_Handler may not be defined */
                                         + (( DEFINED(BusFault_Handler) ? BusFault_Handler : 0 ) + 1)     /* BusFault_Handler may not be defined */
                                         + (( DEFINED(UsageFault_Handler) ? UsageFault_Handler : 0 ) + 1) /* UsageFault_Handler may not be defined */
                                         ) );

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}