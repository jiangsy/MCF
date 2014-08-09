// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2014, LH_Mouse. All wrongs reserved.

#ifndef MCF_CRT_OFFSET_OF_H_
#define MCF_CRT_OFFSET_OF_H_

#include "../env/_crtdef.h"
#include "_make_constant.h"

#ifdef __cplusplus
#include <memory>
#	define __MCF_ADDRESS_OF(lval)	\
	((unsigned char *)::std::addressof(lval))
#else
#	define __MCF_ADDRESS_OF(lval)	\
	((unsigned char *)&(lval))
#endif

#define OFFSET_OF(s, m)	\
	(__MCF_MAKE_CONSTANT(	\
		((MCF_STD size_t)(__MCF_ADDRESS_OF(((s *)(unsigned char *)1)->m) - (unsigned char *)1))	\
	))

// 成员指针转换成聚合指针。
#define DOWN_CAST(s, m, p)	\
	((s *)((unsigned char *)(p) - OFFSET_OF(s, m)))

#endif
