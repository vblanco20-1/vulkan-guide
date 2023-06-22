/*
 * Copyright (C) 2022 - 2023 spnda
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

#pragma once

#include <cstring>

#include "types.hpp"

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

template <typename ElementType, AccessorType EnumAccessorType, typename ComponentType = ElementType>
struct ElementTraitsBase {
	using element_type = ElementType;
	using component_type = ComponentType;
	static constexpr auto type = EnumAccessorType;
	static constexpr auto enum_component_type = ComponentTypeConverter<ComponentType>::type;
};

template <typename>
struct ElementTraits;

template<>
struct ElementTraits<std::int8_t> : ElementTraitsBase<std::int8_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<std::uint8_t> : ElementTraitsBase<std::uint8_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<std::int16_t> : ElementTraitsBase<std::int16_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<std::uint16_t> : ElementTraitsBase<std::uint16_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<std::int32_t> : ElementTraitsBase<std::int32_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<std::uint32_t> : ElementTraitsBase<std::uint32_t, AccessorType::Scalar> {};

template<>
struct ElementTraits<float> : ElementTraitsBase<float, AccessorType::Scalar> {};

template<>
struct ElementTraits<double> : ElementTraitsBase<double, AccessorType::Scalar> {};

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

template <typename SourceType, typename DestType, std::size_t Index>
constexpr DestType convertComponent(const std::byte* bytes) {
	return static_cast<DestType>(reinterpret_cast<const SourceType*>(bytes)[Index]);
}

template <typename ElementType, typename SourceType, std::size_t... I>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
constexpr ElementType convertAccessorElement(const std::byte* bytes, std::index_sequence<I...>) {
	using DestType = typename ElementTraits<ElementType>::component_type;
	static_assert(std::is_arithmetic_v<DestType>, "Accessor traits must provide a valid component type");

	if constexpr (std::is_aggregate_v<ElementType>) {
		return {convertComponent<SourceType, DestType, I>(bytes)...};
	} else {
		return ElementType{convertComponent<SourceType, DestType, I>(bytes)...};
	}
}

template <typename ElementType,
		typename Seq = std::make_index_sequence<getNumComponents(ElementTraits<ElementType>::type)>>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
ElementType getAccessorElementAt(ComponentType componentType, const std::byte* bytes) {
	switch (componentType) {
		case ComponentType::Byte:
			return convertAccessorElement<ElementType, std::int8_t>(bytes, Seq{});
		case ComponentType::UnsignedByte:
			return convertAccessorElement<ElementType, std::uint8_t>(bytes, Seq{});
		case ComponentType::Short:
			return convertAccessorElement<ElementType, std::int16_t>(bytes, Seq{});
		case ComponentType::UnsignedShort:
			return convertAccessorElement<ElementType, std::uint16_t>(bytes, Seq{});
		case ComponentType::Int:
			return convertAccessorElement<ElementType, std::int32_t>(bytes, Seq{});
		case ComponentType::UnsignedInt:
			return convertAccessorElement<ElementType, std::uint32_t>(bytes, Seq{});
		case ComponentType::Float:
			return convertAccessorElement<ElementType, float>(bytes, Seq{});
		case ComponentType::Double:
			return convertAccessorElement<ElementType, double>(bytes, Seq{});
		case ComponentType::Invalid:
			return ElementType{};
	}

	return ElementType{};
}

// Performs a binary search for the index into the sparse index list whose value matches the desired index
template <typename ElementType>
bool findSparseIndex(const std::byte* bytes, std::size_t indexCount, std::size_t desiredIndex,
		std::size_t& resultIndex) {
	auto* elements = reinterpret_cast<const ElementType*>(bytes);
	auto* last = &reinterpret_cast<const ElementType*>(bytes)[indexCount];
	auto count = static_cast<std::ptrdiff_t>(indexCount);

	resultIndex = 0;

	while (count > 0) {
		auto step = count / 2;
		auto index = resultIndex + step;

		if (elements[index] < static_cast<ElementType>(desiredIndex)) {
			resultIndex = index + 1;
			count -= step + 1;
		} else {
			count = step;
		}
	}

	return resultIndex < indexCount && elements[resultIndex] == static_cast<ElementType>(desiredIndex);
}

// Finds the index of the nearest sparse index to the desired index
bool findSparseIndex(ComponentType componentType, const std::byte* bytes, std::size_t indexCount,
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

struct DefaultBufferDataAdapter {
	const std::byte* operator()(const Buffer& buffer) const {
		const std::byte* result = nullptr;

		std::visit(visitor {
			[](auto&) {},
			[&](const sources::Vector& vec) {
				result = reinterpret_cast<const std::byte*>(vec.bytes.data());
			},
			[&](const sources::ByteView& bv) {
				result = bv.bytes.data();
			},
		}, buffer.data);

		return result;
	}
};

template <typename ElementType, typename BufferDataAdapter = DefaultBufferDataAdapter>
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

	if (accessor.sparse) {
		auto& indicesView = asset.bufferViews[accessor.sparse->indicesBufferView];
		auto* indicesBytes = adapter(asset.buffers[indicesView.bufferIndex])
				+ indicesView.byteOffset + accessor.sparse->indicesByteOffset;

		auto& valuesView = asset.bufferViews[accessor.sparse->valuesBufferView];
		auto* valuesBytes = adapter(asset.buffers[valuesView.bufferIndex])
				+ valuesView.byteOffset + accessor.sparse->valuesByteOffset;
		// "The index of the bufferView with sparse values. The referenced buffer view MUST NOT
		// have its target or byteStride properties defined."
		auto valueStride = getElementByteSize(accessor.type, accessor.componentType);

		std::size_t sparseIndex{};
		if (internal::findSparseIndex(accessor.sparse->indexComponentType, indicesBytes, accessor.sparse->count,
				index, sparseIndex)) {
			return internal::getAccessorElementAt<ElementType>(accessor.componentType,
					valuesBytes + valueStride * sparseIndex);
		}

		// 5.1.1. accessor.bufferView
		// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
		// property or extensions MAY override zeros with actual values.
		if (!accessor.bufferViewIndex) {
			if constexpr (std::is_aggregate_v<ElementType>) {
				return {};
			} else {
				return ElementType{};
			}
		}

		auto& view = asset.bufferViews[*accessor.bufferViewIndex];
		auto stride = view.byteStride ? *view.byteStride : getElementByteSize(accessor.type, accessor.componentType);

		auto* bytes = adapter(asset.buffers[view.bufferIndex]);
		bytes += view.byteOffset + accessor.byteOffset;

		return internal::getAccessorElementAt<ElementType>(accessor.componentType, bytes + index * stride);
	} 

	// 5.1.1. accessor.bufferView
	// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
	// property or extensions MAY override zeros with actual values.
	if (!accessor.bufferViewIndex) {
		if constexpr (std::is_aggregate_v<ElementType>) {
			return {};
		} else {
			return ElementType{};
		}
	}

	auto& view = asset.bufferViews[*accessor.bufferViewIndex];
	auto stride = view.byteStride ? *view.byteStride : getElementByteSize(accessor.type, accessor.componentType);

	auto* bytes = adapter(asset.buffers[view.bufferIndex]);
	bytes += view.byteOffset + accessor.byteOffset;

	return internal::getAccessorElementAt<ElementType>(accessor.componentType, bytes + index * stride);
}

template <typename ElementType, typename Functor, typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
void iterateAccessor(const Asset& asset, const Accessor& accessor, Functor&& func,
		const BufferDataAdapter& adapter = {}) {
	using Traits = ElementTraits<ElementType>;
	static_assert(Traits::type != AccessorType::Invalid, "Accessor traits must provide a valid Accessor Type");
	static_assert(std::is_default_constructible_v<ElementType>, "Element type must be default constructible");
	static_assert(std::is_constructible_v<ElementType>, "Element type must be constructible");
	static_assert(std::is_move_assignable_v<ElementType>, "Element type must be move-assignable");

	if (accessor.type != Traits::type) {
		return;
	}

	if (accessor.sparse && accessor.sparse->count > 0) {
		auto& indicesView = asset.bufferViews[accessor.sparse->indicesBufferView];
		auto* indicesBytes = adapter(asset.buffers[indicesView.bufferIndex])
				+ indicesView.byteOffset + accessor.sparse->indicesByteOffset;
		auto indexStride = getElementByteSize(AccessorType::Scalar, accessor.sparse->indexComponentType);

		auto& valuesView = asset.bufferViews[accessor.sparse->valuesBufferView];
		auto* valuesBytes = adapter(asset.buffers[valuesView.bufferIndex])
				+ valuesView.byteOffset + accessor.sparse->valuesByteOffset;
		// "The index of the bufferView with sparse values. The referenced buffer view MUST NOT
		// have its target or byteStride properties defined."
		auto valueStride = getElementByteSize(accessor.type, accessor.componentType);

		const std::byte* srcBytes = nullptr;
		std::size_t srcStride = 0;

		// 5.1.1. accessor.bufferView
		// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
		// property or extensions MAY override zeros with actual values.
		if (accessor.bufferViewIndex) {
			auto& view = asset.bufferViews[*accessor.bufferViewIndex];
			srcBytes = adapter(asset.buffers[view.bufferIndex]) + view.byteOffset + accessor.byteOffset;
			srcStride = view.byteStride ? *view.byteStride
					: getElementByteSize(accessor.type, accessor.componentType);
		}

		auto nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(
				accessor.sparse->indexComponentType, indicesBytes);
		std::size_t sparseIndexCount = 0;

		for (std::size_t i = 0; i < accessor.count; ++i) {
			if (i == nextSparseIndex) {
				func(internal::getAccessorElementAt<ElementType>(accessor.componentType,
						valuesBytes + valueStride * sparseIndexCount));

				++sparseIndexCount;

				if (sparseIndexCount < accessor.sparse->count) {
					nextSparseIndex = internal::getAccessorElementAt<std::uint32_t>(
							accessor.sparse->indexComponentType, indicesBytes + indexStride * sparseIndexCount);
				}
			} else if (accessor.bufferViewIndex) {
				func(internal::getAccessorElementAt<ElementType>(accessor.componentType,
						srcBytes + srcStride * i));
			} else {
				func(ElementType{});
			}
		}

		return;
	}

	// 5.1.1. accessor.bufferView
	// The index of the buffer view. When undefined, the accessor MUST be initialized with zeros; sparse
	// property or extensions MAY override zeros with actual values.
	if (!accessor.bufferViewIndex) {
		for (std::size_t i = 0; i < accessor.count; ++i) {
			func(ElementType{});
		}
	}
	else {
		auto& view = asset.bufferViews[*accessor.bufferViewIndex];
		auto stride = view.byteStride ? *view.byteStride : getElementByteSize(accessor.type, accessor.componentType);

		auto* bytes = adapter(asset.buffers[view.bufferIndex]);
		bytes += view.byteOffset + accessor.byteOffset;

		for (std::size_t i = 0; i < accessor.count; ++i) {
			func(internal::getAccessorElementAt<ElementType>(accessor.componentType, bytes + i * stride));
		}
	}
}

template <typename ElementType, std::size_t TargetStride = sizeof(ElementType),
		 typename BufferDataAdapter = DefaultBufferDataAdapter>
#if FASTGLTF_HAS_CONCEPTS
requires Element<ElementType>
#endif
void copyFromAccessor(const Asset& asset, const Accessor& accessor, void* dest,
		const BufferDataAdapter& adapter = {}) {
	using Traits = ElementTraits<ElementType>;
	static_assert(Traits::type != AccessorType::Invalid, "Accessor traits must provide a valid Accessor Type");
	static_assert(std::is_default_constructible_v<ElementType>, "Element type must be default constructible");
	static_assert(std::is_constructible_v<ElementType>, "Element type must be constructible");
	static_assert(std::is_move_assignable_v<ElementType>, "Element type must be move-assignable");

	if (accessor.type != Traits::type) {
		return;
	}

	auto* dstBytes = reinterpret_cast<std::byte*>(dest);

	if (accessor.sparse && accessor.sparse->count > 0) {
		std::size_t index = 0;
		iterateAccessor<ElementType>(asset, accessor, [&](auto&& value) {
			auto* pDest = reinterpret_cast<ElementType*>(dstBytes + TargetStride * index);
			*pDest = std::forward<ElementType>(value);
			++index;
		}, adapter);

		return;
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
					*pDest = {};
				} else {
					*pDest = ElementType{};
				}
			}
		}

		return;
	}

	auto& view = asset.bufferViews[*accessor.bufferViewIndex];
	auto srcStride = view.byteStride ? *view.byteStride
			: getElementByteSize(accessor.type, accessor.componentType);

	auto* srcBytes = adapter(asset.buffers[view.bufferIndex]) + view.byteOffset + accessor.byteOffset;

	if constexpr (std::is_trivially_copyable_v<ElementType>) {
		if (srcStride == elemSize && srcStride == TargetStride) {
			std::memcpy(dest, srcBytes, elemSize * accessor.count);
		} else {
			for (std::size_t i = 0; i < accessor.count; ++i) {
				std::memcpy(dstBytes + TargetStride * i, srcBytes + srcStride * i, elemSize);
			}
		}
	} else {
		for (std::size_t i = 0; i < accessor.count; ++i) {
			auto* pDest = reinterpret_cast<ElementType*>(dstBytes + TargetStride * i);
			*pDest = internal::getAccessorElementAt<ElementType>(accessor.componentType, srcBytes + srcStride * i);
		}
	}
}

} // namespace fastgltf
