// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2014, LH_Mouse. All wrongs reserved.

#ifndef MCF_CORE_SYSTEM_HPP_
#define MCF_CORE_SYSTEM_HPP_

#include <cstddef>
#include <cstdint>

namespace MCF {

std::size_t GetProcessorCount() noexcept;
std::size_t GetPageSize() noexcept;

}

#endif