
%include "sconst.inc"

EXTERN sys_call_table
EXTERN tss
EXTERN cstart
EXTERN gdt_ptr
EXTERN idt_ptr
EXTERN exception_handler
EXTERN disp_pos
EXTERN spurious_irq
EXTERN p_proc_ready
EXTERN kernel_main
EXTERN disp_str
EXTERN k_reenter
EXTERN clock_handler
EXTERN irq_table

section .bss
StackSpace resb 2*1024
StackTop:


; section .data
; clock_int_msg db "^",0
section .text	; 代码在此

global _start	; 导出 _start
global divide_error
GLOBAL single_step_exception
GLOBAL nmi
GLOBAL breakpoint_exception
GLOBAL overflow
GLOBAL bounds_check
GLOBAL inval_opcode
GLOBAL copr_not_avalible
GLOBAL double_fault
GLOBAL copr_seg_overrun
GLOBAL inval_tss
GLOBAL segment_not_present
GLOBAL stack_exception
GLOBAL general_protection
GLOBAL page_fault
GLOBAL copr_error
GLOBAL restart

global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

GLOBAL sys_call

_start:	; 跳到这里来的时候，我们假设 gs 指向显存
	mov esp,StackTop
	mov dword[disp_pos],0
	sgdt [gdt_ptr]
	call cstart
	lgdt [gdt_ptr]
	lidt [idt_ptr]
	jmp SELECTOR_KERNEL_CS:csinit
csinit:
	xor eax,eax
	mov ax, SELECTOR_TSS
	ltr ax
	; sti
	jmp kernel_main
	hlt

restart:;load process
	mov esp,[p_proc_ready];esp is the start of process block
	lldt [esp + P_LDT_SEL];P_LDT_SEL: offset from process block
	lea eax,[esp + P_STACKTOP]
	mov dword[tss + TSS3_S_SP0],eax; tss.esp0 equ the end of PROCESS.STACKFRAME, so that when 
restart_reenter:					; get into interrupt, the esp will start here and save the register
    dec dword[k_reenter]			; imiditely. ss esp eflags cs eip will push here
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp,4 ;now esp is PROCESS.STACKFRAM.eip
	iretd




save:
	pushad;EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI

	push ds
	push es
	push fs
	push gs

	mov esi,edx

	mov dx,ss
	mov ds,dx
	mov es,dx
	mov fs,dx
	mov edx,esi

	mov esi,esp;esi now is the start of process block
	inc dword[k_reenter]
	cmp dword[k_reenter],0;reenter interrupt
	jne .1

	mov esp,StackTop;begin to process interrupt, change stack to kernel stack and push process schedule
	push restart;function so that when finish interrupt,use ret to jump into "restart"
	jmp [esi + RETADR - P_STACKBASE]; so that the latest ret will return 
	.1:
	push restart_reenter
	jmp [esi + RETADR - P_STACKBASE]

%macro hwint_master 	1
	call save
	in al,INT_M_CTLMASK
	or al,1 <<%1
	out INT_M_CTLMASK,al

	mov ax,EOI
	out INT_M_CTL,al

	sti
	push %1
	call [irq_table + 4 * %1]
	add esp,4
	cli
	in al,INT_M_CTLMASK
	and al,~(1<<%1)
	out INT_M_CTLMASK,al
	ret
%endmacro
hwint00:
	hwint_master 	0

hwint01:
	hwint_master 	1

hwint02:
	hwint_master 	2

hwint03:
	hwint_master 	3

hwint04:
	hwint_master 	4

hwint05:
	hwint_master 	5

hwint06:
	hwint_master 	6

hwint07:
	hwint_master 	7

%macro hwint_slave 	1
	call save
	in al,INT_M_CTLMASK
	or al,1 <<(%1 - 8)
	out INT_S_CTLMASK,al

	mov ax,EOI
	out INT_M_CTL,al
	nop
	out INT_S_CTL,al

	sti
	push %1
	call [irq_table + 4 * %1]
	add esp,4
	cli
	in al,INT_M_CTLMASK
	and al,~(1<<(%1 - 8))
	out INT_S_CTLMASK,al
	ret
%endmacro
hwint08:
	hwint_slave 	8

hwint09:
	hwint_slave 	9

hwint10:
	hwint_slave 	10

hwint11:
	hwint_slave 	11

hwint12:
	hwint_slave 	12

hwint13:
	hwint_slave 	13

hwint14:
	hwint_slave 	14

hwint15:
	hwint_slave 	15
	
divide_error:
push 0xFFFFFFFF
push 0
jmp exception

single_step_exception:
push 0xffffffff
push 1
jmp exception

nmi:
push 0xffffffff
push 2
jmp exception

breakpoint_exception:
push 0xffffffff
push 3
jmp exception

overflow:
push 0xffffffff
push 4
jmp exception
bounds_check:
push 0xffffffff
push 5
jmp exception

inval_opcode:
push 0xffffffff
push 6
jmp exception

copr_not_avalible:
push 0xffffffff
push 7
jmp exception

double_fault:
push 8
jmp exception

copr_seg_overrun:
push 0xffffffff
push 9
jmp exception

inval_tss:
push 10
jmp exception

segment_not_present:
push 11
jmp exception

stack_exception:
push 12
jmp exception

general_protection:
push 13
jmp exception

page_fault:
push 14
jmp exception

copr_error:
push 0xffffffff
push 16
jmp exception

exception:
	call exception_handler
	add esp,8
	hlt


sys_call:
	call save
	
	push esi
	
	push edx
	
	
	push ecx
	push ebx
	push dword[p_proc_ready]
	sti
	call [sys_call_table + 4*eax]
	add esp,4*4
	pop esi
	mov [esi + EAXREG - P_STACKBASE],eax;put return value into STACKFRAME
	cli; when restart close interrupt and IF will set automaticlly when load elflags
	ret

