%include "sconst.inc"

_NR_get_ticks equ 0
_NR_write equ 1
_NR_sendrec equ 2
INT_VECTOR_SYS_CALL 	equ 0x90
GLOBAL get_ticks
GLOBAL write
BITS 32
section .text
get_ticks:
	mov eax,_NR_get_ticks
	int INT_VECTOR_SYS_CALL
	ret
write:
	mov eax,_NR_write
	mov ebx,[esp + 4];buf
	mov ecx,[esp + 8];len
	int INT_VECTOR_SYS_CALL
	ret
sendrec:
	mov eax,_NR_sendrec
	mov ebx,[esp + 4];function 
	mov ecx,[esp + 8];src_dest
	mov edx,[esp + 12];p_msg
	int INT_VECTOR_SYS_CALL
	ret

