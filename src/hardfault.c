#include <stdio.h>
#include "em_device.h"

//
// For some really serious errors, such as invalid instructions,
// memory segmentation faults, etc., the processor will raise a
// hard fault and halt. This handler lets us read some tracing
// information designed to let us track down the problem in our
// program.
//

void debugHardfault(uint32_t *sp)
{
    volatile uint32_t cfsr  = SCB->CFSR;
    volatile uint32_t hfsr  = SCB->HFSR;
    volatile uint32_t mmfar = SCB->MMFAR;
    volatile uint32_t bfar  = SCB->BFAR;

    uint32_t r0  = sp[0];
    uint32_t r1  = sp[1];
    uint32_t r2  = sp[2];
    uint32_t r3  = sp[3];
    uint32_t r12 = sp[4];
    uint32_t lr  = sp[5];
    uint32_t pc  = sp[6];
    uint32_t psr = sp[7];

    // printf() doesn't really go anywhere, but you can put a breakpoint here
    // to debug hard-to-catch errors that cause hardfault.
    printf("HardFault:\n");
    printf("SCB->CFSR   0x%08lx\n", cfsr);
    printf("SCB->HFSR   0x%08lx\n", hfsr);
    printf("SCB->MMFAR  0x%08lx\n", mmfar);
    printf("SCB->BFAR   0x%08lx\n", bfar);
    printf("\n");

    printf("SP          0x%08lx\n", (uint32_t)sp);
    printf("R0          0x%08lx\n", r0);
    printf("R1          0x%08lx\n", r1);
    printf("R2          0x%08lx\n", r2);
    printf("R3          0x%08lx\n", r3);
    printf("R12         0x%08lx\n", r12);
    printf("LR          0x%08lx\n", lr);
    printf("PC          0x%08lx\n", pc);
    printf("PSR         0x%08lx\n", psr);

    while(1);
}

// The name of this handler is magic, and will automatically be inserted in the
// exception handler vector of the SoC. That way, whenever a hard fault occurs, this is called.
// Refer to the EFM32 documentation to learn more about it.
__attribute__( (naked) )
void HardFault_Handler(void)
{
    __asm volatile
    (
        "tst lr, #4                                    \n"
        "ite eq                                        \n"
        "mrseq r0, msp                                 \n"
        "mrsne r0, psp                                 \n"
        "ldr r1, debugHardfault_address                \n"
        "bx r1                                         \n"
        "debugHardfault_address: .word debugHardfault  \n"
    );
}
