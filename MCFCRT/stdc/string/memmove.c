// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2014. LH_Mouse. All wrongs reserved.

#include "../../env/_crtdef.h"
#include "_stringasm.h"

void *memmove(void *dst, const void *src, size_t cb){
	uintptr_t unused;
	__asm__ __volatile__(
		"cmp " RSI ", " RDI " \n"
		"jbe 5f \n"
		"	cmp %5, 64 \n"
		"	jb 1f \n"
		"		mov " RCX ", " RDI " \n"
		"		neg " RCX " \n"
		"		and " RCX ", 0x0F \n"
		"		sub %5, " RCX " \n"
		"		rep movsb \n"
		"		mov " RCX ", %5 \n"
		"		shr " RCX ", 4 \n"
		"		and %5, 0x0F \n"
		"		cmp " RCX ", 64 \n" // 1KiB
		"		jb 4f \n"
		"			test " RSI ", 0x0F \n"
		"			jnz 3f \n"
		"				2: \n"
		"				movdqa xmm0, xmmword ptr[" RSI "] \n"
		"				add " RSI ", 16 \n"
		"				lea " RDI ", dword ptr[" RDI " + 16] \n"
		"				movntdq xmmword ptr[" RDI " - 16], xmm0 \n"
		"				dec " RCX " \n"
		"				jnz 2b \n"
		"				jmp 1f \n"
		"			3: \n"
		"			movdqu xmm0, xmmword ptr[" RSI "] \n"
		"			add " RSI ", 16 \n"
		"			lea " RDI ", dword ptr[" RDI " + 16] \n"
		"			movntdq xmmword ptr[" RDI " - 16], xmm0 \n"
		"			dec " RCX " \n"
		"			jnz 3b \n"
		"			jmp 1f \n"
		"		4: \n"
		"		test " RSI ", 0x0F \n"
		"		jnz 3f \n"
		"			2: \n"
		"			movdqa xmm0, xmmword ptr[" RSI "] \n"
		"			add " RSI ", 16 \n"
		"			lea " RDI ", dword ptr[" RDI " + 16] \n"
		"			movdqa xmmword ptr[" RDI " - 16], xmm0 \n"
		"			dec " RCX " \n"
		"			jnz 2b \n"
		"			jmp 1f \n"
		"		3: \n"
		"		movdqu xmm0, xmmword ptr[" RSI "] \n"
		"		add " RSI ", 16 \n"
		"		lea " RDI ", dword ptr[" RDI " + 16] \n"
		"		movdqa xmmword ptr[" RDI " - 16], xmm0 \n"
		"		dec " RCX " \n"
		"		jnz 3b \n"
		"	1: \n"
		"	mov " RCX ", %5 \n"
#ifdef _WIN64
		"	shr rcx, 3 \n"
		"	rep movsq \n"
		"	mov rcx, %5 \n"
		"	and rcx, 7 \n"
#else
		"	shr ecx, 2 \n"
		"	rep movsd \n"
		"	mov ecx, %5 \n"
		"	and ecx, 3 \n"
#endif
		"	rep movsb \n"
		"	jmp 6f \n"
		"	.align 16 \n"
		"5: \n"
		"je 6f \n"
		"std \n"
		"cmp %5, 64 \n"
		"lea " RSI ", dword ptr[" RSI " + %5] \n"
		"lea " RDI ", dword ptr[" RDI " + %5] \n"
		"jb 1f \n"
		"	lea " RSI ", dword ptr[" RSI " - 1] \n"
		"	mov " RCX ", " RDI " \n"
		"	and " RCX ", 0x0F \n"
		"	lea " RDI ", dword ptr[" RDI " - 1] \n"
		"	sub %5, " RCX " \n"
		"	rep movsb \n"
		"	lea " RSI ", dword ptr[" RSI " + 1] \n"
		"	mov " RCX ", %5 \n"
		"	shr " RCX ", 4 \n"
		"	lea " RDI ", dword ptr[" RDI " + 1] \n"
		"	and %5, 0x0F \n"
		"	cmp " RCX ", 64 \n" // 1KiB
		"	jb 4f \n"
		"		test " RSI ", 0x0F \n"
		"		jnz 3f \n"
		"			2: \n"
		"			sub " RSI ", 16 \n"
		"			movdqa xmm0, xmmword ptr[" RSI "] \n"
		"			movntdq xmmword ptr[" RDI " - 16], xmm0 \n"
		"			lea " RDI ", dword ptr[" RDI " - 16] \n"
		"			dec " RCX " \n"
		"			jnz 2b \n"
		"			jmp 1f \n"
		"		3: \n"
		"		sub " RSI ", 16 \n"
		"		movdqu xmm0, xmmword ptr[" RSI "] \n"
		"		movntdq xmmword ptr[" RDI " - 16], xmm0 \n"
		"		lea " RDI ", dword ptr[" RDI " - 16] \n"
		"		dec " RCX " \n"
		"		jnz 3b \n"
		"		jmp 1f \n"
		"	4: \n"
		"	test " RSI ", 0x0F \n"
		"	jnz 3f \n"
		"		2: \n"
		"		sub " RSI ", 16 \n"
		"		movdqa xmm0, xmmword ptr[" RSI "] \n"
		"		movdqa xmmword ptr[" RDI " - 16], xmm0 \n"
		"		lea " RDI ", dword ptr[" RDI " - 16] \n"
		"		dec " RCX " \n"
		"		jnz 2b \n"
		"		jmp 1f \n"
		"	3: \n"
		"	sub " RSI ", 16 \n"
		"	movdqu xmm0, xmmword ptr[" RSI "] \n"
		"	movdqa xmmword ptr[" RDI " - 16], xmm0 \n"
		"	lea " RDI ", dword ptr[" RDI " - 16] \n"
		"	dec " RCX " \n"
		"	jnz 3b \n"
		"1: \n"
		"mov " RCX ", %5 \n"
#ifdef _WIN64
		"lea rsi, dword ptr[rsi - 8] \n"
		"shr rcx, 3 \n"
		"lea rdi, dword ptr[rdi - 8] \n"
		"rep movsq \n"
		"lea rsi, dword ptr[rsi + 7] \n"
		"mov rcx, %5 \n"
		"and rcx, 7 \n"
		"lea rdi, dword ptr[" RDI " + 7] \n"
#else
		"lea esi, dword ptr[esi - 4] \n"
		"shr ecx, 2 \n"
		"lea edi, dword ptr[edi - 4] \n"
		"rep movsd \n"
		"lea esi, dword ptr[esi + 3] \n"
		"mov ecx, %5 \n"
		"and ecx, 3 \n"
		"lea edi, dword ptr[edi + 3] \n"
#endif
		"rep movsb \n"
		"cld \n"
		"6: \n"
		: "=D"(unused), "=S"(unused), "=r"(unused)
		: "0"(dst), "1"(src), "2"(cb)
		: "cx"
	);
	return dst;
}