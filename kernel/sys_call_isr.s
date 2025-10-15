bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	pusha				; Pushes general purpose registers
	push ds
	push ss
	push es
	push fs
	push gs

	mov eax, esp
	push eax
	call sys_call

	mov esp, eax

	pop gs
	pop fs
	pop es
	pop ss
	pop ds
	popa
	
	iret
