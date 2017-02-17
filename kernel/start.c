#include <lib.h>
#include "type.h"
#include "protect.h"
#include "const.h"
#include "proto.h"
#include "global.h"


/**
 * prepare gdtr, idtr, idt, should be called very early
 */
void cstart() {
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                     "-----\"cstart\" begins-----\n");
    memcpy(&gdt, (void *) (*((u32 *) (&gdt_ptr[2]))), *((u16 *) (&gdt_ptr[0])) + 1);
    u16 *p_gdt_limit = (u16 *) (&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *) (&gdt_ptr[2]);
    *p_gdt_base = (u32 *) gdt;
    *p_gdt_limit = (GDT_SIZE * sizeof(Descriptor) - 1);

    u16 *p_idt_limit = (u16 *) (&idt_ptr[0]);
    u32 *p_idt_base = (u32 *) (&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(Gate) - 1;
    *p_idt_base = (u32) &idt;
    initProtect();

    disp_str("-----\"cstart\" ends-----\n");
}