// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef MCF_STREAMS_OUTPUT_STREAM_ITERATOR_HPP_
#define MCF_STREAMS_OUTPUT_STREAM_ITERATOR_HPP_

#include "AbstractStreams.hpp"
#include <iterator>

namespace MCF {

class OutputStreamIterator : public std::iterator<std::output_iterator_tag, unsigned char> {
private:
	AbstractOutputStream *x_pstrmOwner;

public:
	explicit constexpr OutputStreamIterator(AbstractOutputStream &strmOwner) noexcept
		: x_pstrmOwner(&strmOwner)
	{
	}

public:
	OutputStreamIterator &operator=(unsigned char byData){
		x_pstrmOwner->Put(byData);
		return *this;
	}
	OutputStreamIterator &operator*(){
		return *this;
	}

	OutputStreamIterator &operator++(){
		return *this;
	}
	OutputStreamIterator &operator++(int){
		return *this;
	}

	void operator()(unsigned char byData){
		*(*this) = byData;
		++(*this);
	}
};

}

#endif