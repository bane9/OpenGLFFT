#pragma once

#include <cstdint>

inline uint32_t nextPoT(uint32_t x)
{
	--x;
	x |= x >> 1u;
	x |= x >> 2u;
	x |= x >> 4u;
	x |= x >> 8u;
	x |= x >> 16u;
	
	return ++x;
}

inline uint32_t fft2d_clz(uint32_t x)
{
	static constexpr uint32_t lut[32] = {
		0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
		1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
	};

	++x;
	x = nextPoT(x);

	return lut[x * 0x076be629U >> 27U];
}

