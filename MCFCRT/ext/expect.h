// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2015, LH_Mouse. All wrongs reserved.

#ifndef MCF_CRT_EXPECT_H_
#define MCF_CRT_EXPECT_H_

#include "../env/_crtdef.h"

#define EXPECT(x_)		(__builtin_expect(!!(x_), 1))
#define EXPECT_NOT(x_)	(__builtin_expect(!!(x_), 0))

#endif
