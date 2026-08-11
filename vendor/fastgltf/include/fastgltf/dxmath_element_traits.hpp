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

#ifndef FASTGLTF_DXMATH_TRAITS_HPP
#define FASTGLTF_DXMATH_TRAITS_HPP

#include <fastgltf/tools.hpp>

#if __has_include(<DirectXMath.h>)
#include <DirectXMath.h>
#endif

#if __has_include(<DirectXPackedVector.h>)
#include <DirectXPackedVector.h>
#endif

namespace fastgltf {

FASTGLTF_EXPORT template <typename ElementType, AccessorType EnumAccessorType, typename ComponentType = ElementType>
using TransposedElementTraits = ElementTraitsBase<ElementType, EnumAccessorType, ComponentType, true>;

template <>
struct ElementTraits<DirectX::XMFLOAT2> : ElementTraitsBase<DirectX::XMFLOAT2, AccessorType::Vec2, float> {};

template <>
struct ElementTraits<DirectX::XMFLOAT3> : ElementTraitsBase<DirectX::XMFLOAT3, AccessorType::Vec3, float> {};

template <>
struct ElementTraits<DirectX::XMFLOAT4> : ElementTraitsBase<DirectX::XMFLOAT4, AccessorType::Vec4, float> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMBYTE2> : ElementTraitsBase<DirectX::PackedVector::XMBYTE2, AccessorType::Vec2, std::int8_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMBYTE4> : ElementTraitsBase<DirectX::PackedVector::XMBYTE4, AccessorType::Vec4, std::int8_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMUBYTE2> : ElementTraitsBase<DirectX::PackedVector::XMUBYTE2, AccessorType::Vec2, std::uint8_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMUBYTE4> : ElementTraitsBase<DirectX::PackedVector::XMUBYTE4, AccessorType::Vec4, std::uint8_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMSHORT2> : ElementTraitsBase<DirectX::PackedVector::XMSHORT2, AccessorType::Vec2, std::int16_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMSHORT4> : ElementTraitsBase<DirectX::PackedVector::XMSHORT4, AccessorType::Vec4, std::int16_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMUSHORT2> : ElementTraitsBase<DirectX::PackedVector::XMUSHORT2, AccessorType::Vec2, std::uint16_t> {};

template<>
struct ElementTraits<DirectX::PackedVector::XMUSHORT4> : ElementTraitsBase<DirectX::PackedVector::XMUSHORT4, AccessorType::Vec4, std::uint16_t> {};

template<>
struct ElementTraits<DirectX::XMUINT2> : ElementTraitsBase<DirectX::XMUINT2, AccessorType::Vec2, std::uint32_t> {};

template<>
struct ElementTraits<DirectX::XMUINT3> : ElementTraitsBase<DirectX::XMUINT3, AccessorType::Vec3, std::uint32_t> {};

template<>
struct ElementTraits<DirectX::XMUINT4> : ElementTraitsBase<DirectX::XMUINT4, AccessorType::Vec4, std::uint32_t> {};

template<>
struct ElementTraits<DirectX::XMFLOAT3X3> : TransposedElementTraits<DirectX::XMFLOAT3X3, AccessorType::Mat3, float> {};

template<>
struct ElementTraits<DirectX::XMFLOAT4X4> : TransposedElementTraits<DirectX::XMFLOAT4X4, AccessorType::Mat4, float> {};

} // namespace fastgltf

#endif
