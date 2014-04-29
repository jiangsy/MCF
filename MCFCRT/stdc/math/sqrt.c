// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2014. LH_Mouse. All wrongs reserved.

#include "../../env/_crtdef.h"
#include "_mathasm.h"

float sqrtf(float x){
	register float ret;
	__asm__ __volatile__(
		"fld dword ptr[%1] \n"
		"fsqrt \n"
		__FLT_RET_ST("%1")
		: __FLT_RET_CONS(ret)
		: "m"(x)
	);
	return ret;
}

double sqrt(double x){
	register double ret;
	__asm__ __volatile__(
		"fld qword ptr[%1] \n"
		"fsqrt \n"
		__DBL_RET_ST("%1")
		: __DBL_RET_CONS(ret)
		: "m"(x)
	);
	return ret;
}

__LDBL_DECL(sqrtl, long double x){
	register long double ret;
	__asm__ __volatile__(
		"fld tbyte ptr[%1] \n"
		"fsqrt \n"
		__LDBL_RET_ST()
		: __LDBL_RET_CONS(ret)
		: "m"(x), __LDBL_RET_CONS_IN()
	);
	__LDBL_RETURN(ret);
}