// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2015, LH_Mouse. All wrongs reserved.

#ifndef MCF_FUNCTION_FUNCTION_HPP_
#define MCF_FUNCTION_FUNCTION_HPP_

#include "../SmartPointers/IntrusivePtr.hpp"
#include "../Utilities/Assert.hpp"
#include "../Utilities/Invoke.hpp"
#include "_ForwardedParam.hpp"
#include <type_traits>
#include <utility>
#include <functional>

namespace MCF {

namespace Impl_Function {
	template<typename RetT, typename ...ParamsT>
	class FunctorBase : public IntrusiveBase<FunctorBase<RetT, ParamsT...>> {
	public:
		virtual ~FunctorBase() = default;

	public:
		virtual RetT Dispatch(ForwardedParam<ParamsT>...vParams) const = 0;
	};

	template<typename FuncT, typename RetT, typename ...ParamsT>
	class Functor : public FunctorBase<RetT, ParamsT...> {
	private:
		const std::remove_reference_t<FuncT> x_vFunc;

	public:
		explicit Functor(FuncT &&vFunc)
			: x_vFunc(std::forward<FuncT>(vFunc))
		{
		}

	public:
		RetT Dispatch(ForwardedParam<ParamsT>...vParams) const override {
			return Invoke(x_vFunc, std::forward<ParamsT>(vParams)...);
		}
	};
}

template<typename FuncT>
class Function {
	static_assert((sizeof(FuncT), false), "Class template Function instantiated with non-function template type parameter.");
};

template<typename RetT, typename ...ParamsT>
class Function<RetT (ParamsT...)> {
private:
	IntrusivePtr<const Impl_Function::FunctorBase<RetT, ParamsT...>> x_pFunctor;

public:
	constexpr Function() noexcept = default;
	Function(const Function &) noexcept = default;
	Function(Function &&) noexcept = default;
	Function &operator=(const Function &) noexcept = default;
	Function &operator=(Function &&) noexcept = default;

	constexpr Function(std::nullptr_t) noexcept {
	}
	template<typename FuncT,
		std::enable_if_t<
			std::is_convertible<std::result_of_t<FuncT && (ForwardedParam<ParamsT>...)>, RetT>::value,
			int> = 0>
	Function(FuncT &&x_vFunc){
		Reset(std::forward<FuncT>(x_vFunc));
	}
	Function &operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}
	template<typename FuncT,
		std::enable_if_t<
			std::is_convertible<std::result_of_t<FuncT && (ForwardedParam<ParamsT>...)>, RetT>::value,
			int> = 0>
	Function &operator=(FuncT &&x_vFunc){
		Reset(std::forward<FuncT>(x_vFunc));
		return *this;
	}

public:
	void Reset(std::nullptr_t = nullptr) noexcept {
		x_pFunctor.Reset();
	}
	template<typename FuncT,
		std::enable_if_t<
			std::is_convertible<std::result_of_t<FuncT && (ForwardedParam<ParamsT>...)>, RetT>::value,
			int> = 0>
	void Reset(FuncT &&x_vFunc){
		x_pFunctor.Reset(new Impl_Function::Functor<FuncT, RetT, ParamsT...>(std::forward<FuncT>(x_vFunc)));
	}

	void Swap(Function &rhs) noexcept {
		x_pFunctor.Swap(rhs.x_pFunctor);
	}

public:
	explicit operator bool() const noexcept {
		return !!x_pFunctor;
	}
	RetT operator()(ParamsT ...vParams) const {
		ASSERT(x_pFunctor);

		return x_pFunctor->Dispatch(std::forward<ParamsT>(vParams)...); // 值形参当作右值引用传递。
	}
};

template<typename RetT, typename ...ParamsT>
void swap(Function<RetT (ParamsT...)> &lhs, Function<RetT (ParamsT...)> &rhs) noexcept {
	lhs.Swap(rhs);
}

}

#endif