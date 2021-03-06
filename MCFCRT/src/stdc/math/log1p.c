// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#include "../../env/_crtdef.h"
#include "_asm.h"
#include "_constants.h"

static const double kPosThreshould =  0.292892; // < (1 - sqrt(2) / 2)
static const double kNegThreshould = -0.292892; // > -(1 - sqrt(2) / 2)

float log1pf(float x){
	register float ret;
	__asm__ volatile (
		"fldln2 \n"
		"fld dword ptr[%1] \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	fyl2xp1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld1 \n"
		"faddp st(1), st \n"
		"fyl2x \n"
		"2: \n"
		__MCFCRT_FLT_RET_ST("%1")
		: __MCFCRT_FLT_RET_CONS(ret)
		: "m"(x), "m"(kNegThreshould), "m"(kPosThreshould)
		: "ax"
	);
	return ret;
}

double log1p(double x){
	register double ret;
	__asm__ volatile (
		"fldln2 \n"
		"fld qword ptr[%1] \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	fyl2xp1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld1 \n"
		"faddp st(1), st \n"
		"fyl2x \n"
		"2: \n"
		__MCFCRT_DBL_RET_ST("%1")
		: __MCFCRT_DBL_RET_CONS(ret)
		: "m"(x), "m"(kNegThreshould), "m"(kPosThreshould)
		: "ax"
	);
	return ret;
}

long double log1pl(long double x){
	register long double ret;
	__asm__ volatile (
		"fldln2 \n"
		"fld tbyte ptr[%1] \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	fyl2xp1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld1 \n"
		"faddp st(1), st \n"
		"fyl2x \n"
		"2: \n"
		__MCFCRT_LDBL_RET_ST("%1")
		: __MCFCRT_LDBL_RET_CONS(ret)
		: "m"(x), "m"(kNegThreshould), "m"(kPosThreshould)
		: "ax"
	);
	return ret;
}
