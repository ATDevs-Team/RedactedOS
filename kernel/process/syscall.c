#include "syscall.h"
#include "console/kio.h"
#include "interrupts/exception_handler.h"
#include "console/serial/uart.h"
#include "interrupts/gic.h"
#include "ram_e.h"

void sync_el0_handler_c(){
    uint64_t x0;
    asm volatile ("mov %0, x11" : "=r"(x0));
    uint64_t *x1;
    asm volatile ("mov %0, x12" : "=r"(x1));
    uint64_t x2;
    asm volatile ("mov %0, x13" : "=r"(x2));
    uint64_t x29;
    asm volatile ("mov %0, x15" : "=r"(x29));
    uint64_t x30;
    asm volatile ("mov %0, x16" : "=r"(x30));
    uint64_t elr;
    asm volatile ("mrs %0, elr_el1" : "=r"(elr));
    uint64_t spsr;
    asm volatile ("mrs %0, spsr_el1" : "=r"(spsr));

    uint64_t currentEL = (spsr >> 2) & 0b11;

    uint64_t sp_el;
    if (currentEL == 1)
        asm volatile ("mov %0, x17" : "=r"(sp_el));
    else if (currentEL == 0)
        asm volatile ("mrs %0, sp_el0" : "=r"(sp_el));
    else 
        handle_exception_with_info("UNEXPECTED EL",currentEL);//100€ to whoever manages to trigger this exception

    uint64_t esr;
    asm volatile ("mrs %0, esr_el1" : "=r"(esr));

    uint64_t ec = (esr >> 26) & 0x3F;
    uint64_t iss = esr & 0xFFFFFF;

    if (ec == 0x15) {
        switch (iss)
        {
        case 3:
            kprintf_args_raw((const char *)x0, x1, x2);
            break;
        
        default:
            handle_exception_with_info("Unknown syscall", iss);
            break;
        }
        
    } else {
        //We could handle more exceptions now, such as x25 (unmasked x96) = data abort
        handle_exception_with_info("UNEXPECTED EXCEPTION",ec);
    }
            
    asm volatile ("mov x29, %0" :: "r"(x29));
    asm volatile ("mov x30, %0" :: "r"(x30));
    if (currentEL == 0)
        asm volatile ("msr sp_el0, %0" :: "r"(sp_el));
    asm volatile ("msr elr_el1, %0" : : "r"(elr));
    asm volatile ("msr spsr_el1, %0" : : "r"(spsr));
    asm volatile ("mov sp, %0" :: "r"(sp_el));
    asm volatile ("eret");
    
}
