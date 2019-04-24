typedef float fp;//a floating-point number
#define vecscale(o) \
	inline const vec3& operator o(fp scalar) const\
	{\
		return vec3(x o scalar, y o scalar, z o scalar);\
	}\
	inline const vec3& operator o=(fp scalar) const\
	{\
		return vec3(x o scalar, y o scalar, z o scalar);\
	}
#define vecadd(o) \
	inline const vec3& operator o(vec3 other) const\
	{\
		return vec3(x o other.x, y o other.y, z o other.z);\
	}\
	inline const vec3& operator o=(vec3 other) const\
	{\
		return vec3(x o other.x, y o other.y, z o other.z); \
	}
#pragma once
struct vec3
{
public:
	fp x, y, z;
	explicit vec3(fp x, fp y, fp z) : x(x), y(y), z(z) {};
	explicit vec3(fp x, fp y) : x(x), y(y), z(0) {};
	explicit vec3() :x(0), y(0), z(0) {};
	vecscale(*)
	vecscale(/)
	vecadd(*)
	vecadd(/ )
};