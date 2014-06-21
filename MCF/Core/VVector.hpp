// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2014. LH_Mouse. All wrongs reserved.

#ifndef MCF_V_VECTOR_HPP_
#define MCF_V_VECTOR_HPP_

#include "../../MCFCRT/ext/assert.h"
#include "Utilities.hpp"
#include <initializer_list>
#include <type_traits>
#include <iterator>
#include <utility>
#include <cstddef>

namespace MCF {

template<class Element_t, std::size_t ALT_STOR_THRESHOLD = 256u / sizeof(Element_t)>
class VVector {
	template<class, std::size_t>
	friend class VVector;

private:
	Element_t *xm_pBegin;
	Element_t *xm_pEnd;

	union {
		alignas(std::max_align_t) unsigned char xm_aSmall[sizeof(Element_t) * ALT_STOR_THRESHOLD];
		Element_t *xm_pEndOfStor;
	};

public:
	constexpr VVector() noexcept
		: xm_pBegin	((Element_t *)std::begin(xm_aSmall))
		, xm_pEnd	(xm_pBegin)
	{
	}
	template<typename... Params_t>
	explicit VVector(std::size_t uCount, const Params_t &...vParams)
		: VVector()
	{
		FillAtEnd(uCount, vParams...);
	}
	template<class Iterator_t>
	VVector(Iterator_t itBegin, Iterator_t itEnd)
		: VVector()
	{
		CopyToEnd(itBegin, itEnd);
	}
	template<class Iterator_t>
	VVector(Iterator_t itBegin, std::size_t uCount)
		: VVector()
	{
		CopyToEnd(itBegin, uCount);
	}
	VVector(std::initializer_list<Element_t> rhs)
		: VVector()
	{
		CopyToEnd(rhs.begin(), rhs.size());
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector(const VVector<Element_t, OTHER_THRESHOLD> &rhs)
		: VVector()
	{
		CopyToEnd(rhs.GetBegin(), rhs.GetEnd());
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector(VVector<Element_t, OTHER_THRESHOLD> &&rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value && (ALT_STOR_THRESHOLD >= OTHER_THRESHOLD))
		: VVector()
	{
		xMoveFrom(rhs);
	}
	VVector(const VVector &rhs)
		: VVector()
	{
		CopyToEnd(rhs.GetBegin(), rhs.GetEnd());
	}
	VVector(VVector &&rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value)
		: VVector()
	{
		xMoveFrom(rhs);
	}
	VVector &operator=(std::initializer_list<Element_t> rhs){
		VVector(rhs).Swap(*this);
		return *this;
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector &operator=(const VVector<Element_t, OTHER_THRESHOLD> &rhs){
		VVector(rhs).Swap(*this);
		return *this;
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector &operator=(VVector<Element_t, OTHER_THRESHOLD> &&rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value && (ALT_STOR_THRESHOLD >= OTHER_THRESHOLD))
	{
		VVector(std::move(rhs)).Swap(*this);
		return *this;
	}
	VVector &operator=(const VVector &rhs){
		if(&rhs != this){
			VVector(rhs).Swap(*this);
		}
		return *this;
	}
	VVector &operator=(VVector &&rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value)
	{
		rhs.Swap(*this);
		return *this;
	}
	~VVector() noexcept {
		Clear(true);
	}

private:
	template<std::size_t OTHER_THRESHOLD>
	void xMoveFrom(VVector<Element_t, OTHER_THRESHOLD> &rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value && (ALT_STOR_THRESHOLD >= OTHER_THRESHOLD))
	{
		ASSERT(IsEmpty());
		ASSERT((void *)this != (void *)&rhs);

		if(rhs.xm_pBegin != (Element_t *)std::begin(rhs.xm_aSmall)){
			if(xm_pBegin != (Element_t *)std::begin(xm_aSmall)){
				::operator delete[](xm_pBegin);
			}
			xm_pBegin		= rhs.xm_pBegin;
			xm_pEnd			= rhs.xm_pEnd;
			xm_pEndOfStor	= rhs.xm_pEndOfStor;

			rhs.xm_pBegin	= (Element_t *)std::begin(rhs.xm_aSmall);
			rhs.xm_pEnd		= rhs.xm_pBegin;
		} else {
			auto pRead = rhs.GetBegin();
			const auto uSize = rhs.GetSize();
			if(ALT_STOR_THRESHOLD < OTHER_THRESHOLD){
				Reserve(uSize);
			}
			for(auto i = uSize; i; --i){
				PushNoCheck(std::move_if_noexcept(*pRead));
				++pRead;
			}
		}
	}
	void xMoveFrom(VVector<Element_t, 0> &rhs) noexcept {
		ASSERT(IsEmpty());
		ASSERT((void *)this != (void *)&rhs);

		if(xm_pBegin != (Element_t *)std::begin(xm_aSmall)){
			::operator delete[](xm_pBegin);
		}
		xm_pBegin		= rhs.xm_pBegin;
		xm_pEnd			= rhs.xm_pEnd;
		xm_pEndOfStor	= rhs.xm_pEndOfStor;

		rhs.xm_pBegin	= nullptr;
		rhs.xm_pEnd		= nullptr;
	}

public:
	const Element_t *GetBegin() const noexcept {
		return xm_pBegin;
	}
	Element_t *GetBegin() noexcept {
		return xm_pBegin;
	}
	const Element_t *GetCBegin() const noexcept {
		return GetBegin();
	}
	const Element_t *GetEnd() const noexcept {
		return xm_pEnd;
	}
	Element_t *GetEnd() noexcept {
		return xm_pEnd;
	}
	const Element_t *GetCEnd() const noexcept {
		return GetEnd();
	}

	const Element_t *GetData() const noexcept {
		return GetBegin();
	}
	Element_t *GetData() noexcept {
		return GetBegin();
	}

	std::size_t GetSize() const noexcept {
		return (std::size_t)(GetEnd() - GetBegin());
	}
	template<typename... Params_t>
	void Resize(std::size_t uNewSize, const Params_t &... vParams){
		const std::size_t uOldSize = GetSize();
		if(uNewSize > uOldSize){
			FillAtEnd(uNewSize - uOldSize, vParams...);
		} else if(uNewSize < uOldSize){
			TruncateFromEnd(uOldSize - uNewSize);
		}
	}
	template<typename... Params_t>
	Element_t *ResizeMore(std::size_t uDeltaSize, const Params_t &... vParams){
		const auto uOldSize = GetSize();
		FillAtEnd(uDeltaSize, vParams...);
		return GetData() + uOldSize;
	}

	bool IsEmpty() const noexcept {
		return GetEnd() == GetBegin();
	}
	void Clear(bool bDeallocateBuffer = false) noexcept {
		TruncateFromEnd(GetSize());

		if(bDeallocateBuffer && (xm_pBegin != (Element_t *)std::begin(xm_aSmall))){
			::operator delete[](xm_pBegin);
			xm_pBegin = (Element_t *)std::begin(xm_aSmall);
		}
	}

	std::size_t GetCapacity() const noexcept {
		if(xm_pBegin == (Element_t *)std::begin(xm_aSmall)){
			return sizeof(xm_aSmall) / sizeof(Element_t);
		}
		return (std::size_t)(xm_pEndOfStor - xm_pBegin);
	}
	void Reserve(std::size_t uNewCapacity){
		const auto uOldCapacity = GetCapacity();
		if(uNewCapacity > uOldCapacity){
			auto uSizeToAlloc = uOldCapacity + 1;
			uSizeToAlloc += (uSizeToAlloc >> 1);
			uSizeToAlloc = (uSizeToAlloc + 0xF) & (std::size_t)-0x10;
			uSizeToAlloc = Max(uSizeToAlloc, uNewCapacity);

			const auto pOldBegin = GetBegin();
			const auto pOldEnd = GetEnd();
			const auto pNewBegin = (Element_t *)::operator new[](sizeof(Element_t) * uSizeToAlloc);
			auto pRead = pOldBegin;
			auto pWrite = pNewBegin;
			try {
				while(pRead != pOldEnd){
					Construct<Element_t>(pWrite, std::move_if_noexcept(*pRead));
					++pWrite;
					++pRead;
				}
			} catch(...){
				while(pWrite != pNewBegin){
					--pWrite;
					Destruct(pWrite);
				}
				::operator delete[](pNewBegin);
				throw;
			}
			while(pRead != pOldBegin){
				--pRead;
				Destruct(pRead);
			}

			if(xm_pBegin != (Element_t *)std::begin(xm_aSmall)){
				::operator delete[](xm_pBegin);
			}
			xm_pBegin		= pNewBegin;
			xm_pEnd			= pWrite;
			xm_pEndOfStor	= xm_pBegin + uSizeToAlloc;
		}
	}
	void ReserveMore(std::size_t uDeltaCapacity){
		Reserve(GetSize() + uDeltaCapacity);
	}

	Element_t &PushNoCheck()
		noexcept(std::is_nothrow_constructible<Element_t>::value)
	{
		ASSERT_MSG(GetSize() < GetCapacity(), L"容器已满。");

		if(std::is_pod<Element_t>::value){
#ifndef NDEBUG
			__builtin_memset(xm_pEnd, 0xCC, sizeof(*xm_pEnd));
#endif
		} else {
			Construct<Element_t>(xm_pEnd);
		}
		return *(xm_pEnd++);
	}
	template<typename... Params_t>
	Element_t &PushNoCheck(Params_t &&...vParams)
		noexcept(std::is_nothrow_constructible<Element_t, Params_t &&...>::value)
	{
		ASSERT_MSG(GetSize() < GetCapacity(), L"容器已满。");

		Construct<Element_t>(xm_pEnd, std::forward<Params_t>(vParams)...);
		return *(xm_pEnd++);
	}
	template<typename... Params_t>
	Element_t &Push(Params_t &&...vParams){
		Reserve(GetSize() + 1);
		return PushNoCheck(std::forward<Params_t>(vParams)...);
	}
	void Pop() noexcept {
		ASSERT(!IsEmpty());

		Destruct(--xm_pEnd);
	}

	template<typename... Params_t>
	void FillAtEndNoCheck(std::size_t uCount, const Params_t &...vParams)
		noexcept(std::is_nothrow_constructible<Element_t, const Params_t &...>::value)
	{
		for(std::size_t i = 0; i < uCount; ++i){
			PushNoCheck(vParams...);
		}
	}
	template<class Iterator_t>
	void CopyToEndNoCheck(Iterator_t itBegin, Iterator_t itEnd)
		noexcept(noexcept(PushNoCheck(*std::declval<Iterator_t>())))
	{
		auto itCur = itBegin;
		while(itCur != itEnd){
			PushNoCheck(*itCur);
			++itCur;
		}
	}
	template<class Iterator_t>
	void CopyToEndNoCheck(Iterator_t itBegin, std::size_t uCount)
		noexcept(noexcept(PushNoCheck(*std::declval<Iterator_t>())))
	{
		auto itCur = itBegin;
		for(std::size_t i = 0; i < uCount; ++i){
			PushNoCheck(*itCur);
			++itCur;
		}
	}
	template<typename... Params_t>
	void FillAtEnd(std::size_t uCount, const Params_t &...vParams){
		Reserve(GetSize() + uCount);
		FillAtEndNoCheck(uCount, vParams...);
	}
	template<class Iterator_t>
	void CopyToEnd(Iterator_t itBegin, Iterator_t itEnd){
		auto itCur = itBegin;
		while(itCur != itEnd){
			Push(*itCur);
			++itCur;
		}
	}
	template<class Iterator_t>
	void CopyToEnd(Iterator_t itBegin, std::size_t uCount){
		Reserve(GetSize() + uCount);
		CopyToEndNoCheck(std::move(itBegin), uCount);
	}
	void TruncateFromEnd(std::size_t uCount) noexcept {
		ASSERT(GetSize() >= uCount);

		for(std::size_t i = 0; i < uCount; ++i){
			Pop();
		}
	}

	void Swap(VVector &rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value)
	{
		if(this != &rhs){
			VVector vecTemp;
			vecTemp.xMoveFrom(*this);
			Clear();
			xMoveFrom(rhs);
			rhs.Clear();
			rhs.xMoveFrom(vecTemp);
		}
	}

public:
	explicit operator const Element_t *() const noexcept {
		return GetData();
	}
	explicit operator Element_t *() noexcept {
		return GetData();
	}

	const Element_t &operator[](std::size_t uIndex) const noexcept {
		ASSERT_MSG(uIndex < GetSize(), L"索引越界。");

		return GetData()[uIndex];
	}
	Element_t &operator[](std::size_t uIndex) noexcept {
		ASSERT_MSG(uIndex < GetSize(), L"索引越界。");

		return GetData()[uIndex];
	}
};

template<class Element_t>
class VVector<Element_t, 0> {
	template<class, std::size_t>
	friend class VVector;

private:
	Element_t *xm_pBegin;
	Element_t *xm_pEnd;
	Element_t *xm_pEndOfStor;

public:
	constexpr VVector() noexcept
		: xm_pBegin		(nullptr)
		, xm_pEnd		(xm_pBegin)
		, xm_pEndOfStor	(xm_pBegin)
	{
	}
	template<typename... Params_t>
	explicit VVector(std::size_t uCount, const Params_t &...vParams)
		: VVector()
	{
		FillAtEnd(uCount, vParams...);
	}
	template<class Iterator_t>
	VVector(Iterator_t itBegin, Iterator_t itEnd)
		: VVector()
	{
		CopyToEnd(itBegin, itEnd);
	}
	template<class Iterator_t>
	VVector(Iterator_t itBegin, std::size_t uCount)
		: VVector()
	{
		CopyToEnd(itBegin, uCount);
	}
	VVector(std::initializer_list<Element_t> rhs)
		: VVector()
	{
		CopyToEnd(rhs.begin(), rhs.size());
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector(const VVector<Element_t, OTHER_THRESHOLD> &rhs)
		: VVector()
	{
		CopyToEnd(rhs.GetBegin(), rhs.GetEnd());
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector(VVector<Element_t, OTHER_THRESHOLD> &&rhs)
		: VVector()
	{
		xMoveFrom(rhs);
	}
	VVector(const VVector &rhs)
		: VVector()
	{
		CopyToEnd(rhs.GetBegin(), rhs.GetEnd());
	}
	VVector(VVector &&rhs) noexcept
		: VVector()
	{
		xMoveFrom(rhs);
	}
	VVector &operator=(std::initializer_list<Element_t> rhs){
		VVector(rhs).Swap(*this);
		return *this;
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector &operator=(const VVector<Element_t, OTHER_THRESHOLD> &rhs){
		VVector(rhs).Swap(*this);
		return *this;
	}
	template<std::size_t OTHER_THRESHOLD>
	VVector &operator=(VVector<Element_t, OTHER_THRESHOLD> &&rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value && (0 >= OTHER_THRESHOLD))
	{
		VVector(std::move(rhs)).Swap(*this);
		return *this;
	}
	VVector &operator=(const VVector &rhs){
		if(&rhs != this){
			VVector(rhs).Swap(*this);
		}
		return *this;
	}
	VVector &operator=(VVector &&rhs) noexcept {
		if(&rhs != this){
			rhs.Swap(*this);
		}
		return *this;
	}
	~VVector() noexcept {
		Clear(true);
	}

private:
	template<std::size_t OTHER_THRESHOLD>
	void xMoveFrom(VVector<Element_t, OTHER_THRESHOLD> &rhs)
		noexcept(std::is_nothrow_move_constructible<Element_t>::value && (0 >= OTHER_THRESHOLD))
	{
		ASSERT(IsEmpty());
		ASSERT((void *)this != (void *)&rhs);

		if(rhs.xm_pBegin != (Element_t *)std::begin(rhs.xm_aSmall)){
			::operator delete[](xm_pBegin);
			xm_pBegin		= rhs.xm_pBegin;
			xm_pEnd			= rhs.xm_pEnd;
			xm_pEndOfStor	= rhs.xm_pEndOfStor;

			rhs.xm_pBegin	= (Element_t *)std::begin(rhs.xm_aSmall);
			rhs.xm_pEnd		= rhs.xm_pBegin;
		} else {
			auto pRead = rhs.GetBegin();
			const auto uSize = rhs.GetSize();
			if(0 < OTHER_THRESHOLD){
				Reserve(uSize);
			}
			for(auto i = uSize; i; --i){
				PushNoCheck(std::move_if_noexcept(*pRead));
				++pRead;
			}
		}
	}
	void xMoveFrom(VVector &rhs) noexcept {
		ASSERT(IsEmpty());
		ASSERT((void *)this != (void *)&rhs);

		::operator delete[](xm_pBegin);
		xm_pBegin		= rhs.xm_pBegin;
		xm_pEnd			= rhs.xm_pEnd;
		xm_pEndOfStor	= rhs.xm_pEndOfStor;

		rhs.xm_pBegin	= nullptr;
		rhs.xm_pEnd		= nullptr;
	}

public:
	const Element_t *GetBegin() const noexcept {
		return xm_pBegin;
	}
	Element_t *GetBegin() noexcept {
		return xm_pBegin;
	}
	const Element_t *GetCBegin() const noexcept {
		return GetBegin();
	}
	const Element_t *GetEnd() const noexcept {
		return xm_pEnd;
	}
	Element_t *GetEnd() noexcept {
		return xm_pEnd;
	}
	const Element_t *GetCEnd() const noexcept {
		return GetEnd();
	}

	const Element_t *GetData() const noexcept {
		return GetBegin();
	}
	Element_t *GetData() noexcept {
		return GetBegin();
	}

	std::size_t GetSize() const noexcept {
		return (std::size_t)(GetEnd() - GetBegin());
	}
	template<typename... Params_t>
	void Resize(std::size_t uNewSize, const Params_t &... vParams){
		const std::size_t uOldSize = GetSize();
		if(uNewSize > uOldSize){
			FillAtEnd(uNewSize - uOldSize, vParams...);
		} else if(uNewSize < uOldSize){
			TruncateFromEnd(uOldSize - uNewSize);
		}
	}
	template<typename... Params_t>
	Element_t *ResizeMore(std::size_t uDeltaSize, const Params_t &... vParams){
		const auto uOldSize = GetSize();
		FillAtEnd(uDeltaSize, vParams...);
		return GetData() + uOldSize;
	}

	bool IsEmpty() const noexcept {
		return GetEnd() == GetBegin();
	}
	void Clear(bool bDeallocateBuffer = false) noexcept {
		TruncateFromEnd(GetSize());

		if(bDeallocateBuffer){
			::operator delete[](xm_pBegin);
		}
	}

	std::size_t GetCapacity() const noexcept {
		return (std::size_t)(xm_pEndOfStor - xm_pBegin);
	}
	void Reserve(std::size_t uNewCapacity){
		const auto uOldCapacity = GetCapacity();
		if(uNewCapacity > uOldCapacity){
			auto uSizeToAlloc = uOldCapacity + 1;
			uSizeToAlloc += (uSizeToAlloc >> 1);
			uSizeToAlloc = (uSizeToAlloc + 0xF) & (std::size_t)-0x10;
			uSizeToAlloc = Max(uSizeToAlloc, uNewCapacity);

			const auto pOldBegin = GetBegin();
			const auto pOldEnd = GetEnd();
			const auto pNewBegin = (Element_t *)::operator new[](sizeof(Element_t) * uSizeToAlloc);
			auto pRead = pOldBegin;
			auto pWrite = pNewBegin;
			try {
				while(pRead != pOldEnd){
					Construct<Element_t>(pWrite, std::move_if_noexcept(*pRead));
					++pWrite;
					++pRead;
				}
			} catch(...){
				while(pWrite != pNewBegin){
					--pWrite;
					Destruct(pWrite);
				}
				::operator delete[](pNewBegin);
				throw;
			}
			while(pRead != pOldBegin){
				--pRead;
				Destruct(pRead);
			}

			::operator delete[](xm_pBegin);
			xm_pBegin		= pNewBegin;
			xm_pEnd			= pWrite;
			xm_pEndOfStor	= xm_pBegin + uSizeToAlloc;
		}
	}
	void ReserveMore(std::size_t uDeltaCapacity){
		Reserve(GetSize() + uDeltaCapacity);
	}

	Element_t &PushNoCheck()
		noexcept(std::is_nothrow_constructible<Element_t>::value)
	{
		ASSERT_MSG(GetSize() < GetCapacity(), L"容器已满。");

		if(std::is_pod<Element_t>::value){
#ifndef NDEBUG
			__builtin_memset(xm_pEnd, 0xCC, sizeof(*xm_pEnd));
#endif
		} else {
			Construct<Element_t>(xm_pEnd);
		}
		return *(xm_pEnd++);
	}
	template<typename... Params_t>
	Element_t &PushNoCheck(Params_t &&...vParams)
		noexcept(std::is_nothrow_constructible<Element_t, Params_t &&...>::value)
	{
		ASSERT_MSG(GetSize() < GetCapacity(), L"容器已满。");

		Construct<Element_t>(xm_pEnd, std::forward<Params_t>(vParams)...);
		return *(xm_pEnd++);
	}
	template<typename... Params_t>
	Element_t &Push(Params_t &&...vParams){
		Reserve(GetSize() + 1);
		return PushNoCheck(std::forward<Params_t>(vParams)...);
	}
	void Pop() noexcept {
		ASSERT(!IsEmpty());

		Destruct(--xm_pEnd);
	}

	template<typename... Params_t>
	void FillAtEndNoCheck(std::size_t uCount, const Params_t &...vParams)
		noexcept(std::is_nothrow_constructible<Element_t, const Params_t &...>::value)
	{
		for(std::size_t i = 0; i < uCount; ++i){
			PushNoCheck(vParams...);
		}
	}
	template<class Iterator_t>
	void CopyToEndNoCheck(Iterator_t itBegin, Iterator_t itEnd)
		noexcept(std::is_nothrow_constructible<Element_t, decltype(*std::declval<Iterator_t>())>::value)
	{
		auto itCur = itBegin;
		while(itCur != itEnd){
			PushNoCheck(*itCur);
			++itCur;
		}
	}
	template<class Iterator_t>
	void CopyToEndNoCheck(Iterator_t itBegin, std::size_t uCount)
		noexcept(std::is_nothrow_constructible<Element_t, decltype(*std::declval<Iterator_t>())>::value)
	{
		auto itCur = itBegin;
		for(std::size_t i = 0; i < uCount; ++i){
			PushNoCheck(*itCur);
			++itCur;
		}
	}
	template<typename... Params_t>
	void FillAtEnd(std::size_t uCount, const Params_t &...vParams){
		Reserve(GetSize() + uCount);
		FillAtEndNoCheck(uCount, vParams...);
	}
	template<class Iterator_t>
	void CopyToEnd(Iterator_t itBegin, Iterator_t itEnd){
		auto itCur = itBegin;
		while(itCur != itEnd){
			Push(*itCur);
			++itCur;
		}
	}
	template<class Iterator_t>
	void CopyToEnd(Iterator_t itBegin, std::size_t uCount){
		Reserve(GetSize() + uCount);
		CopyToEndNoCheck(std::move(itBegin), uCount);
	}
	void TruncateFromEnd(std::size_t uCount) noexcept {
		ASSERT(GetSize() >= uCount);

		for(std::size_t i = 0; i < uCount; ++i){
			Pop();
		}
	}

	void Swap(VVector &rhs) noexcept {
		if(this != &rhs){
			std::swap(xm_pBegin,		rhs.xm_pBegin);
			std::swap(xm_pEnd,			rhs.xm_pEnd);
			std::swap(xm_pEndOfStor,	rhs.xm_pEndOfStor);
		}
	}

public:
	explicit operator const Element_t *() const noexcept {
		return GetData();
	}
	explicit operator Element_t *() noexcept {
		return GetData();
	}

	const Element_t &operator[](std::size_t uIndex) const noexcept {
		ASSERT_MSG(uIndex < GetSize(), L"索引越界。");

		return GetData()[uIndex];
	}
	Element_t &operator[](std::size_t uIndex) noexcept {
		ASSERT_MSG(uIndex < GetSize(), L"索引越界。");

		return GetData()[uIndex];
	}
};

template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
const Element_t *begin(const VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetBegin();
}
template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
Element_t *begin(VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetBegin();
}
template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
const Element_t *cbegin(const VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetCBegin();
}

template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
const Element_t *end(const VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetEnd();
}
template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
Element_t *end(VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetEnd();
}
template<typename Element_t, std::size_t ALT_STOR_THRESHOLD>
const Element_t *cend(const VVector<Element_t, ALT_STOR_THRESHOLD> &vec) noexcept {
	return vec.GetCEnd();
}

template<typename Element_t>
using Vector = VVector<Element_t, 0>;

}

#endif
