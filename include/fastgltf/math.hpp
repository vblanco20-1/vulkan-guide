/*
 * Copyright (C) 2022 - 2026 Sean Apeler
 * This file is part of fastgltf <https://github.com/spnda/fastgltf>.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef FASTGLTF_MATH_HPP
#define FASTGLTF_MATH_HPP

#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <cmath>
#include <functional>
#include <initializer_list>
#include <tuple>
#endif

#include <fastgltf/util.hpp>

/**
 * The fastgltf::math namespace contains all math functions and types which are needed for working with glTF assets.
 */
namespace fastgltf::math {
	FASTGLTF_EXPORT inline constexpr long double pi = 3.1415926535897932385L;

	/** Value clamp using std::less */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] decltype(auto) clamp(const T& v, const T& min, const T& max) noexcept {
		std::less comp {};
		return comp(v, min) ? min : (comp(max, v) ? max : v);
	}

	/** Linear interpolation of two values based on t */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto lerp(T a, T b, T t) noexcept {
		return a + t * (b - a);
	}

	/** Degree to radians conversion */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto radians(const T& degrees) noexcept {
		return T(degrees * pi / 180.0);
	}

	/** Radians to degrees conversion */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto degrees(const T& radians) noexcept {
		return T(radians * 180.0 / pi);
	}

	FASTGLTF_EXPORT template <typename T, std::size_t N, std::size_t M>
	class mat;

	FASTGLTF_EXPORT template <typename T, std::size_t N>
	class vec {
		static_assert(N >= 2 && N <= 4);

		std::array<T, N> _data;

	public:
		constexpr vec() noexcept : _data() {}

		explicit vec(T value) noexcept {
			_data.fill(value);
		}

		/** Creates a new vector with N components from N values in the order X, Y, Z, W */
		template <typename... Args, std::enable_if_t<sizeof...(Args) == N, bool> = true>
		constexpr explicit vec(Args... args) noexcept : _data { T(std::forward<Args>(args))... } {}

		static constexpr auto fromPointer(T* ptr) noexcept {
			vec<T, N> ret;
			for (std::size_t i = 0; i < N; ++i)
				ret[i] = ptr[i];
			return ret;
		}

		constexpr vec(const vec&) noexcept = default;
		constexpr vec& operator=(const vec& other) noexcept = default;

		template <typename U, std::enable_if_t<!std::is_same_v<T, U>, bool> = true>
		constexpr explicit vec(const vec<U, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] = static_cast<T>(other[i]);
		}
		template <typename U, std::enable_if_t<!std::is_same_v<T, U>, bool> = true>
		constexpr vec<T, N>& operator=(const vec<U, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] = static_cast<T>(other[i]);
			return *this;
		}

		template <std::size_t M, std::enable_if_t<M >= N, bool> = true>
		constexpr explicit vec(const vec<T, M>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] = other[i];
		}
		template <std::size_t M, std::enable_if_t<M >= N, bool> = true>
		constexpr vec<T, N>& operator=(const vec<T, M>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] = other[i];
			return *this;
		}

		template <std::size_t M, std::enable_if_t<M < N, bool> = true>
		constexpr explicit vec(const vec<T, M>& other) noexcept : vec(T(0)) {
			for (std::size_t i = 0; i < M; ++i)
				(*this)[i] = other[i];
		}
		template <std::size_t M, std::enable_if_t<M < N, bool> = true>
		constexpr vec<T, N>& operator=(const vec<T, M>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] = i < M ? other[i] : T(0);
			return *this;
		}

		constexpr vec(std::initializer_list<T> list) noexcept {
			for (auto it = std::begin(list); it != std::end(list); ++it)
				(*this)[std::distance(std::begin(list), it)] = *it;
		}
		constexpr vec<T, N>& operator=(std::initializer_list<T> list) noexcept {
			for (auto it = std::begin(list); it != std::end(list); ++it)
				(*this)[std::distance(std::begin(list), it)] = *it;
			return *this;
		}

		[[nodiscard]] constexpr std::size_t size() const noexcept {
			return N;
		}
		[[nodiscard]] constexpr std::size_t size_bytes() const noexcept {
			return size() * sizeof(T);
		}

		[[nodiscard]] constexpr decltype(auto) operator[](std::size_t idx) noexcept {
			return _data[idx];
		}
		[[nodiscard]] constexpr decltype(auto) operator[](std::size_t idx) const noexcept {
			return _data[idx];
		}

		[[nodiscard]] constexpr decltype(auto) x() noexcept {
			return (*this)[0];
		}
		[[nodiscard]] constexpr decltype(auto) y() noexcept {
			return (*this)[1];
		}
		[[nodiscard]] constexpr decltype(auto) z() noexcept {
			static_assert(N >= 3);
			return (*this)[2];
		}
		[[nodiscard]] constexpr decltype(auto) w() noexcept {
			static_assert(N >= 4);
			return (*this)[3];
		}
		[[nodiscard]] constexpr decltype(auto) x() const noexcept {
			return (*this)[0];
		}
		[[nodiscard]] constexpr decltype(auto) y() const noexcept {
			return (*this)[1];
		}
		[[nodiscard]] constexpr decltype(auto) z() const noexcept {
			static_assert(N >= 3);
			return (*this)[2];
		}
		[[nodiscard]] constexpr decltype(auto) w() const noexcept {
			static_assert(N >= 4);
			return (*this)[3];
		}

		[[nodiscard]] constexpr auto data() noexcept {
			return _data.data();
		}
		[[nodiscard]] constexpr auto data() const noexcept {
			return _data.data();
		}

		[[nodiscard]] constexpr bool operator==(const vec<T, N>& other) const noexcept {
			for (std::size_t i = 0; i < N; ++i)
				if ((*this)[i] != other[i])
					return false;
			return true;
		}
		[[nodiscard]] constexpr bool operator!=(const vec<T, N>& other) const noexcept {
			return !(*this == other);
		}

		constexpr auto operator*(T scalar) const noexcept {
			return vec<T, N>(*this) *= scalar;
		}
		constexpr auto operator*=(T scalar) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] *= scalar;
			return *this;
		}
		constexpr auto operator*(const vec<T, N>& other) const noexcept {
			return vec<T, N>(*this) *= other;
		}
		constexpr auto operator*=(const vec<T, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] *= other[i];
			return *this;
		}

		constexpr auto operator/(T scalar) const noexcept {
			return vec<T, N>(*this) /= scalar;
		}
		constexpr auto operator/=(T scalar) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] /= scalar;
			return *this;
		}
		constexpr auto operator/(const vec<T, N>& other) const noexcept {
			return vec<T, N>(*this) /= other;
		}
		constexpr auto operator/=(const vec<T, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] /= other[i];
			return *this;
		}

		constexpr auto operator+() const noexcept {
			vec<T, N> ret;
			for (std::size_t i = 0; i < N; ++i)
				ret[i] = +(*this)[i];
			return ret;
		}
		constexpr auto operator+(T scalar) const noexcept {
			return vec<T, N>(*this) += scalar;
		}
		constexpr auto operator+=(T scalar) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] += scalar;
			return *this;
		}
		constexpr auto operator+(const vec<T, N>& other) const noexcept {
			return vec<T, N>(*this) += other;
		}
		constexpr auto operator+=(const vec<T, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] += other[i];
			return *this;
		}
		constexpr auto operator++() noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i]++;
		}
		constexpr auto operator++(int) noexcept {
			vec<T, N> ret(*this);
			++(*this);
			return ret;
		}

		constexpr auto operator-() const noexcept {
			vec<T, N> ret;
			for (std::size_t i = 0; i < N; ++i)
				ret[i] = -(*this)[i];
			return ret;
		}
		constexpr auto operator-(T scalar) const noexcept {
			return vec<T, N>(*this) -= scalar;
		}
		constexpr auto operator-=(T scalar) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] -= scalar;
			return *this;
		}
		constexpr auto operator-(const vec<T, N>& other) const noexcept {
			return vec<T, N>(*this) -= other;
		}
		constexpr auto operator-=(const vec<T, N>& other) noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i] -= other[i];
			return *this;
		}
		constexpr auto operator--() noexcept {
			for (std::size_t i = 0; i < N; ++i)
				(*this)[i]--;
		}
		constexpr auto operator--(int) noexcept {
			vec<T, N> ret(*this);
			--(*this);
			return ret;
		}

		template <std::size_t M, std::enable_if_t<M < N, bool> = true>
		constexpr explicit operator vec<T, M>() const noexcept {
			vec<T, M> ret;
			for (std::size_t i = 0; i < M; ++i)
				ret[i] = (*this)[i];
			return ret;
		}
	};

	/** Reduction sum of a single vector */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] auto sum(const vec<T, N>& a) noexcept {
		T ret = a.x();
		for (std::size_t i = 1; i < N; ++i)
			ret += a[i];
		return ret;
	}

	/** Computes the dot product of two vectors */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] auto dot(const vec<T, N>& a, const vec<T, N>& b) noexcept {
		return sum(a * b);
	}

	/** Computes the 3D cross product of two vectors */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept {
		return vec<T, 3>(
			a.y() * b.z() - a.z() * b.y(),
			a.z() * b.x() - a.x() * b.z(),
			a.x() * b.y() - a.y() * b.x()
		);
	}

	/** Computes the euclidean length of this vector */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] T length(const vec<T, N>& v) noexcept {
		return sqrt(dot(v, v));
	}

	/** Normalizes the vector to have a length of 1 */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] auto normalize(const vec<T, N>& v) noexcept {
		return v / length(v);
	}

	/** Component-wise vector clamp */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] auto clamp(const vec<T, N>& v, const T& min, const T& max) noexcept {
		vec<T, N> ret = v;
		for (std::size_t i = 0; i < N; ++i)
			ret[i] = clamp(ret[i], min, max);
		return ret;
	}

	/** Component-wise vector lerp */
	FASTGLTF_EXPORT template <typename T, std::size_t N>
	[[nodiscard]] auto lerp(const vec<T, N>& a, const vec<T, N>& b, const T& t) noexcept {
		vec<T, N> ret;
		for (std::size_t i = 0; i < N; ++i)
			ret[i] = lerp(a[i], b[i], t);
		return ret;
	}

	FASTGLTF_EXPORT template <std::size_t N> using s8vec = vec<std::int8_t, N>;
	FASTGLTF_EXPORT using s8vec2 = s8vec<2>;
	FASTGLTF_EXPORT using s8vec3 = s8vec<3>;
	FASTGLTF_EXPORT using s8vec4 = s8vec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using u8vec = vec<std::uint8_t, N>;
	FASTGLTF_EXPORT using u8vec2 = u8vec<2>;
	FASTGLTF_EXPORT using u8vec3 = u8vec<3>;
	FASTGLTF_EXPORT using u8vec4 = u8vec<4>;

	FASTGLTF_EXPORT template <std::size_t N> using s16vec = vec<std::int16_t, N>;
	FASTGLTF_EXPORT using s16vec2 = s16vec<2>;
	FASTGLTF_EXPORT using s16vec3 = s16vec<3>;
	FASTGLTF_EXPORT using s16vec4 = s16vec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using u16vec = vec<std::uint16_t, N>;
	FASTGLTF_EXPORT using u16vec2 = u16vec<2>;
	FASTGLTF_EXPORT using u16vec3 = u16vec<3>;
	FASTGLTF_EXPORT using u16vec4 = u16vec<4>;

	FASTGLTF_EXPORT template <std::size_t N> using s32vec = vec<std::int32_t, N>;
	FASTGLTF_EXPORT using s32vec2 = s32vec<2>;
	FASTGLTF_EXPORT using s32vec3 = s32vec<3>;
	FASTGLTF_EXPORT using s32vec4 = s32vec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using u32vec = vec<std::uint32_t, N>;
	FASTGLTF_EXPORT using u32vec2 = u32vec<2>;
	FASTGLTF_EXPORT using u32vec3 = u32vec<3>;
	FASTGLTF_EXPORT using u32vec4 = u32vec<4>;

	FASTGLTF_EXPORT template <std::size_t N> using s64vec = vec<std::int64_t, N>;
	FASTGLTF_EXPORT using s64vec2 = s64vec<2>;
	FASTGLTF_EXPORT using s64vec3 = s64vec<3>;
	FASTGLTF_EXPORT using s64vec4 = s64vec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using u64vec = vec<std::uint64_t, N>;
	FASTGLTF_EXPORT using u64vec2 = u64vec<2>;
	FASTGLTF_EXPORT using u64vec3 = u64vec<3>;
	FASTGLTF_EXPORT using u64vec4 = u64vec<4>;

	FASTGLTF_EXPORT template <std::size_t N> using ivec = vec<int, N>;
	FASTGLTF_EXPORT using ivec2 = ivec<2>;
	FASTGLTF_EXPORT using ivec3 = ivec<3>;
	FASTGLTF_EXPORT using ivec4 = ivec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using uvec = vec<unsigned int, N>;
	FASTGLTF_EXPORT using uvec2 = uvec<2>;
	FASTGLTF_EXPORT using uvec3 = uvec<3>;
	FASTGLTF_EXPORT using uvec4 = uvec<4>;

	FASTGLTF_EXPORT template <std::size_t N> using fvec = vec<float, N>;
	FASTGLTF_EXPORT using fvec2 = fvec<2>;
	FASTGLTF_EXPORT using fvec3 = fvec<3>;
	FASTGLTF_EXPORT using fvec4 = fvec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using f32vec = vec<float, N>;
	FASTGLTF_EXPORT using f32vec2 = f32vec<2>;
	FASTGLTF_EXPORT using f32vec3 = f32vec<3>;
	FASTGLTF_EXPORT using f32vec4 = f32vec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using dvec = vec<double, N>;
	FASTGLTF_EXPORT using dvec2 = dvec<2>;
	FASTGLTF_EXPORT using dvec3 = dvec<3>;
	FASTGLTF_EXPORT using dvec4 = dvec<4>;
	FASTGLTF_EXPORT template <std::size_t N> using f64vec = vec<double, N>;
	FASTGLTF_EXPORT using f64vec2 = f64vec<2>;
	FASTGLTF_EXPORT using f64vec3 = f64vec<3>;
	FASTGLTF_EXPORT using f64vec4 = f64vec<4>;

	/** A quaternion */
	FASTGLTF_EXPORT template <typename T>
	class quat {
		static_assert(std::is_floating_point_v<T>);

		std::array<T, 4> _data;

	public:
		constexpr explicit quat() noexcept : _data {0.f, 0.f, 0.f, 1.f} {}

		/** Creates a new quaternion from 4 floats in the order X, Y, Z, W */
		template <typename... Args, std::enable_if_t<sizeof...(Args) == 4, bool> = true>
		constexpr explicit quat(Args... args) noexcept : _data { std::forward<Args>(args)... } {}

		static constexpr auto fromPointer(T* ptr) noexcept {
			quat<T> ret;
			for (std::size_t i = 0; i < ret.size(); ++i)
				ret[i] = ptr[i];
			return ret;
		}

		[[nodiscard]] constexpr std::size_t size() const noexcept {
			return _data.size();
		}
		[[nodiscard]] constexpr std::size_t size_bytes() const noexcept {
			return size() * sizeof(T);
		}

		[[nodiscard]] constexpr decltype(auto) operator[](std::size_t idx) noexcept {
			return _data[idx];
		}
		[[nodiscard]] constexpr decltype(auto) operator[](std::size_t idx) const noexcept {
			return _data[idx];
		}

		[[nodiscard]] constexpr decltype(auto) x() noexcept {
			return (*this)[0];
		}
		[[nodiscard]] constexpr decltype(auto) y() noexcept {
			return (*this)[1];
		}
		[[nodiscard]] constexpr decltype(auto) z() noexcept {
			return (*this)[2];
		}
		[[nodiscard]] constexpr decltype(auto) w() noexcept {
			return (*this)[3];
		}
		[[nodiscard]] constexpr decltype(auto) x() const noexcept {
			return (*this)[0];
		}
		[[nodiscard]] constexpr decltype(auto) y() const noexcept {
			return (*this)[1];
		}
		[[nodiscard]] constexpr decltype(auto) z() const noexcept {
			return (*this)[2];
		}
		[[nodiscard]] constexpr decltype(auto) w() const noexcept {
			return (*this)[3];
		}

		[[nodiscard]] constexpr auto data() noexcept {
			return _data.data();
		}
		[[nodiscard]] constexpr auto data() const noexcept {
			return _data.data();
		}

		[[nodiscard]] constexpr bool operator==(const quat<T>& other) const noexcept {
			for (std::size_t i = 0; i < size(); ++i)
				if ((*this)[i] != other[i])
					return false;
			return true;
		}
		[[nodiscard]] constexpr bool operator!=(const quat<T>& other) const noexcept {
			return !(*this == other);
		}

		constexpr auto operator*(T scalar) const noexcept {
			return quat<T>(*this) *= scalar;
		}
		constexpr auto operator*=(T scalar) noexcept {
			for (std::size_t i = 0; i < 4; ++i)
				(*this)[i] *= scalar;
			return *this;
		}

		constexpr auto operator/(T scalar) const noexcept {
			return quat<T>(*this) *= scalar;
		}
		constexpr auto operator/=(T scalar) noexcept {
			for (std::size_t i = 0; i < 4; ++i)
				(*this)[i] /= scalar;
			return *this;
		}

		constexpr auto operator+() const noexcept {
			quat<T> ret(*this);
			for (std::size_t i = 0; i < 4; ++i)
				ret[i] = +ret[i];
			return ret;
		}
		constexpr auto operator+(const quat<T>& other) const noexcept {
			return quat<T>(*this) += other;
		}
		constexpr auto operator+=(const quat<T>& other) noexcept {
			for (std::size_t i = 0; i < 4; ++i)
				(*this)[i] += other[i];
			return *this;
		}

		constexpr auto operator-() const noexcept {
			quat<T> ret(*this);
			for (std::size_t i = 0; i < 4; ++i)
				ret[i] = -ret[i];
			return ret;
		}
		constexpr auto operator-(const quat<T>& other) const noexcept {
			return quat<T>(*this) -= other;
		}
		constexpr auto operator-=(const quat<T>& other) noexcept {
			for (std::size_t i = 0; i < 4; ++i)
				(*this)[i] -= other[i];
			return *this;
		}
	};

	/** Computes the dot product of two quaternions */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto dot(const quat<T>& a, const quat<T>& b) noexcept {
		T ret(0);
		for (std::size_t i = 0; i < 4; ++i)
			ret += a[i] * b[i];
		return ret;
	}

	/** Normalizes the quaternion to have a length of 1 */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto normalize(const quat<T>& q) noexcept {
		return q / sqrt(dot(q, q));
	}

	/** Spherical interpolation of two quaternions */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto slerp(quat<T> a, quat<T> b, T interpolation) noexcept {
		auto d = dot(a, b);

		if (d < T(0)) {
			a = -a;
			d = -d;
		}

		if (d > T(0.9995)) // Simple linear interpolation when both quats are close to each other
			return normalize(a + (b - a) * interpolation);

		auto theta0 = std::acos(d);
		auto sinTheta0 = std::sin(theta0);
		auto theta = interpolation * theta0;
		auto sinTheta = std::sin(theta);

		return a * (cos(theta) - d * sinTheta / sinTheta0) + b * (sinTheta / sinTheta0);
	}

	/**  Converts the given quaternion into a 3x3 rotation matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto asMatrix(const quat<T>& rot) noexcept {
		vec<T, 3> c1(
			T(1) - T(2) * (rot.y() * rot.y() + rot.z() * rot.z()),
			T(2) * (rot.x() * rot.y() + rot.w() * rot.z()),
			T(2) * (rot.x() * rot.z() - rot.w() * rot.y())
		);
		vec<T, 3> c2(
			T(2) * (rot.x() * rot.y() - rot.w() * rot.z()),
			T(1) - T(2) * (rot.x() * rot.x() + rot.z() * rot.z()),
			T(2) * (rot.y() * rot.z() + rot.w() * rot.x())
		);
		vec<T, 3> c3(
			T(2) * (rot.x() * rot.z() + rot.w() * rot.y()),
			T(2) * (rot.y() * rot.z() - rot.w() * rot.x()),
			T(1) - T(2) * (rot.x() * rot.x() + rot.y() * rot.y())
		);
		return mat<T, 3, 3>(c1, c2, c3);
	}

	FASTGLTF_EXPORT using fquat = quat<float>;
	FASTGLTF_EXPORT using dquat = quat<double>;

	/**
	 * A matrix stored in column major order.
	 *
	 * @tparam N Number of rows, or the length of a single column.
	 * @tparam M Number of columns, or the length of a single row.
	 */
	template <typename T, std::size_t N, std::size_t M>
	class mat {
		static_assert(N >= 2 && N <= 4);
		static_assert(M >= 2 && M <= 4);

		// Every vec<> here is a column, with M being the column count.
		std::array<vec<T, N>, M> _data;

		template <typename... Args, std::size_t... i>
		constexpr void copy_values(const std::tuple<Args...>& tuple, std::integer_sequence<std::size_t, i...>) noexcept {
			(..., (_data[i / M][i % N] = std::get<i>(std::move(tuple))));
		}

	public:
		/** Initialises a identity matrix with a specified value. */
		constexpr explicit mat(T value = T(1)) noexcept {
			for (std::size_t i = 0U; i < fastgltf::min(N, M); ++i)
				_data[i][i] = value;
		}

		/** Creates a matrix from M vectors */
		template <typename... Args, std::enable_if_t<sizeof...(Args) == M, bool> = true>
		constexpr explicit mat(Args... args) noexcept : _data { std::forward<Args>(args)... } {}

		/** Creates a matrix from N * M floats */
		template <typename... Args, std::enable_if_t<sizeof...(Args) == M * N, bool> = true>
		constexpr explicit mat(Args... args) noexcept {
			const auto tuple = std::forward_as_tuple(args...);
			copy_values(tuple, std::make_integer_sequence<std::size_t, sizeof...(Args)>());
		}

		constexpr mat(const mat& other) = default;

		/** Truncates the matrix to a smaller one, discarding the additional rows and/or colums  */
		template <std::size_t Q, std::size_t P, std::enable_if_t<N < Q && M < P, bool> = true>
		constexpr explicit mat(const mat<T, Q, P>& other) noexcept {
			for (std::size_t i = 0; i < columns(); ++i)
				(*this).col(i) = vec<T, N>(other.col(i));
		}

		/** Creates a larger matrix from a smaller one, while initializing the new components to identity  */
		template <std::size_t Q, std::size_t P, std::enable_if_t<Q < N && P < M, bool> = true>
		constexpr explicit mat(const mat<T, Q, P>& other) noexcept : mat(T(1)) {
			for (std::size_t i = 0; i < other.columns(); ++i)
				(*this).col(i) = vec<T, N>(other.col(i));
		}

		constexpr mat& operator=(const mat& other) = default;

		[[nodiscard]] constexpr std::size_t columns() const noexcept {
			return M;
		}
		[[nodiscard]] constexpr std::size_t rows() const noexcept {
			return N;
		}
		[[nodiscard]] constexpr std::size_t size() const noexcept {
			return rows() * columns();
		}
		[[nodiscard]] constexpr std::size_t size_bytes() const noexcept {
			return size() * sizeof(T);
		}

		/** Returns the column vector at the given index. */
		[[nodiscard]] FASTGLTF_FORCEINLINE constexpr decltype(auto) operator[](std::size_t idx) noexcept {
			return col(idx);
		}
		[[nodiscard]] FASTGLTF_FORCEINLINE constexpr decltype(auto) operator[](std::size_t idx) const noexcept {
			return col(idx);
		}

		[[nodiscard]] FASTGLTF_FORCEINLINE constexpr decltype(auto) col(std::size_t idx) noexcept {
			return _data[idx];
		}
		[[nodiscard]] FASTGLTF_FORCEINLINE constexpr decltype(auto) col(std::size_t idx) const noexcept {
			return _data[idx];
		}

		/** Returns the row vector at the given index. Note that this is always a copy. */
		template <typename Indices = std::make_integer_sequence<std::size_t, M>>
		[[nodiscard]] constexpr auto row(std::size_t idx) const noexcept {
			return row(idx, Indices{});
		}

	private:
		template <std::size_t... Indices>
		[[nodiscard]] constexpr auto row(std::size_t idx, [[maybe_unused]] std::integer_sequence<std::size_t, Indices...> seq) const noexcept {
			return vec<T, M>(col(Indices)[idx]...);
		}

	public:
		[[nodiscard]] constexpr auto data() noexcept {
			return _data[0].data();
		}
		[[nodiscard]] constexpr auto data() const noexcept {
			return _data[0].data();
		}

		[[nodiscard]] constexpr bool operator==(const mat<T, N, M>& other) const noexcept {
			for (std::size_t i = 0; i < N; ++i)
				if (col(i) != other.col(i))
					return false;
			return true;
		}
		[[nodiscard]] constexpr bool operator!=(const mat<T, N, M>& other) const noexcept {
			return !(*this == other);
		}

		constexpr auto operator-() const noexcept {
			mat<T, N, M> ret;
			for (std::size_t i = 0; i < columns(); ++i)
				ret.col(i) = -(*this).col(i);
			return ret;
		}

		constexpr auto operator*(T scalar) const noexcept {
			mat<T, N, M> ret;
			for (std::size_t i = 0; i < columns(); ++i)
				ret[i] = col(i) * scalar;
			return ret;
		}
		constexpr auto operator*(vec<T, M> other) const noexcept {
			vec<T, M> ret = col(0) * other[0];
			for (std::size_t i = 1; i < columns(); ++i)
				ret += col(i) * other[i];
			return ret;
		}

		constexpr auto operator/(T scalar) const noexcept {
			mat<T, N, M> ret;
			for (std::size_t i = 0; i < columns(); ++i)
				ret[i] = col(i) / scalar;
			return ret;
		}

		template <std::size_t P, std::size_t Q, std::enable_if_t<M == P, bool> = true>
		constexpr auto operator*(const mat<T, P, Q>& other) const noexcept {
			mat<T, N, Q> ret(0.f);
			for (std::size_t i = 0; i < other.columns(); ++i)
				for (std::size_t j = 0; j < rows(); ++j)
					for (std::size_t k = 0; k < columns(); ++k)
						ret.col(i)[j] += col(k)[j] * other.col(i)[k];
			return ret;
		}
	};

	static_assert(std::is_trivially_copyable_v<mat<float, 4, 4>>);
	static_assert(std::is_trivially_copyable_v<vec<float, 4>>);
	static_assert(std::is_trivially_copyable_v<std::array<vec<float, 4>, 4>>);
	static_assert(std::is_trivially_copyable_v<std::array<float, 4>>);

	/** Transposes the given matrix */
	FASTGLTF_EXPORT template <typename T, std::size_t N, std::size_t M>
	[[nodiscard]] auto transpose(const mat<T, N, M>& m) noexcept {
		mat<T, M, N> ret;
		for (std::size_t i = 0; i < N; ++i)
			for (std::size_t j = 0; j < M; ++j)
				ret[i][j] = m[j][i];
		return ret;
	}

	/** Computes the determinant of the 2x2 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto determinant(const mat<T, 2, 2>& m) noexcept {
		return m.col(0).x() * m.col(1).y() - m.col(0).y() * m.col(1).x();
	}

	/** Computes the determinant of the 3x3 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto determinant(const mat<T, 3, 3>& m) noexcept {
		// https://www.mathcentre.ac.uk/resources/uploaded/sigma-matrices9-2009-1.pdf
		// This uses the last row of the matrix, and their minors to ease computation.
		auto lrow = m.row(2);
		// Computes the minors for every element of the last row
		auto cofactors = vec<T, 3>(
			+determinant(mat<T, 2, 2>(vec<T, 2>(m.col(1)), vec<T, 2>(m.col(2)))),
			-determinant(mat<T, 2, 2>(vec<T, 2>(m.col(0)), vec<T, 2>(m.col(2)))),
			+determinant(mat<T, 2, 2>(vec<T, 2>(m.col(0)), vec<T, 2>(m.col(1)))));
		return dot(lrow, cofactors);
	}

	/** Computes the determinant of the 4x4 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto determinant(const mat<T, 4, 4>& m) noexcept {
		const auto d = m.data();
		T a00 = d[0], a10 = d[4], a20 = d[8],  a30 = d[12],
		  a01 = d[1], a11 = d[5], a21 = d[9],  a31 = d[13],
		  a02 = d[2], a12 = d[6], a22 = d[10], a32 = d[14],
		  a03 = d[3], a13 = d[7], a23 = d[11], a33 = d[15];

		T b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10,
		  b03 = a01 * a12 - a02 * a11, b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12,
		  b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30, b08 = a20 * a33 - a23 * a30,
		  b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32;

		return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
	}

	/** Computes the inverse of the 2x2 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto inverse(const mat<T, 2, 2>& m) noexcept {
		const auto inv_determinant = T(1) / determinant(m);
		return mat<T, 2, 2>(m.col(1).y(), -m.col(0).y(), -m.col(1).x(), m.col(0).x()) * inv_determinant;
	}

	/** Computes the inverse of the 3x3 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto inverse(const mat<T, 3, 3>& m) noexcept {
		const auto inv_determinant = T(1) / determinant(m);
		auto inv = mat<T, 3, 3>(
			m.col(1)[1] * m.col(2)[2] - m.col(2)[1] * m.col(1)[2], m.col(2)[1] * m.col(0)[2] - m.col(0)[1] * m.col(2)[2], m.col(0)[1] * m.col(1)[2] - m.col(1)[1] * m.col(0)[2],
			m.col(2)[0] * m.col(1)[2] - m.col(1)[0] * m.col(2)[2], m.col(0)[0] * m.col(2)[2] - m.col(2)[0] * m.col(0)[2], m.col(1)[0] * m.col(0)[2] - m.col(0)[0] * m.col(1)[2],
			m.col(1)[0] * m.col(2)[1] - m.col(2)[0] * m.col(1)[1], m.col(2)[0] * m.col(0)[1] - m.col(0)[0] * m.col(2)[1], m.col(0)[0] * m.col(1)[1] - m.col(1)[0] * m.col(0)[1]);
		return inv * inv_determinant;
	}

	/** Computes the affine inverse of a 4x4 matrix */
	template <typename T>
	[[nodiscard]] auto affineInverse(const mat<T, 4, 4>& m) noexcept {
		const auto inv = inverse(mat<T, 3, 3>(m));
		const auto l = -inv * vec<T, 3>(m.col(3));
		return mat<T, 4, 4>(
			vec<T, 4>(inv.col(0).x(), inv.col(0).y(), inv.col(0).z(), 0.f),
			vec<T, 4>(inv.col(1).x(), inv.col(1).y(), inv.col(1).z(), 0.f),
			vec<T, 4>(inv.col(2).x(), inv.col(2).y(), inv.col(2).z(), 0.f),
			vec<T, 4>(l.x(), l.y(), l.z(), 1.f));
	}

	/** Computes the inverse of a 4x4 matrix */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto inverse(const mat<T, 4, 4>& m) noexcept {
		const auto d = m.data();
		T a00 = d[0], a10 = d[4], a20 = d[8],  a30 = d[12],
		  a01 = d[1], a11 = d[5], a21 = d[9],  a31 = d[13],
		  a02 = d[2], a12 = d[6], a22 = d[10], a32 = d[14],
		  a03 = d[3], a13 = d[7], a23 = d[11], a33 = d[15];

		T b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10,
		  b03 = a01 * a12 - a02 * a11, b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12,
		  b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30, b08 = a20 * a33 - a23 * a30,
		  b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32;

		// Calculate the determinant
		T det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

		if (det == T(0))
			return mat<T, 4, 4>();

		const auto inv_determinant = T(1) / det;
		return mat<T, 4, 4>(
			a11 * b11 - a12 * b10 + a13 * b09, a02 * b10 - a01 * b11 - a03 * b09, a31 * b05 - a32 * b04 + a33 * b03, a22 * b04 - a21 * b05 - a23 * b03,
			a12 * b08 - a10 * b11 - a13 * b07, a00 * b11 - a02 * b08 + a03 * b07, a32 * b02 - a30 * b05 - a33 * b01, a20 * b05 - a22 * b02 + a23 * b01,
			a10 * b10 - a11 * b08 + a13 * b06, a01 * b08 - a00 * b10 - a03 * b06, a30 * b04 - a31 * b02 + a33 * b00, a21 * b02 - a20 * b04 - a23 * b00,
			a11 * b07 - a10 * b09 - a12 * b06, a00 * b09 - a01 * b07 + a02 * b06, a31 * b01 - a30 * b03 - a32 * b00, a20 * b03 - a21 * b01 + a22 * b00) * inv_determinant;
	}

	/** Translates a given transform matrix by the world space translation vector */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto translate(const mat<T, 4, 4>& m, const vec<T, 3>& translation) noexcept {
		mat<T, 4, 4> ret = m;
		ret.col(3) = m.col(0) * translation.x() + m.col(1) * translation.y() + m.col(2) * translation.z() + m.col(3);
		return ret;
	}

	/** Scales a given matrix by the three dimensional scale vector. */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto scale(const mat<T, 4, 4>& m, const vec<T, 3>& scale) noexcept {
		mat<T, 4, 4> ret;
		ret.col(0) = m.col(0) * scale.x();
		ret.col(1) = m.col(1) * scale.y();
		ret.col(2) = m.col(2) * scale.z();
		ret.col(3) = m.col(3);
		return ret;
	}

	/** Rotates the given matrix using the given quaternion */
	FASTGLTF_EXPORT template <typename T>
	[[nodiscard]] auto rotate(const mat<T, 4, 4>& m, const quat<T>& rot) noexcept {
		return m * mat<T, 4, 4>(asMatrix(rot));
	}

	FASTGLTF_EXPORT template <std::size_t N, std::size_t M> using fmat = mat<float, N, M>;
	FASTGLTF_EXPORT using fmat2x2 = fmat<2, 2>;
	FASTGLTF_EXPORT using fmat3x3 = fmat<3, 3>;
	FASTGLTF_EXPORT using fmat4x4 = fmat<4, 4>;
	FASTGLTF_EXPORT template <std::size_t N, std::size_t M> using dmat = mat<double, N, M>;
	FASTGLTF_EXPORT using dmat2x2 = dmat<2, 2>;
	FASTGLTF_EXPORT using dmat3x3 = dmat<3, 3>;
	FASTGLTF_EXPORT using dmat4x4 = dmat<4, 4>;

	/**
	 * Decomposes a transform matrix into the translation, rotation, and scale components. This
	 * function does not support skew, shear, or perspective. This currently uses a quick algorithm
	 * to calculate the quaternion from the rotation matrix, which might occasionally loose some
	 * precision, though we try to use doubles here.
	 */
	FASTGLTF_EXPORT inline void decomposeTransformMatrix(fmat4x4 matrix, fvec3& scale, fquat& rotation, fvec3& translation) {
		// Extract the translation. We zero the translation out, as we reuse the matrix as
		// the rotation matrix at the end.
		translation = matrix.col(3);
		matrix.col(3) = fvec4(0.f, 0.f, 0.f, matrix.col(3)[3]);

		// Extract the scale. We calculate the euclidean length of the columns.
		// We then construct a vector with those lengths.
		scale = math::fvec3(
			length(matrix.col(0)),
			length(matrix.col(1)),
			length(matrix.col(2))
		);

		// Remove the scaling from the matrix, leaving only the rotation.
		// matrix is now the rotation matrix.
		matrix.col(0) /= scale.x();
		matrix.col(1) /= scale.y();
		matrix.col(2) /= scale.z();

		// Construct the quaternion. This algo is copied from here:
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/christian.htm.
		// glTF orders the components as x,y,z,w
		rotation = math::fquat(
			max(.0f, 1.f + matrix[0][0] - matrix[1][1] - matrix[2][2]),
			max(.0f, 1.f - matrix[0][0] + matrix[1][1] - matrix[2][2]),
			max(.0f, 1.f - matrix[0][0] - matrix[1][1] + matrix[2][2]),
			max(.0f, 1.f + matrix[0][0] + matrix[1][1] + matrix[2][2])
		);
		rotation.x() = static_cast<float>(sqrt(static_cast<double>(rotation.x()))) / 2;
		rotation.y() = static_cast<float>(sqrt(static_cast<double>(rotation.y()))) / 2;
		rotation.z() = static_cast<float>(sqrt(static_cast<double>(rotation.z()))) / 2;
		rotation.w() = static_cast<float>(sqrt(static_cast<double>(rotation.w()))) / 2;

		rotation.x() = std::copysignf(rotation.x(), matrix[1][2] - matrix[2][1]);
		rotation.y() = std::copysignf(rotation.y(), matrix[2][0] - matrix[0][2]);
		rotation.z() = std::copysignf(rotation.z(), matrix[0][1] - matrix[1][0]);
	}
} // namespace fastgltf::math

#endif
