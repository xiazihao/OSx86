#include <lib.h>
#include "type.h"
#include "protect.h"
#include "const.h"
#include "proto.h"
#include "global.h"
#include <page.h>

void init_page();

/**
 * prepare gdtr, idtr, idt, should be called very early
 */
void cstart() {
//    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
//                     "-----\"cstart\" begins-----\n");
    memcpy(&gdt, (void *) (*((u32 *) (&gdt_ptr[2]))), *((u16 *) (&gdt_ptr[0])) + 1);
    u16 *p_gdt_limit = (u16 *) (&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *) (&gdt_ptr[2]);
    *p_gdt_base = (u32 *) gdt;
    *p_gdt_limit = (GDT_SIZE * sizeof(Descriptor) - 1);

    u16 *p_idt_limit = (u16 *) (&idt_ptr[0]);
    u32 *p_idt_base = (u32 *) (&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(Gate) - 1;
    *p_idt_base = (u32) &idt;
    init_protect();
    init_page();
//    disp_str("-----\"cstart\" ends-----\n");
}

typedef struct {
    u32 baseLow;
    u32 baseHigh;
    u32 lengthLow;
    u32 lengthHigh;
    u32 type;
} ARDS;

void init_page() {
    unsigned int total = 0;
    int *num = (int *) 0x91000;
    ARDS *ards = (ARDS *) 0x91010;
    for (int i = 0; i < *num; ++i) {
        if (ards->type == 1) {
            total += ards->lengthLow;
        }
        ards++;
    }
    unsigned int temp = 0x1ef0000 / 0x1000 / 0x1000 + 1;
    u32 *page_dir_base = (u32 *) PAGEDIRBASE;
    // init page director
    for (int i = 0; i < 1024; ++i) {
        *page_dir_base = (u32) ((PAGETABLEBASE + i * 4096) | PAGE_PRESENT | PAGE_RW_W | PAGE_US_U);
        page_dir_base++;
    }
    u32 *page_table_base = (u32 *) PAGETABLEBASE;
    for (int j = 0; j < 1024 * temp; ++j) {
        *page_table_base = (u32) (j * 4096 | PAGE_PRESENT | PAGE_RW_W | PAGE_US_U);
        page_table_base++;
    }
}