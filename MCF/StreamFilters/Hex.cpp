// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2014, LH_Mouse. All wrongs reserved.

#include "../StdMCF.hpp"
#include "Hex.hpp"
using namespace MCF;

// ========== HexEncoder ==========
// 其他非静态成员函数。
void HexEncoder::xDoInit(){
}
void HexEncoder::xDoUpdate(const void *pData, std::size_t uSize){
	static constexpr unsigned char HEX_TABLE[] = "00112233445566778899aAbBcCdDeEfF";

	for(std::size_t i = 0; i < uSize; ++i){
		const unsigned uByte = static_cast<const unsigned char *>(pData)[i];
		unsigned char abyHex[2];
		abyHex[0] = HEX_TABLE[(uByte >> 4) * 2 + (unsigned)xm_bUpperCase];
		abyHex[1] = HEX_TABLE[(uByte & 0x0F) * 2 + (unsigned)xm_bUpperCase];
		xOutput(abyHex, sizeof(abyHex));
	}
}
void HexEncoder::xDoFinalize(){
}

// ========== HexDecoder ==========
// 其他非静态成员函数。
void HexDecoder::xDoInit(){
	xm_nHigh = -1;
}
void HexDecoder::xDoUpdate(const void *pData, std::size_t uSize){
	static constexpr signed char HEX_REVERSE_TABLE[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	};

	for(std::size_t i = 0; i < uSize; ++i){
		const int nDigit = HEX_REVERSE_TABLE[static_cast<const unsigned char *>(pData)[i]];
		if(nDigit == -1){
			continue;
		}
		if(xm_nHigh == -1){
			xm_nHigh = nDigit;
		} else {
			xOutput((unsigned char)((xm_nHigh << 4) | nDigit));
			xm_nHigh = -1;
		}
	}
}
void HexDecoder::xDoFinalize(){
}