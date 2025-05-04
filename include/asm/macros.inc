; macOS linker/assembler require undescore prefix
; for extern C routines
%macro define 1
	%ifdef __MACH__
		global _%1
		_%1:
	%else
		global %1
		%1:
	%endif
%endmacro

; so we can call our 'defined' routines portably
%macro calldef 1
	%ifdef __MACH__
		call _%1
	%else 
		call %1
	%endif
%endmacro

; PI extern call
%macro externcall 1
	%ifdef __MACH__
		call _%1 wrt ..plt
	%else
		call %1 wrt ..plt
	%endif
%endmacro

; extern decl
%macro externdef 1
	%ifdef __MACH__
		extern _%1
	%else
		extern %1
	%endif
%endmacro

; an explicit call to fiber_yield() will prompt the
; compiler to do much of the work for us, thanks to
; 64-bit calling convention
%macro pushcallee 0
	push rbx
	push rbp
	push r12
	push r13
	push r14
	push r15
	pushf
	sub rsp, 8
%endmacro

%macro popcallee 0
	add rsp, 8
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rbx
%endmacro

; if we call yield in asm, we must enforce calling
; convention ourselves
%macro pushcaller 0
	push rax
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r12
	sub rsp, 8
%endmacro

%macro popcaller 0
	add rsp, 8
	pop r12
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rax
%endmacro

; where yielding is not co-operative,
; save all registers (and rflags)
%macro pushall 0
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf
%endmacro

%macro popall 0
	popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

