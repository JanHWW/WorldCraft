#pragma once
/**

 * @file    SimplexNoise.h

 * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).

 *

 * Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)

 *

 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt

 * or copy at http://opensource.org/licenses/MIT)

 */

#pragma once
#include <cstddef>  // size_t
#include <cstdint>  // int32_t/uint8_t

#define NOISE_REPEAT 0x100
 //A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
struct SimplexNoise
{
public:
	//functions:
	SimplexNoise(int seed);
	// 1D Perlin simplex noise

	float noise1(float x);

	// 2D Perlin simplex noise

	float noise2(float x, float y);

	// 3D Perlin simplex noise

	float noise3(float x, float y, float z);
private:
	//variables:
	/**

 * Permutation table. This is just a random jumble of all numbers 0-255.

 *

 * This produce a repeatable pattern of 256, but Ken Perlin stated

 * that it is not a problem for graphic texture as the noise features disappear

 * at a distance far enough to be able to see a repeatable pattern of 256.

 *

 * This needs to be exactly the same for all instances on all platforms,

 * so it's easiest to just keep it as static explicit data.

 * This also removes the need for any initialisation of this class.

 *

 * Note that making this an uint32_t[] instead of a uint8_t[] might make the

 * code run faster on platforms with a high penalty for unaligned single

 * byte addressing. Intel x86 is generally single-byte-friendly, but

 * some other CPUs are faster with 4-aligned reads.

 * However, a char[] is smaller, which avoids cache trashing, and that

 * is probably the most important aspect on most architectures.

 * This array is accessed a *lot* by the noise functions.

 * A vector-valued noise over 3D accesses it 96 times, and a

 * float-valued 4D noise 64 times. We want this to fit in the cache!

 */
	uint8_t perm[NOISE_REPEAT];
	//functions:
	uint8_t hash(int32_t i);
};