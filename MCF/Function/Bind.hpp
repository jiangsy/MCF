// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2015, LH_Mouse. All wrongs reserved.

#ifndef MCF_FUNCTION_BIND_HPP_
#define MCF_FUNCTION_BIND_HPP_

#include <type_traits>
#include <utility>
#include <tuple>
#include "../Utilities/Invoke.hpp"

namespace MCF {

namespace Impl_Bind {
	template<std::size_t kIndexT>
	struct Placeholder {
	};

	template<typename ...ParamsAddT>
	struct ParamSelector {
		std::tuple<ParamsAddT &&...> tupParamsAdd;

		explicit ParamSelector(ParamsAddT &&...vParamsAdd) noexcept
			: tupParamsAdd(std::forward<ParamsAddT>(vParamsAdd)...)
		{
		}

		template<typename ParamT>
		decltype(auto) operator()(ParamT &vParam) noexcept {
			return vParam;
		}
		template<std::size_t kIndexT>
		decltype(auto) operator()(Placeholder<kIndexT> /* vParam */) noexcept {
			auto &&vRet = std::get<kIndexT - 1>(tupParamsAdd);
			return static_cast<decltype(vRet)>(vRet);
		}
	};

	template<typename FuncT, typename ...ParamsT>
	class BindResult {
	private:
		FuncT x_vFunc;
		std::tuple<ParamsT...> x_tupParams;

	public:
		explicit BindResult(FuncT vFunc, ParamsT ...vParams)
			: x_vFunc(std::move(vFunc)), x_tupParams(std::move(vParams)...)
		{
		}

	private:
		template<std::size_t ...kParamIndicesT, typename ...ParamsAddT>
		decltype(auto) xDispatchParams(std::index_sequence<kParamIndicesT...>, ParamsAddT &&...vParamsAdd) const {
			ParamSelector<ParamsAddT...> vSelector(std::forward<ParamsAddT>(vParamsAdd)...);
			return Invoke(x_vFunc, vSelector(std::get<kParamIndicesT>(x_tupParams))...);
		}

	public:
		template<typename ...ParamsAddT>
		decltype(auto) operator()(ParamsAddT &&...vParamsAdd) const {
			return xDispatchParams(std::index_sequence_for<ParamsT...>(), std::forward<ParamsAddT>(vParamsAdd)...);
		}
	};

}

constexpr Impl_Bind::Placeholder< 1>  _1;
constexpr Impl_Bind::Placeholder< 2>  _2;
constexpr Impl_Bind::Placeholder< 3>  _3;
constexpr Impl_Bind::Placeholder< 4>  _4;
constexpr Impl_Bind::Placeholder< 5>  _5;
constexpr Impl_Bind::Placeholder< 6>  _6;
constexpr Impl_Bind::Placeholder< 7>  _7;
constexpr Impl_Bind::Placeholder< 8>  _8;
constexpr Impl_Bind::Placeholder< 9>  _9;

constexpr Impl_Bind::Placeholder<10> _10;
constexpr Impl_Bind::Placeholder<11> _11;
constexpr Impl_Bind::Placeholder<12> _12;
constexpr Impl_Bind::Placeholder<13> _13;
constexpr Impl_Bind::Placeholder<14> _14;
constexpr Impl_Bind::Placeholder<15> _15;
constexpr Impl_Bind::Placeholder<16> _16;
constexpr Impl_Bind::Placeholder<17> _17;
constexpr Impl_Bind::Placeholder<18> _18;
constexpr Impl_Bind::Placeholder<19> _19;

constexpr Impl_Bind::Placeholder<20> _20;
constexpr Impl_Bind::Placeholder<21> _21;
constexpr Impl_Bind::Placeholder<22> _22;
constexpr Impl_Bind::Placeholder<23> _23;
constexpr Impl_Bind::Placeholder<24> _24;
constexpr Impl_Bind::Placeholder<25> _25;
constexpr Impl_Bind::Placeholder<26> _26;
constexpr Impl_Bind::Placeholder<27> _27;
constexpr Impl_Bind::Placeholder<28> _28;
constexpr Impl_Bind::Placeholder<29> _29;

template<typename FuncT, typename ...ParamsT>
auto Bind(FuncT &&vFunc, ParamsT &&...vParams){
	return Impl_Bind::BindResult<std::decay_t<FuncT>, std::decay_t<ParamsT>...>(std::forward<FuncT>(vFunc), std::forward<ParamsT>(vParams)...);
}

}

#endif