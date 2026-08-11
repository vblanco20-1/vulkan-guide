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

#ifndef FASTGLTF_TOOLS_HPP
#define FASTGLTF_TOOLS_HPP

#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <cstring>
#include <iterator>
#endif

#include <fastgltf/types.hpp>

#if FASTGLTF_CPP_23 && __has_include(<stdfloat>)
#include <stdfloat>

#if defined(__STDCPP_FLOAT32_T__) && __STDCPP_FLOAT32_T__
#define FASTGLTF_HAS_FLOAT32 1
#endif

#if defined(__STDCPP_FLOAT64_T__) && __STDCPP_FLOAT64_T__
#define FASTGLTF_HAS_FLOAT64 1
#endif
#else
#define FASTGLTF_HAS_FLOAT32 0
#define FASTGLTF_HAS_FLOAT64 0
#endif

namespace fastgltf {

template <typename>
struct ComponentTypeConverter;

template<>
struct ComponentTypeConverter<std::int8_t> {
	static constexpr auto type = ComponentType::Byte;
};

template<>
struct ComponentTypeConverter<std::uint8_t> {
	static constexpr auto type = ComponentType::UnsignedByte;
};

template<>
struct ComponentTypeConverter<std::int16_t> {
	static constexpr auto type = ComponentType::Short;
};

template<>
struct ComponentTypeConverter<std::uint16_t> {
	static constexpr auto type = ComponentType::UnsignedShort;
};

template<>
struct ComponentTypeConverter<std::int32_t> {
	static constexpr auto type = ComponentType::Int;
};

template<>
struct ComponentTypeConverter<std::uint32_t> {
	static constexpr auto type = ComponentType::UnsignedInt;
};

template<>
struct ComponentTypeConverter<float> {
	static constexpr auto type = ComponentType::Float;
};

template<>
struct ComponentTypeConverter<double> {
	static constexpr auto type = ComponentType::Double;
};

#if FASTGLTF_HAS_FLOAT32
template <>
struct ComponentTypeConverter<std::float32_t> {
	static constexpr auto type = ComponentType::Float;
};
#endif

#if FASTGLTF_HAS_FLOAT64
template <>
struct ComponentTypeConverter<std::float64_t> {
	static constexpr auto type = ComponentType::Double;
};
#endif

FASTGLTF_EXPORT template <typename ElementType, AccessorType EnumAccessorType, typename ComponentType = ElementType, bool transpose = false>
struct ElementTraitsBase {
	using element_type = ElementType;
	using component_type = ComponentType;
	static constexpr auto type = EnumAccessorType;
	static constexpr auto enum_component_type = ComponentTypeConverter<ComponentType>::type;
	static constexpr auto needs_transpose = transpose;
};

FASTGLTF_EXPORT template <typename>
struct ElementTraits;

template<> struct ElementTraits<std::int8_t> : ElementTraitsBase<std::int8_t, AccessorType::Scalar> {};
template<> struct ElementTraits<std::uint8_t> : ElementTraitsBase<std::uint8_t, AccessorType::Scalar> {};
template<> struct ElementTraits<std::int16_t> : ElementTraitsBase<std::int16_t, AccessorType::Scalar> {};
template<> struct ElementTraits<std::uint16_t> : ElementTraitsBase<std::uint16_t, AccessorType::Scalar> {};
template<> struct ElementTraits<std::int32_t> : ElementTraitsBase<std::int32_t, AccessorType::Scalar> {};
template<> struct ElementTraits<std::uint32_t> : ElementTraitsBase<std::uint32_t, AccessorType::Scalar> {};

template<> struct ElementTraits<float> : ElementTraitsBase<float, AccessorType::Scalar> {};
template<> struct ElementTraits<double> : ElementTraitsBase<double, AccessorType::Scalar> {};

#if FASTGLTF_HAS_FLOAT32
template<> struct ElementTraits<std::float32_t> : ElementTraitsBase<std::float32_t, AccessorType::Scalar> {};
#endif

#if FASTGLTF_HAS_FLOAT64
template<> struct ElementTraits<std::float64_t> : ElementTraitsBase<std::float64_t, AccessorType::Scalar> {};
#endif

template<> struct ElementTraits<math::s8vec2> : ElementTraitsBase<math::s8vec2, AccessorType::Vec2, std::int8_t> {};
template<> struct ElementTraits<math::s8vec3> : ElementTraitsBase<math::s8vec3, AccessorType::Vec3, std::int8_t> {};
template<> struct ElementTraits<math::s8vec4> : ElementTraitsBase<math::s8vec4, AccessorType::Vec4, std::int8_t> {};
template<> struct ElementTraits<math::u8vec2> : ElementTraitsBase<math::u8vec2, AccessorType::Vec2, std::uint8_t> {};
template<> struct ElementTraits<math::u8vec3> : ElementTraitsBase<math::u8vec3, AccessorType::Vec3, std::uint8_t> {};
template<> struct ElementTraits<math::u8vec4> : ElementTraitsBase<math::u8vec4, AccessorType::Vec4, std::uint8_t> {};

template<> struct ElementTraits<math::s16vec2> : ElementTraitsBase<math::s16vec2, AccessorType::Vec2, std::int16_t> {};
template<> struct ElementTraits<math::s16vec3> : ElementTraitsBase<math::s16vec3, AccessorType::Vec3, std::int16_t> {};
template<> struct ElementTraits<math::s16vec4> : ElementTraitsBase<math::s16vec4, AccessorType::Vec4, std::int16_t> {};
template<> struct ElementTraits<math::u16vec2> : ElementTraitsBase<math::u16vec2, AccessorType::Vec2, std::uint16_t> {};
template<> struct ElementTraits<math::u16vec3> : ElementTraitsBase<math::u16vec3, AccessorType::Vec3, std::uint16_t> {};
template<> struct ElementTraits<math::u16vec4> : ElementTraitsBase<math::u16vec4, AccessorType::Vec4, std::uint16_t> {};

template<> struct ElementTraits<math::s32vec2> : ElementTraitsBase<math::s32vec2, AccessorType::Vec2, std::int16_t> {};
template<> struct ElementTraits<math::s32vec3> : ElementTraitsBase<math::s32vec3, AccessorType::Vec3, std::int16_t> {};
template<> struct ElementTraits<math::s32vec4> : ElementTraitsBase<math::s32vec4, AccessorType::Vec4, std::int16_t> {};
template<> struct ElementTraits<math::u32vec2> : ElementTraitsBase<math::u32vec2, AccessorType::Vec2, std::uint32_t> {};
template<> struct ElementTraits<math::u32vec3> : ElementTraitsBase<math::u32vec3, AccessorType::Vec3, std::uint32_t> {};
template<> struct ElementTraits<math::u32vec4> : ElementTraitsBase<math::u32vec4, AccessorType::Vec4, std::uint32_t> {};

template<> struct ElementTraits<math::fvec2> : ElementTraitsBase<math::fvec2, AccessorType::Vec2, float> {};
template<> struct ElementTraits<math::fvec3> : ElementTraitsBase<math::fvec3, AccessorType::Vec3, float> {};
template<> struct ElementTraits<math::fvec4> : ElementTraitsBase<math::fvec4, AccessorType::Vec4, float> {};
template<> struct ElementTraits<math::dvec2> : ElementTraitsBase<math::dvec2, AccessorType::Vec2, double> {};
template<> struct ElementTraits<math::dvec3> : ElementTraitsBase<math::dvec3, AccessorType::Vec3, double> {};
template<> struct ElementTraits<math::dvec4> : ElementTraitsBase<math::dvec4, AccessorType::Vec4, double> {};

template<> struct ElementTraits<math::fquat> : ElementTraitsBase<math::fquat, AccessorType::Vec4, float> {};
template<> struct ElementTraits<math::dquat> : ElementTraitsBase<math::dquat, AccessorType::Vec4, double> {};

template<> struct ElementTraits<math::fmat2x2> : ElementTraitsBase<math::fmat2x2, AccessorType::Mat2, float> {};
template<> struct ElementTraits<math::fmat3x3> : ElementTraitsBase<math::fmat3x3, AccessorType::Mat3, float> {};
template<> struct ElementTraits<math::fmat4x4> : ElementTraitsBase<math::fmat4x4, AccessorType::Mat4, float> {};

#if FASTGLTF_HAS_CONCEPTS
template <typename ElementType>
concept Element = std::is_arithmetic_v<typename ElementTraits<ElementType>::component_type>
		&& ElementTraits<ElementType>::type != AccessorType::Invalid
		&& ElementTraits<ElementType>::enum_component_type != ComponentType::Invalid
		&& std::is_default_constructible_v<ElementType>
		&& std::is_constructible_v<ElementType>
		&& std::is_move_assignable_v<ElementType>;
#endif

namespace internal {

/**
 * This function deserializes some N bytes in little endian order (as required by the glTF spec)
 * into the given arithmetic type T in a standard-conforming fashion.
 *
 * This uses bit-shifts and ORs to correctly convert the bytes to avoid violating the strict aliasing
 * rule as if we would just use T*.
 */
template <typename T>
constexpr T deserializeComponent(const std::byte* bytes, std::size_t index) {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>, "Component deserialization is only supported on basic arithmetic types.");
    T ret = 0;
    // Turns out that on some systems a byte is not 8-bit so this sizeof is not technically correct.
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        ret |= (static_cast<T>(bytes[i + index * sizeof(T)]) << i * 8);
    }
    return ret;
}

#if FASTGLTF_HAS_FLOAT32
template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline std::float32_t deserializeComponent<std::float32_t>(const std::byte* bytes, std::size_t index) {
	return bit_cast<std::float32_t>(deserializeComponent<std::uint32_t>(bytes, index));
}

template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline float deserializeComponent<float>(const std::byte* bytes, std::size_t index) {
	return static_cast<float>(deserializeComponent<std::float32_t>(bytes, index));
}
#else
template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline float deserializeComponent<float>(const std::byte* bytes, std::size_t index) {
    static_assert(std::numeric_limits<float>::is_iec559 &&
                  std::numeric_limits<float>::radix == 2 &&
                  std::numeric_limits<float>::digits == 24 &&
                  std::numeric_limits<float>::max_exponent == 128,
                  "Float deserialization is only supported on IEE754 platforms");
    return bit_cast<float>(deserializeComponent<std::uint32_t>(bytes, index));
}
#endif

#if FASTGLTF_HAS_FLOAT64
template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline std::float64_t deserializeComponent<std::float64_t>(const std::byte* bytes, std::size_t index) {
	return bit_cast<std::float64_t>(deserializeComponent<std::uint64_t>(bytes, index));
}

template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline double deserializeComponent<double>(const std::byte* bytes, std::size_t index) {
	return static_cast<double>(deserializeComponent<std::float64_t>(bytes, index));
}
#else
template<>
#if FASTGLTF_CONSTEXPR_BITCAST
constexpr
#endif
inline double deserializeComponent<double>(const std::byte* bytes, std::size_t index) {
    static_assert(std::numeric_limits<double>::is_iec559 &&
                  std::numeric_limits<double>::radix == 2 &&
                  std::numeric_limits<double>::digits == 53 &&
                  std::numeric_limits<double>::max_exponent == 1024,
                  "Float deserialization is only supported on IEE754 platforms");
    return bit_cast<double>(deserializeComponent<std::uint64_t>(bytes, index));
}
#endif

template <typename DestType, typename SourceType>
constexpr DestType convertComponent(const SourceType& source, bool normalized) {
	if (normalized) {
		if constexpr (std::is_floating_point_v<SourceType> && std::is_integral_v<DestType>) {
			// float -> int conversion
			return static_cast<DestType>(std::round(source * static_cast<SourceType>(std::numeric_limits<DestType>::max())));
		} else if constexpr (std::is_integral_v<SourceType> && std::is_floating_point_v<DestType>) {
			// int -> float conversion
			DestType minValue;
			if constexpr (std::is_signed_v<DestType>) {
				minValue = static_cast<DestType>(-1.0);
			} else {
				minValue = static_cast<DestType>(0.0);
			}

			// We have to use max here because for uchar -> float we could have -128 but 1.0 should represent 127,
			// which is why -128 and -127 both equate to 1.0.
			return fastgltf::max(static_cast<DestType>(source) / static_cast<DestType>(std::numeric_limits<SourceType>::max()),
			                     minValue);
		}
	}

	return static_cast<DestType>(source);
}

template <typename DestType, typename SourceType>
constexpr DestType convertComponent(const std::byte* bytes, std::size_t index, AccessorType accessorType, bool normalized) {
	if (isMatrix(accessorType)) {
		const auto rowCount = getElementRowCount(accessorType);
		const auto componentSize = sizeof(SourceType);
		if ((rowCount * componentSize) % 4 != 0) {
			// There's only four cases where this happens, but the glTF spec requires us to insert some padding for each column.
			auto paddedIndex = index + (index / rowCount) * (4 - (rowCount % 4));
			return convertComponent<DestType>(deserializeComponent<SourceType>(bytes, paddedIndex), normalized);
		}

		return convertComponent<DestType>(deserializeComponent<SourceType>(bytes, index), normalized);
	}

	return convertComponent<DestType>(deserializeComponent<SourceType>(bytes, index), normalized);
}

constexpr std::size_t getComponentIndex(std::size_t i, AccessorType type, bool transposed) {
	if (transposed) {
		auto n = getElementRowCount(type);
		return (i % n) * n + (i / n);
	}

	return i;
}

template <typename DestType>
constexpr DestType getAccessorComponentAt(ComponentType componentType, AccessorType type, const std::byte* bytes, std::size_t componentIdx, bool normalized = false, bool transposed = false) {
	auto idx = getComponentIndex(componentIdx, type, transposed);
	switch (componentType) {
		case ComponentType::Byte:
			return internal::convertComponent<DestType, std::int8_t>(bytes, idx, type, normalized);
		case ComponentType::UnsignedByte:
			return internal::convertComponent<DestType, std::uint8_t>(bytes, idx, type, normalized);
		case ComponentType::Short:
			return internal::convertComponent<DestType, std::int16_t>(bytes, idx, type, normalized);
		case ComponentType::UnsignedShort:
			return internal::convertComponent<DestType, std::uint16_t>(bytes, idx, type, normalized);
		case ComponentType::Int:
			return internal::convertComponent<DestType, std::int32_t>(bytes, idx, type, normalized);
		case ComponentType::UnsignedInt:
			return internal::convertComponent<DestType, std::uint32_t>(bytes, idx, type, normalized);
		case ComponentType::Float:
			return internal::convertComponent<DestType, float>(bytes, idx, type, normalized);
		case ComponentType::Double:
			return internal::convertComponent<DestType, double>(bytes, idx, type, normalized);
		case ComponentType::Invalid:
		default:
			return DestType {};
	}
}

template <typename ElementType, typename SourceType, std::size_t... I>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
constexpr ElementType convertAccessorElement(const std::byte* bytes, bool normalized, std::index_sequence<I...>) {
	using Traits = ElementTraits<ElementType>;
	static_assert(std::is_arithmetic_v<typename Traits::component_type>, "Accessor traits must provide a valid component type");

	if constexpr (std::is_aggregate_v<ElementType>) {
		return {convertComponent<typename Traits::component_type, SourceType>(
				bytes, getComponentIndex(I, Traits::type, Traits::needs_transpose), Traits::type, normalized)...};
	} else {
		return ElementType(convertComponent<typename Traits::component_type, SourceType>(
				bytes, getComponentIndex(I, Traits::type, Traits::needs_transpose), Traits::type, normalized)...);
	}
}

template <typename ElementType,
		typename Seq = std::make_index_sequence<getNumComponents(ElementTraits<ElementType>::type)>>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
ElementType getAccessorElementAt(ComponentType componentType, const std::byte* bytes, bool normalized = false) {
	switch (componentType) {
		case ComponentType::Byte:
			return convertAccessorElement<ElementType, std::int8_t>(bytes, normalized, Seq{});
		case ComponentType::UnsignedByte:
			return convertAccessorElement<ElementType, std::uint8_t>(bytes, normalized, Seq{});
		case ComponentType::Short:
			return convertAccessorElement<ElementType, std::int16_t>(bytes, normalized, Seq{});
		case ComponentType::UnsignedShort:
			return convertAccessorElement<ElementType, std::uint16_t>(bytes, normalized, Seq{});
		case ComponentType::Int:
			return convertAccessorElement<ElementType, std::int32_t>(bytes, normalized, Seq{});
		case ComponentType::UnsignedInt:
			return convertAccessorElement<ElementType, std::uint32_t>(bytes, normalized, Seq{});
		case ComponentType::Float:
			return convertAccessorElement<ElementType, float>(bytes, normalized, Seq{});
		case ComponentType::Double:
			return convertAccessorElement<ElementType, double>(bytes, normalized, Seq{});
		case ComponentType::Invalid:
		default:
			return ElementType{};
	}
}

// Performs a binary search for the index into the sparse index list whose value matches the desired index
template <typename ElementType>
bool findSparseIndex(const std::byte* indices, std::size_t indexCount, std::size_t desiredIndex,
		std::size_t& resultIndex) {
	auto count = indexCount;

	resultIndex = 0;

	while (count > 0) {
		auto step = count / 2;
		auto index = resultIndex + step;

		if (deserializeComponent<ElementType>(indices, index) < static_cast<ElementType>(desiredIndex)) {
			resultIndex = index + 1;
			count -= step + 1;
		} else {
			count = step;
		}
	}

	return resultIndex < indexCount && deserializeComponent<ElementType>(indices, resultIndex) == static_cast<ElementType>(desiredIndex);
}

// Finds the index of the nearest sparse index to the desired index
inline bool findSparseIndex(ComponentType componentType, const std::byte* bytes, std::size_t indexCount,
		std::size_t desiredIndex, std::size_t& resultIndex) {
	switch (componentType) {
		case ComponentType::Byte:
			return findSparseIndex<std::int8_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::UnsignedByte:
			return findSparseIndex<std::uint8_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::Short:
			return findSparseIndex<std::int16_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::UnsignedShort:
			return findSparseIndex<std::uint16_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::Int:
			return findSparseIndex<std::int32_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::UnsignedInt:
			return findSparseIndex<std::uint32_t>(bytes, indexCount, desiredIndex, resultIndex);
		case ComponentType::Float:
		case ComponentType::Double:
		case ComponentType::Invalid:
			return false;
	}

	return false;
}

} // namespace internal

FASTGLTF_EXPORT struct DefaultBufferDataAdapter {
	auto operator()(const Asset& asset, const std::size_t bufferViewIdx) const {
		const auto& bufferView = asset.bufferViews[bufferViewIdx];

		const auto data = std::visit(visitor {
			[](auto&) -> span<const std::byte> {
				assert(false && "Tried accessing a buffer with no data, likely because no buffers were loaded. Perhaps you forgot to specify the LoadExternalBuffers option?");
				return {};
			},
			[](const sources::Fallback&) -> span<const std::byte> {
				assert(false && "Tried accessing data of a fallback buffer.");
				return {};
			},
			[&](const sources::Array& array) -> span<const std::byte> {
				return span(array.bytes.data(), array.bytes.size_bytes());
			},
			[&](const sources::Vector& vec) -> span<const std::byte> {
				return span(vec.bytes.data(), vec.bytes.size());
			},
			[&](const sources::ByteView& bv) -> span<const std::byte> {
				return bv.bytes;
			},
		}, asset.buffers[bufferView.bufferIndex].data);

		return data.subspan(bufferView.byteOffset, bufferView.byteLength);
	}
};

template <typename ElementType, typename BufferDataAdapter>
class IterableAccessor;

template <typename ElementType, typename BufferDataAdapter = DefaultBufferDataAdapter>
class AccessorIterator {
protected:
	const IterableAccessor<ElementType, BufferDataAdapter>* accessor = nullptr;
	std::size_t idx = std::numeric_limits<std::size_t>::max();
	mutable std::size_t sparseIdx = 0;
	mutable std::size_t nextSparseIndex = 0;

public:
	using value_type = ElementType;
	using reference = ElementType&;
	using pointer = ElementType*;
	using difference_type = std::ptrdiff_t;

	// This iterator isn't truly random access (as per the C++ definition), but we do want to support
	// some things that these come with (e.g. std::distance using operator-).
	using iterator_category = std::input_iterator_tag;

	AccessorIterator() = default;

	AccessorIterator(const IterableAccessor<ElementType, BufferDataAdapter>* accessor, std::size_t idx = 0)
			: accessor(accessor), idx(idx) {
		if (accessor->accessor.sparse.has_value()) {
			// Get the first sparse index.
			nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(accessor->indexComponentType,
			                                                                &accessor->indicesBytes[accessor->indexStride * sparseIdx]);
		}
	}

	AccessorIterator& operator++() noexcept {
		++idx;
		return *this;
	}

	AccessorIterator operator++(int) noexcept {
		auto x = *this;
		++(*this);
		return x;
	}

	[[nodiscard]] difference_type operator-(const AccessorIterator& other) const noexcept {
		return static_cast<difference_type>(idx - other.idx);
	}

	[[nodiscard]] bool operator==(const AccessorIterator& iterator) const noexcept {
		// We don't compare sparse properties
		return accessor == iterator.accessor &&
			idx == iterator.idx &&
			accessor->bufferBytes.data() == iterator.accessor->bufferBytes.data() &&
			accessor->stride == iterator.accessor->stride &&
			accessor->componentType == iterator.accessor->componentType;
	}

	[[nodiscard]] bool operator!=(const AccessorIterator& iterator) const noexcept {
		return !(*this == iterator);
	}

	[[nodiscard]] ElementType operator*() const noexcept {
		if (accessor->accessor.sparse.has_value()) {
			if (idx == nextSparseIndex) {
				// Get the sparse value for this index
				auto value = internal::getAccessorElementAt<ElementType>(accessor->componentType,
																		 &accessor->valuesBytes[accessor->valueStride * sparseIdx],
																		 accessor->accessor.normalized);

				// Find the next sparse index.
				++sparseIdx;
				if (sparseIdx < accessor->sparseCount) {
					nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(accessor->indexComponentType,
					                                                                &accessor->indicesBytes[accessor->indexStride * sparseIdx]);
				}
				return value;
			}
		}

		return internal::getAccessorElementAt<ElementType>(accessor->componentType,
														   &accessor->bufferBytes[idx * accessor->stride],
														   accessor->accessor.normalized);
	}
};

#if FASTGLTF_HAS_CONCEPTS
static_assert(std::input_iterator<AccessorIterator<math::fvec4>>, "AccessorIterator needs to satisfy input_iterator");
#endif

template <typename ElementType, typename BufferDataAdapter = DefaultBufferDataAdapter>
class IterableAccessor {
	friend class AccessorIterator<ElementType, BufferDataAdapter>;

	const Asset& asset;
	const Accessor& accessor;

	span<const std::byte> bufferBytes;
	std::size_t stride;
	fastgltf::ComponentType componentType;

	// Data needed for sparse accessors
	fastgltf::ComponentType indexComponentType;
	span<const std::byte> indicesBytes;
	span<const std::byte> valuesBytes;
	std::size_t indexStride;
	std::size_t valueStride;
	std::size_t sparseCount;

public:
	using iterator = AccessorIterator<ElementType, BufferDataAdapter>;

	explicit IterableAccessor(const Asset& asset, const Accessor& accessor, const BufferDataAdapter& adapter) : asset(asset), accessor(accessor) {
		assert(accessor.type == ElementTraits<ElementType>::type && "The destination type needs to have the same AccessorType as the accessor.");
		componentType = accessor.componentType;

		const auto& view = asset.bufferViews[*accessor.bufferViewIndex];
		stride = view.byteStride ? *view.byteStride : getElementByteSize(accessor.type, accessor.componentType);

		bufferBytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);

		if (accessor.sparse.has_value()) {
			indicesBytes = adapter(asset, accessor.sparse->indicesBufferView).subspan(accessor.sparse->indicesByteOffset);

			indexStride = getElementByteSize(AccessorType::Scalar, accessor.sparse->indexComponentType);

			valuesBytes = adapter(asset, accessor.sparse->valuesBufferView).subspan(accessor.sparse->valuesByteOffset);

			// "The index of the bufferView with sparse values. The referenced buffer view MUST NOT
			// have its target or byteStride properties defined."
			valueStride = getElementByteSize(accessor.type, accessor.componentType);

			indexComponentType = accessor.sparse->indexComponentType;
			sparseCount = accessor.sparse->count;
		}
	}

	[[nodiscard]] iterator begin() const noexcept {
		return iterator(this, 0);
	}

	[[nodiscard]] iterator end() const noexcept {
		return iterator(this, accessor.count);
	}
};

#if FASTGLTF_HAS_CONCEPTS
static_assert(std::ranges::input_range<IterableAccessor<math::fvec4>>, "IterableAccessor needs to satisfy input_range");
#endif

FASTGLTF_EXPORT template <typename ElementType, typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
ElementType getAccessorElement(const Asset& asset, const Accessor& accessor, size_t index,
		const BufferDataAdapter& adapter = {}) {
	using Traits = ElementTraits<ElementType>;
	static_assert(Traits::type != AccessorType::Invalid, "Accessor traits must provide a valid Accessor Type");
	static_assert(std::is_default_constructible_v<ElementType>, "Element type must be default constructible");
	static_assert(std::is_constructible_v<ElementType>, "Element type must be constructible");
	static_assert(std::is_move_assignable_v<ElementType>, "Element type must be move-assignable");

	assert(accessor.type == Traits::type && "The destination type needs to have the same AccessorType as the accessor.");

	if (accessor.sparse) {
		auto indicesBytes = adapter(asset, accessor.sparse->indicesBufferView).subspan(accessor.sparse->indicesByteOffset);

		auto valuesBytes = adapter(asset, accessor.sparse->valuesBufferView).subspan(accessor.sparse->valuesByteOffset);
		// "The index of the bufferView with sparse values. The referenced buffer view MUST NOT
		// have its target or byteStride properties defined."
		auto valueStride = getElementByteSize(accessor.type, accessor.componentType);

		std::size_t sparseIndex{};
		if (internal::findSparseIndex(accessor.sparse->indexComponentType, indicesBytes.data(), accessor.sparse->count,
				index, sparseIndex)) {
			return internal::getAccessorElementAt<ElementType>(accessor.componentType,
					&valuesBytes[valueStride * sparseIndex],
					accessor.normalized);
		}
	}

	// 5.1.1. accessor.bufferView
	// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
	// property or extensions MAY override zeros with actual values.
	if (!accessor.bufferViewIndex) {
		if constexpr (std::is_aggregate_v<ElementType>) {
			return ElementType{};
		} else {
			return ElementType();
		}
	}

	const auto& view = asset.bufferViews[*accessor.bufferViewIndex];
    auto stride = view.byteStride.value_or(getElementByteSize(accessor.type, accessor.componentType));

	auto bytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);

	return internal::getAccessorElementAt<ElementType>(
            accessor.componentType, &bytes[index * stride], accessor.normalized);
}

FASTGLTF_EXPORT template<typename ElementType, typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
IterableAccessor<ElementType, BufferDataAdapter> iterateAccessor(const Asset& asset, const Accessor& accessor, const BufferDataAdapter& adapter = {}) {
	return IterableAccessor<ElementType, BufferDataAdapter>(asset, accessor, adapter);
}

FASTGLTF_EXPORT template <typename ElementType, typename Functor, typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType> && std::is_invocable_v<Functor, ElementType>
#endif
void iterateAccessor(const Asset& asset, const Accessor& accessor, Functor&& func,
		const BufferDataAdapter& adapter = {}) {
	using Traits = ElementTraits<ElementType>;
	static_assert(Traits::type != AccessorType::Invalid, "Accessor traits must provide a valid accessor type");
	static_assert(Traits::enum_component_type != ComponentType::Invalid, "Accessor traits must provide a valid component type");
	static_assert(std::is_default_constructible_v<ElementType>, "Element type must be default constructible");
	static_assert(std::is_constructible_v<ElementType>, "Element type must be constructible");
	static_assert(std::is_move_assignable_v<ElementType>, "Element type must be move-assignable");

	assert(accessor.type == Traits::type && "The destination type needs to have the same AccessorType as the accessor.");

	if (accessor.sparse && accessor.sparse->count > 0) {
		auto indicesBytes = adapter(asset, accessor.sparse->indicesBufferView).subspan(accessor.sparse->indicesByteOffset);
		auto indexStride = getElementByteSize(AccessorType::Scalar, accessor.sparse->indexComponentType);

		auto valuesBytes = adapter(asset, accessor.sparse->valuesBufferView).subspan(accessor.sparse->valuesByteOffset);
		// "The index of the bufferView with sparse values. The referenced buffer view MUST NOT
		// have its target or byteStride properties defined."
		auto valueStride = getElementByteSize(accessor.type, accessor.componentType);

		span<const std::byte> srcBytes;
		std::size_t srcStride = 0;

		// 5.1.1. accessor.bufferView
		// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
		// property or extensions MAY override zeros with actual values.
		if (accessor.bufferViewIndex) {
			auto& view = asset.bufferViews[*accessor.bufferViewIndex];
			srcBytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);
            srcStride = view.byteStride.value_or(getElementByteSize(accessor.type, accessor.componentType));
        }

		auto nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(
				accessor.sparse->indexComponentType, indicesBytes.data());
		std::size_t sparseIndexCount = 0;

		for (std::size_t i = 0; i < accessor.count; ++i) {
			if (i == nextSparseIndex) {
				auto element = internal::getAccessorElementAt<ElementType>(accessor.componentType,
						&valuesBytes[valueStride * sparseIndexCount],
						accessor.normalized);

				std::invoke(func, element);

				++sparseIndexCount;

				if (sparseIndexCount < accessor.sparse->count) {
					nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(
							accessor.sparse->indexComponentType, &indicesBytes[indexStride * sparseIndexCount]);
				}
			} else if (accessor.bufferViewIndex) {
				auto element = internal::getAccessorElementAt<ElementType>(accessor.componentType,
						&srcBytes[srcStride * i],
						accessor.normalized);

				std::invoke(func, element);
			} else {
				std::invoke(func, ElementType {});
			}
		}

		return;
	}

	// 5.1.1. accessor.bufferView
	// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
	// property or extensions MAY override zeros with actual values.
	if (!accessor.bufferViewIndex) {
		for (std::size_t i = 0; i < accessor.count; ++i) {
			std::invoke(func, ElementType {});
		}
	} else {
		auto& view = asset.bufferViews[*accessor.bufferViewIndex];
        auto stride = view.byteStride.value_or(getElementByteSize(accessor.type, accessor.componentType));

		auto bytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);

		for (std::size_t i = 0; i < accessor.count; ++i) {
			auto element = internal::getAccessorElementAt<ElementType>(
				accessor.componentType, &bytes[i * stride], accessor.normalized);
			std::invoke(func, element);
		}
	}
}

FASTGLTF_EXPORT template <typename ElementType, typename Functor, typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType> && std::is_invocable_v<Functor, ElementType, std::size_t>
#endif
void iterateAccessorWithIndex(const Asset& asset, const Accessor& accessor, Functor&& func,
                     const BufferDataAdapter& adapter = {}) {
	std::size_t idx = 0;
	iterateAccessor<ElementType>(asset, accessor, [&](auto&& elementType) {
		std::invoke(func, std::forward<ElementType>(elementType), idx++);
	}, adapter);
}

FASTGLTF_EXPORT template <typename ElementType, std::size_t TargetStride = sizeof(ElementType),
    typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
void copyFromAccessor(const Asset& asset, const Accessor& accessor, void* dest,
		const BufferDataAdapter& adapter = {}) {
	using Traits = ElementTraits<ElementType>;
	static_assert(Traits::type != AccessorType::Invalid, "Accessor traits must provide a valid accessor type");
	static_assert(Traits::enum_component_type != ComponentType::Invalid, "Accessor traits must provide a valid component type");
	static_assert(std::is_default_constructible_v<ElementType>, "Element type must be default constructible");
	static_assert(std::is_constructible_v<ElementType>, "Element type must be constructible");
	static_assert(std::is_move_assignable_v<ElementType>, "Element type must be move-assignable");

	assert(accessor.type == Traits::type && "The destination type needs to have the same AccessorType as the accessor.");

	auto* dstBytes = static_cast<std::byte*>(dest);

	if (accessor.sparse && accessor.sparse->count > 0) {
		return iterateAccessorWithIndex<ElementType>(asset, accessor, [&](auto&& value, std::size_t index) {
			auto* pDest = reinterpret_cast<ElementType*>(dstBytes + TargetStride * index);
			*pDest = std::forward<ElementType>(value);
		}, adapter);
	}

	auto elemSize = getElementByteSize(accessor.type, accessor.componentType);

	// 5.1.1. accessor.bufferView
	// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
	// property or extensions MAY override zeros with actual values.
	if (!accessor.bufferViewIndex) {
		if constexpr (std::is_trivially_copyable_v<ElementType>) {
			if (TargetStride == elemSize) {
				std::memset(dest, 0, elemSize * accessor.count);
			} else {
				for (std::size_t i = 0; i < accessor.count; ++i) {
					std::memset(dstBytes + i * TargetStride, 0, elemSize);
				}
			}
		} else {
			for (std::size_t i = 0; i < accessor.count; ++i) {
				auto* pDest = reinterpret_cast<ElementType*>(dstBytes + TargetStride * i);

				if constexpr (std::is_aggregate_v<ElementType>) {
					*pDest = ElementType {};
				} else {
					*pDest = ElementType();
				}
			}
		}

		return;
	}

	auto& view = asset.bufferViews[*accessor.bufferViewIndex];
	auto srcStride = view.byteStride.value_or(elemSize);

	auto srcBytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);

    // If the data is normalized or the component/accessor type is different, we have to convert each element and can't memcpy.
	if (std::is_trivially_copyable_v<ElementType> && !accessor.normalized && accessor.componentType == Traits::enum_component_type && !isMatrix(accessor.type)) {
		if (srcStride == elemSize && srcStride == TargetStride) {
			std::memcpy(dest, srcBytes.data(), elemSize * accessor.count);
		} else {
			for (std::size_t i = 0; i < accessor.count; ++i) {
				std::memcpy(dstBytes + TargetStride * i, &srcBytes[srcStride * i], elemSize);
			}
		}
	} else {
		for (std::size_t i = 0; i < accessor.count; ++i) {
			auto* pDest = reinterpret_cast<ElementType*>(dstBytes + TargetStride * i);
			*pDest = internal::getAccessorElementAt<ElementType>(
                    accessor.componentType, &srcBytes[srcStride * i]);
		}
	}
}

/**
 * This function allows copying each component into a linear list, instead of copying per-element,
 * while still performing the correct conversions for the destination type.
 * It is advised to *not* use this function unless necessary, like for example when implementing
 * a generic animation interface.
 */
FASTGLTF_EXPORT template <typename ComponentType, typename BufferDataAdapter = DefaultBufferDataAdapter>
void copyComponentsFromAccessor(const Asset& asset, const Accessor& accessor, void* dest, const BufferDataAdapter& adapter = {}) {
	constexpr auto DestType = ComponentTypeConverter<ComponentType>::type;

	assert((!bool(accessor.sparse) || accessor.sparse->count == 0) && "copyComponentsFromAccessor currently does not support sparse accessors.");

	auto* dstBytes = static_cast<std::byte*>(dest);

	auto elemSize = getElementByteSize(accessor.type, accessor.componentType);
	auto componentCount = getNumComponents(accessor.type);

	auto& view = asset.bufferViews[*accessor.bufferViewIndex];
	auto srcStride = view.byteStride.value_or(elemSize);

	auto srcBytes = adapter(asset, *accessor.bufferViewIndex).subspan(accessor.byteOffset);

	if (accessor.componentType == DestType && !accessor.normalized && !isMatrix(accessor.type)) {
		if (srcStride == elemSize) {
			std::memcpy(dest, srcBytes.data(), elemSize * accessor.count);
		} else {
			for (std::size_t i = 0; i < accessor.count; ++i) {
				std::memcpy(dstBytes + elemSize * i, &srcBytes[srcStride * i], elemSize);
			}
		}
	} else {
		for (std::size_t i = 0; i < accessor.count; ++i) {
			for (std::size_t j = 0; j < componentCount; ++j) {
				auto* pDest = reinterpret_cast<ComponentType*>(dstBytes + elemSize * i) + j;
				*pDest = internal::getAccessorComponentAt<ComponentType>(
					accessor.componentType, accessor.type, &srcBytes[i * srcStride], j, accessor.normalized);
			}
		}
	}
}

/**
 * Returns the local transform matrix for a given node, computing it from TRS components if necessary
 */
FASTGLTF_EXPORT inline auto getLocalTransformMatrix(const Node& node) {
	return visit_exhaustive(visitor {
		[&](const math::fmat4x4& matrix) {
			return matrix;
		},
		[&](const TRS& trs) {
			return scale(rotate(translate(math::fmat4x4(), trs.translation), trs.rotation), trs.scale);
		}
	}, node.transform);
}

/**
 * Computes the transform matrix for a given node, and multiplies the given base with that matrix.
 */
FASTGLTF_EXPORT inline auto getTransformMatrix(const Node& node, const math::fmat4x4& base = math::fmat4x4()) {
	return base * getLocalTransformMatrix(node);
}

/**
 * Iterates over every node within a scene recursively, computing the world space transform of each node,
 * and calling the callback function with that node and the transform.
 */
FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
requires std::same_as<std::remove_cvref_t<AssetType>, Asset>
      && std::is_invocable_v<Callback, fastgltf::Node&, const fastgltf::math::fmat4x4&>
#endif
void iterateSceneNodes(AssetType&& asset, std::size_t sceneIndex, math::fmat4x4 initial, Callback callback) {
	auto& scene = asset.scenes[sceneIndex];

	auto function = [&](std::size_t nodeIndex, math::fmat4x4 nodeMatrix, auto& self) -> void {
		assert(asset.nodes.size() > nodeIndex);
		auto& node = asset.nodes[nodeIndex];
		nodeMatrix = getTransformMatrix(node, nodeMatrix);

		std::invoke(callback, node, std::as_const(nodeMatrix));

		for (auto& child : node.children) {
			self(child, nodeMatrix, self);
		}
	};

	for (auto& sceneNode : scene.nodeIndices) {
		function(sceneNode, initial, function);
	}
}

} // namespace fastgltf

#endif
