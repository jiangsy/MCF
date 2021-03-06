// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef MCF_UTILITIES_VARIANT_HPP_
#define MCF_UTILITIES_VARIANT_HPP_

#include "../Core/ParameterPackManipulators.hpp"
#include "../Core/Assert.hpp"
#include "../Core/Exception.hpp"
#include "../SmartPointers/UniquePtr.hpp"
#include <utility>
#include <typeinfo>
#include <cstddef>

namespace MCF {

namespace Impl_Variant {
	class ActiveElementBase {
	public:
		virtual ~ActiveElementBase();

	public:
		virtual std::size_t GetIndex() const noexcept = 0;
		virtual const std::type_info *GetTypeInfo() const noexcept = 0;
		virtual void *GetAddress() noexcept = 0;
		virtual UniquePtr<ActiveElementBase> Clone() const = 0;
	};

	template<typename FunctorT, std::size_t kIndex, typename ...ElementsT>
	struct Applier;

	template<typename FunctorT, std::size_t kIndex, typename FirstT, typename ...RemainingT>
	struct Applier<FunctorT, kIndex, FirstT, RemainingT...> {
		void operator()(FunctorT &vFunctor, std::size_t uActiveIndex, void *pElement) const {
			if(uActiveIndex == kIndex){
				std::forward<FunctorT>(vFunctor)(*static_cast<FirstT *>(pElement));
				return;
			}
			Applier<FunctorT, kIndex + 1, RemainingT...>()(vFunctor, uActiveIndex, pElement);
		}
	};
	template<typename FunctorT, std::size_t kIndex>
	struct Applier<FunctorT, kIndex> {
		[[noreturn]] void operator()(FunctorT &vFunctor, std::size_t uActiveIndex, void *pElement) const {
			(void)vFunctor;
			(void)uActiveIndex;
			(void)pElement;

			MCF_DEBUG_CHECK(false);
			std::terminate();
		}
	};
}

template<typename ...ElementsT>
class Variant {
public:
	enum : std::size_t {
		kEmpty = (std::size_t)-1,
	};

private:
	template<typename ElementT>
	class X_ActiveElement final : public Impl_Variant::ActiveElementBase {
	private:
		ElementT x_vElement;

	public:
		template<typename ...ParamsT>
		explicit X_ActiveElement(ParamsT &&...vParams)
			: x_vElement(std::forward<ParamsT>(vParams)...)
		{
		}
		~X_ActiveElement() override {
		}

	public:
		std::size_t GetIndex() const noexcept override {
			return FindFirstType<ElementT, ElementsT...>();
		}
		const std::type_info *GetTypeInfo() const noexcept override {
			return &typeid(ElementT);
		}
		void *GetAddress() noexcept override {
			return &reinterpret_cast<char &>(x_vElement);
		}
		UniquePtr<Impl_Variant::ActiveElementBase> Clone() const override {
			return MakeUnique<X_ActiveElement>(*this);
		}
	};

private:
	UniquePtr<Impl_Variant::ActiveElementBase> x_pElement;

public:
	constexpr Variant() noexcept
		: x_pElement()
	{
	}
	template<typename ParamT, std::enable_if_t<
		!std::is_base_of<Variant, ParamT>::value &&
			(FindFirstType<std::decay_t<ParamT>, ElementsT...>() != (std::size_t)-1),
		int> = 0>
	Variant(ParamT &&vParam)
		: Variant()
	{
		Set(std::forward<ParamT>(vParam));
	}
	Variant(const Variant &rhs)
		: Variant()
	{
		Set(rhs);
	}
	Variant(Variant &&rhs) noexcept
		: Variant()
	{
		Set(std::move(rhs));
	}
	template<typename ParamT, std::enable_if_t<
		!std::is_base_of<Variant, ParamT>::value &&
			(FindFirstType<std::decay_t<ParamT>, ElementsT...>() != (std::size_t)-1),
		int> = 0>
	Variant &operator=(ParamT &&vParam){
		Set(std::forward<ParamT>(vParam));
		return *this;
	}
	Variant &operator=(const Variant &rhs){
		Set(rhs);
		return *this;
	}
	Variant &operator=(Variant &&rhs) noexcept {
		Set(std::move(rhs));
		return *this;
	}

public:
	std::size_t GetIndex() const noexcept {
		if(!x_pElement){
			return kEmpty;
		}
		return x_pElement->GetIndex();
	}
	const std::type_info *GetTypeInfo() const noexcept {
		if(!x_pElement){
			return nullptr;
		}
		return x_pElement->GetTypeInfo();
	}
	template<typename ParamT>
	const ParamT *Get() const noexcept {
		using Element = std::decay_t<ParamT>;
		if(GetIndex() != FindFirstType<Element, ElementsT...>()){
			return nullptr;
		}
		MCF_DEBUG_CHECK(dynamic_cast<const X_ActiveElement<Element> *>(x_pElement.Get()));
		return static_cast<const Element *>(static_cast<const X_ActiveElement<Element> *>(x_pElement.Get())->GetAddress());
	}
	template<typename ParamT>
	ParamT *Get() noexcept {
		using Element = std::decay_t<ParamT>;
		if(GetIndex() != FindFirstType<ParamT, ElementsT...>()){
			return nullptr;
		}
		MCF_DEBUG_CHECK(dynamic_cast<X_ActiveElement<Element> *>(x_pElement.Get()));
		return static_cast<Element *>(static_cast<X_ActiveElement<Element> *>(x_pElement.Get())->GetAddress());
	}
	template<typename ElementT, typename ...ParamsT>
	void Emplace(ParamsT &&...vParams){
		static_assert(FindFirstType<ElementT, ElementsT...>() != (std::size_t)-1, "ElementT is not found in ElementsT.");

		ElementT *pReplaceableElement;
		if(std::is_nothrow_constructible<ElementT, ParamsT &&...>::value && std::is_nothrow_move_assignable<ElementT>::value){
			pReplaceableElement = Get<ElementT>();
		} else {
			pReplaceableElement = nullptr;
		}
		if(pReplaceableElement){
			*pReplaceableElement = ElementT(std::forward<ParamsT>(vParams)...);
		} else {
			x_pElement = MakeUnique<X_ActiveElement<ElementT>>(std::forward<ParamsT>(vParams)...);
		}
	}
	template<typename ElementT>
	void Set(ElementT vElement){
		Emplace<ElementT>(std::move(vElement));
	}
	void Set(const Variant &rhs){
		x_pElement = rhs.x_pElement ? rhs.x_pElement->Clone() : nullptr;
	}
	void Set(Variant &&rhs) noexcept {
		x_pElement = std::move(rhs.x_pElement);
	}

	template<typename FunctorT>
	void Apply(FunctorT &&vFunctor) const {
		if(!x_pElement){
			MCF_THROW(Exception, ERROR_NOT_READY, Rcntws::View(L"Variant: 尚未设定活动元素。"));
		}
		Impl_Variant::Applier<FunctorT, 0, const ElementsT...>()(
			vFunctor, x_pElement->GetIndex(), x_pElement->GetAddress());
	}
	template<typename FunctorT>
	void Apply(FunctorT &&vFunctor){
		if(!x_pElement){
			MCF_THROW(Exception, ERROR_NOT_READY, Rcntws::View(L"Variant: 尚未设定活动元素。"));
		}
		Impl_Variant::Applier<FunctorT, 0, ElementsT...>()(
			vFunctor, x_pElement->GetIndex(), x_pElement->GetAddress());
	}

	void Swap(Variant<ElementsT...> &rhs) noexcept {
		using std::swap;
		swap(x_pElement, rhs.x_pElement);
	}

public:
	explicit operator bool() const noexcept {
		return GetIndex() != kEmpty;
	}

	friend void swap(Variant &lhs, Variant &rhs) noexcept {
		lhs.Swap(rhs);
	}
};

}

#endif
