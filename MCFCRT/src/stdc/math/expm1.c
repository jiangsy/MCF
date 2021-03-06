// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#include "../../env/_crtdef.h"
#include "_asm.h"
#include "_constants.h"

float expm1f(float x){
	register float ret;
	__asm__ volatile (
		"fldl2e \n"
		"fmul dword ptr[%1] \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	f2xm1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld st \n"
		"frndint \n"
		"fsub st(1), st \n"
		"fld1 \n"
		"fscale \n"
		"fstp st(1) \n"
		"fxch st(1) \n"
		"f2xm1 \n"
		"fadd qword ptr[%3] \n"
		"fmulp st(1), st \n"
		"fadd qword ptr[%2] \n"
		"2: \n"
		__MCFCRT_FLT_RET_ST("%1")
		: __MCFCRT_FLT_RET_CONS(ret)
		: "m"(x), "m"(__MCFCRT_kMath_Neg_1_0), "m"(__MCFCRT_kMath_Pos_1_0)
		: "ax"
	);
	return ret;
}

double expm1(double x){
	register double ret;
	__asm__ volatile (
		"fldl2e \n"
		"fmul qword ptr[%1] \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	f2xm1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld st \n"
		"frndint \n"
		"fsub st(1), st \n"
		"fld1 \n"
		"fscale \n"
		"fstp st(1) \n"
		"fxch st(1) \n"
		"f2xm1 \n"
		"fadd qword ptr[%3] \n"
		"fmulp st(1), st \n"
		"fadd qword ptr[%2] \n"
		"2: \n"
		__MCFCRT_DBL_RET_ST("%1")
		: __MCFCRT_DBL_RET_CONS(ret)
		: "m"(x), "m"(__MCFCRT_kMath_Neg_1_0), "m"(__MCFCRT_kMath_Pos_1_0)
		: "ax"
	);
	return ret;
}

long double expm1l(long double x){
	register long double ret;
	__asm__ volatile (
		"fld tbyte ptr[%1] \n"
		"fldl2e \n"
		"fmulp st(1), st \n"
		"fcom qword ptr[%2] \n"
		"fstsw ax \n"
		"test ah, 0x41 \n"
		"jnz 1f \n"
		"fcom qword ptr[%3] \n"
		"fstsw ax \n"
		"test ah, 0x01 \n"
		"jz 1f \n"
		"	f2xm1 \n"
		"	jmp 2f \n"
		"1: \n"
		"fld st \n"
		"frndint \n"
		"fsub st(1), st \n"
		"fld1 \n"
		"fscale \n"
		"fstp st(1) \n"
		"fxch st(1) \n"
		"f2xm1 \n"
		"fadd qword ptr[%3] \n"
		"fmulp st(1), st \n"
		"fadd qword ptr[%2] \n"
		"2: \n"
		__MCFCRT_LDBL_RET_ST("%1")
		: __MCFCRT_LDBL_RET_CONS(ret)
		: "m"(x), "m"(__MCFCRT_kMath_Neg_1_0), "m"(__MCFCRT_kMath_Pos_1_0)
		: "ax"
	);
	return ret;
}
