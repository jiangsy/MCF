// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef MCF_CORE_ALIGNED_STORAGE_HPP_
#define MCF_CORE_ALIGNED_STORAGE_HPP_

#include <cstddef>

namespace MCF {

namespace Impl_AlignedStorage {
	template<std::size_t kFirstT, std::size_t kSecondT>
	struct MaxHelperBinary {
		static constexpr std::size_t kValue = (kFirstT > kSecondT) ? kFirstT : kSecondT;
	};

	template<std::size_t kFirstT, std::size_t ...kRemainingT>
	struct MaxHelper {
		static constexpr std::size_t kValue = MaxHelperBinary<kFirstT, MaxHelper<kRemainingT...>::kValue>::kValue;
	};
	template<std::size_t kFirstT>
	struct MaxHelper<kFirstT> {
		static constexpr std::size_t kValue = kFirstT;
	};
}

template<std::size_t kElementCountT, typename ...ElementsT>
struct AlignedStorage {
	enum : std::size_t {
		kElementCount     = kElementCountT,
		kElementSize      = Impl_AlignedStorage::MaxHelper<kMinSizeT, sizeof(ElementsT)...>::kValue,
		kElementAlignment = Impl_AlignedStorage::MaxHelper<kMinSizeT, alignof(ElementsT)...>::kValue,
	};

	static_assert(kElementCount > 0, "kElementCount must be greater than zero.");
	static_assert(kElementCount <= (std::size_t)-1 / kElementSize, "kElementCount is too large.");

	alignas(kElementAlignment) char a[kElementSize * kElementCount];
};

}

#endif
