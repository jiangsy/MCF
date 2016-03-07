// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef MCF_STREAMS_MD5_OUTPUT_STREAM_HPP_
#define MCF_STREAMS_MD5_OUTPUT_STREAM_HPP_

#include "AbstractOutputStream.hpp"
#include "../Core/Array.hpp"
#include <cstdint>

namespace MCF {

class Md5OutputStream : public AbstractOutputStream {
private:
	int x_nChunkOffset;
	std::uint8_t x_abyChunk[64];
	std::uint32_t x_au32Reg[4];
	std::uint64_t x_u64BytesTotal;

public:
	Md5OutputStream() noexcept
		: x_nChunkOffset(-1), x_abyChunk(), x_au32Reg(), x_u64BytesTotal()
	{
	}
	~Md5OutputStream() override;

	Md5OutputStream(Md5OutputStream &&) noexcept = default;
	Md5OutputStream &operator=(Md5OutputStream &&) noexcept = default;

public:
	void Put(unsigned char byData) override;

	void Put(const void *pData, std::size_t uSize) override;

	void Flush(bool bHard) override;

	void Reset() noexcept;
	Array<std::uint8_t, 16> Finalize() noexcept;

	void Swap(Md5OutputStream &rhs) noexcept {
		using std::swap;
		swap(x_nChunkOffset,  rhs.x_nChunkOffset);
		swap(x_abyChunk,      rhs.x_abyChunk);
		swap(x_au32Reg,       rhs.x_au32Reg);
		swap(x_u64BytesTotal, rhs.x_u64BytesTotal);
	}

	friend void swap(Md5OutputStream &lhs, Md5OutputStream &rhs) noexcept {
		lhs.Swap(rhs);
	}
};

}

#endif
