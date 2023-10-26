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

#include <cassert>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <memory_resource>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

// Utils header already includes some headers, which we'll try and avoid including twice.
#include "util.hpp"

#if FASTGLTF_CPP_20
#include <span>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

#define FASTGLTF_QUOTE_Q(x) #x
#define FASTGLTF_QUOTE(x) FASTGLTF_QUOTE_Q(x)

// fastgltf version string. Use FASTGLTF_QUOTE to stringify.
#define FASTGLTF_VERSION 0.5.0

namespace fastgltf {
#pragma region Enums
    // clang-format off
    enum class PrimitiveType : std::uint8_t {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6,
    };

    /**
     * Represents the type of element in the buffer pointed to by the accessor.
     *
     * We encode these values with the number of components in their top 8 bits for fast
     * access & storage. Therefore, use the fastgltf::getNumComponents and fastgltf::getElementByteSize
     * functions to extract data from this enum.
     */
    enum class AccessorType : std::uint16_t {
        Invalid = 0,
        Scalar  = ( 1 << 8) | 1,
        Vec2    = ( 2 << 8) | 2,
        Vec3    = ( 3 << 8) | 3,
        Vec4    = ( 4 << 8) | 4,
        Mat2    = ( 4 << 8) | 5,
        Mat3    = ( 9 << 8) | 6,
        Mat4    = (16 << 8) | 7,
    };

    /**
     * Represents the various types of components an accessor could point at. This describes the
     * format each component of the structure, which in return is described by fastgltf::AccessorType, is in.
     *
     * We use the top 16-bits to encode the amount of bits this component type needs.
     * The lower 16-bits are used to store the glTF ID for the type. Therefore, use the fastgltf::getComponentBitSize
     * and fastgltf::getGLComponentType functions should be used to extract data from this enum.
     */
    enum class ComponentType : std::uint32_t {
        Invalid         = 0,
        Byte            = ( 8 << 16) | 5120,
        UnsignedByte    = ( 8 << 16) | 5121,
        Short           = (16 << 16) | 5122,
        UnsignedShort   = (16 << 16) | 5123,
        /**
         * Signed integers are not officially allowed by the glTF spec, but are placed here for
         * the sake of completeness.
         */
        Int             = (32 << 16) | 5124,
        UnsignedInt     = (32 << 16) | 5125,
        Float           = (32 << 16) | 5126,
        /**
         * Doubles are not officially allowed by the glTF spec, but can be enabled by passing
         * Options::AllowDouble if you require it.
         */
        Double          = (64 << 16) | 5130,
    };

    enum class Filter : std::uint16_t {
        Nearest = 9728,
        Linear = 9729,
        NearestMipMapNearest = 9984,
        LinearMipMapNearest = 9985,
        NearestMipMapLinear = 9986,
        LinearMipMapLinear = 9987,
    };

    enum class Wrap : std::uint16_t {
        ClampToEdge = 33071,
        MirroredRepeat = 33648,
        Repeat = 10497,
    };

    /**
     * Represents the intended OpenGL GPU buffer type to use with this buffer view.
     */
    enum class BufferTarget : std::uint16_t {
        ArrayBuffer = 34962,
        ElementArrayBuffer = 34963,
    };

    enum class MimeType : std::uint16_t {
        None = 0,
        JPEG = 1,
        PNG = 2,
        KTX2 = 3,
        DDS = 4,
        GltfBuffer = 5,
        OctetStream = 6,
    };

    enum class AnimationInterpolation : std::uint16_t {
        /**
         * The animated values are linearly interpolated between keyframes. When targeting a
         * rotation, spherical linear interpolation (slerp) SHOULD be used to interpolate quaternions.
         */
        Linear = 0,
        /**
         * The animated values remain constant to the output of the first keyframe, until the next
         * keyframe.
         */
        Step = 1,
        /**
         * The animation’s interpolation is computed using a cubic spline with specified tangents.
         * The number of output elements MUST equal three times the number of input elements. For
         * each input element, the output stores three elements, an in-tangent, a spline vertex,
         * and an out-tangent.
         */
        CubicSpline = 2,
    };

    enum class AnimationPath : std::uint16_t {
        /**
         * The values are the translation along the X, Y, and Z axes.
         */
        Translation = 1,
        /**
         * The values are a quaternion in the order x, y, z, w where w is the scalar.
         */
        Rotation = 2,
        /**
         * The values are scaling factors along the X, Y, and Z axes.
         */
        Scale = 3,
        Weights = 4,
    };

    enum class CameraType : std::uint8_t {
        Perspective = 0,
        Orthographic = 1,
    };

    enum class AlphaMode : std::uint8_t {
        Opaque,
        Mask,
        Blend,
    };

    enum class MeshoptCompressionMode : std::uint8_t {
        None = 0,
        Attributes,
        Triangles,
        Indices,
    };

    enum class MeshoptCompressionFilter : std::uint8_t {
        None = 0,
        Octahedral,
        Quaternion,
        Exponential,
    };

    enum class LightType : std::uint8_t {
        Directional,
        Spot,
        Point,
    };

    enum class Category : std::uint32_t {
        None        = 0,
        Buffers     = 1 <<  0,
        BufferViews = 1 <<  1,
        Accessors   = 1 <<  2,
        Images      = 1 <<  3,
        Samplers    = 1 <<  4,
        Textures    = 1 <<  5,
        Animations  = 1 <<  6,
        Cameras     = 1 <<  7,
        Materials   = 1 <<  8,
        Meshes      = 1 <<  9,
        Skins       = 1 << 10,
        Nodes       = 1 << 11,
        Scenes      = 1 << 12,
        Asset       = 1 << 13,

        All = ~(~0u << 14),
        // Includes everything needed for rendering but animations
        OnlyRenderable = All & ~(Animations) & ~(Skins),
        OnlyAnimations = Animations | Accessors | BufferViews | Buffers,
    };

    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Category, Category, |)
    FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(Category, Category, &)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Category, Category, |)
    FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(Category, Category, &)
    FASTGLTF_UNARY_OP_TEMPLATE_MACRO(Category, ~)
    // clang-format on
#pragma endregion

#pragma region ConversionFunctions
    /**
     * Gets the number of components for each element for the given accessor type. For example, with
     * a Vec3 accessor type this will return 3, as a Vec3 contains 3 components.
     */
    constexpr std::uint8_t getNumComponents(AccessorType type) noexcept {
        return (to_underlying(type) >> 8U) & 0xFFU;
    }

    constexpr std::uint16_t getComponentBitSize(ComponentType componentType) noexcept {
        auto masked = to_underlying(componentType) & 0xFFFF0000;
        return static_cast<std::uint16_t>(masked >> 16U);
    }

    constexpr std::uint16_t getElementByteSize(AccessorType type, ComponentType componentType) noexcept {
        return static_cast<std::uint16_t>(getNumComponents(type)) * (getComponentBitSize(componentType) / 8);
    }

    constexpr std::uint16_t getGLComponentType(ComponentType type) noexcept {
        return to_underlying(type) & 0xFFFF;
    }

    /**
     * Don't use this, use getComponenType instead.
     * This order matters as we assume that their glTF constant is ascending to index it.
     */
    static constexpr std::array<ComponentType, 11> components = {
        ComponentType::Byte,
        ComponentType::UnsignedByte,
        ComponentType::Short,
        ComponentType::UnsignedShort,
        ComponentType::Int,
        ComponentType::UnsignedInt,
        ComponentType::Float,
        ComponentType::Invalid,
        ComponentType::Invalid,
        ComponentType::Invalid,
        ComponentType::Double,
    };

    constexpr ComponentType getComponentType(std::underlying_type_t<ComponentType> componentType) noexcept {
        const auto index = componentType - getGLComponentType(ComponentType::Byte);
        if (index >= components.size())
            return ComponentType::Invalid;
        return components[index];
    }

    // This order matters as we assume that their glTF constant is ascending to index it.
    static constexpr std::array<AccessorType, 7> accessorTypes = {
        AccessorType::Scalar,
        AccessorType::Vec2,
        AccessorType::Vec3,
        AccessorType::Vec4,
        AccessorType::Mat2,
        AccessorType::Mat3,
        AccessorType::Mat4,
    };

    /**
     * Gets the AccessorType by its string representation found in glTF files.
     */
    constexpr AccessorType getAccessorType(std::string_view accessorTypeName) noexcept {
        assert(!accessorTypeName.empty());
        switch (accessorTypeName[0]) {
            case 'S': return AccessorType::Scalar;
            case 'V': {
                auto componentCount = static_cast<std::size_t>(accessorTypeName[3] - '2');
                if (componentCount + 1 >= accessorTypes.size())
                    return AccessorType::Invalid;
                return accessorTypes[componentCount + 1];
            }
            case 'M': {
                auto componentCount = static_cast<std::size_t>(accessorTypeName[3] - '2');
                if (componentCount + 4 >= accessorTypes.size())
                    return AccessorType::Invalid;
                return accessorTypes[componentCount + 4];
            }
        }

        return AccessorType::Invalid;
    }
#pragma endregion

#pragma region Containers
	/*
	 * The amount of items that the SmallVector can initially store in the storage
	 * allocated within the object itself.
	 */
	static constexpr auto initialSmallVectorStorage = 8;

    /**
     * A custom vector class for fastgltf, which can store up to N objects within itself.
     * This is useful for cases where the vector is expected to only ever hold a tiny amount of small objects,
     * such as a node's children.
     * SmallVector is also mostly conformant to C++17's std::vector, and can therefore be used as a drop-in replacement.
     * @note It is also available with polymorphic allocators in the fastgltf::pmr namespace.
     */
    template <typename T, std::size_t N = initialSmallVectorStorage, typename Allocator = std::allocator<T>>
    class SmallVector final {
        static_assert(N != 0, "Cannot create a SmallVector with 0 initial capacity");

        alignas(T) std::array<std::byte, N * sizeof(T)> storage = {};

		Allocator allocator;

        T* _data;
        std::size_t _size = 0, _capacity = N;

        void copy(const T* first, std::size_t count, T* result) {
            if (count > 0) {
				if constexpr (std::is_trivially_copyable_v<T>) {
					std::memcpy(result, first, count * sizeof(T));
				} else {
					*result++ = *first;
					for (std::size_t i = 1; i < count; ++i) {
						*result++ = *++first;
					}
				}
            }
        }

    public:
		using iterator = T*;
		using const_iterator = const T*;

        SmallVector() : _data(reinterpret_cast<T*>(storage.data())) {}

		explicit SmallVector(const Allocator& allocator) noexcept : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {}

        explicit SmallVector(std::size_t size, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            resize(size);
        }

        SmallVector(std::size_t size, const T& value, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            assign(size, value);
        }

        SmallVector(std::initializer_list<T> init, const Allocator& allocator = Allocator()) : allocator(allocator), _data(reinterpret_cast<T*>(storage.data())) {
            assign(init);
        }

        SmallVector(const SmallVector& other) noexcept : _data(reinterpret_cast<T*>(storage.data())) {
            resize(other.size());
            copy(other.begin(), other.size(), begin());
        }

        SmallVector(SmallVector&& other) noexcept : _data(reinterpret_cast<T*>(storage.data())) {
            if (other.isUsingStack()) {
                if (!other.empty()) {
                    resize(other.size());
                    copy(other.begin(), other.size(), begin());
                    other._data = reinterpret_cast<T*>(other.storage.data()); // Reset pointer
                    _size = std::exchange(other._size, 0);
                    _capacity = std::exchange(other._capacity, N);
                }
            } else {
                _data = std::exchange(other._data, nullptr);
                _size = std::exchange(other._size, 0);
                _capacity = std::exchange(other._capacity, N);
            }
        }

        SmallVector& operator=(const SmallVector& other) {
            if (std::addressof(other) != this) {
                if (!isUsingStack() && _data) {
	                std::destroy(begin(), end());
					allocator.deallocate(_data, _capacity);
                    _data = reinterpret_cast<T*>(storage.data());
                    _size = _capacity = 0;
                }

                resize(other.size());
                copy(other.begin(), other.size(), begin());
            }
            return *this;
        }

        SmallVector& operator=(SmallVector&& other) noexcept {
            if (std::addressof(other) != this) {
                if (other.isUsingStack()) {
                    if (!other.empty()) {
                        resize(other.size());
                        copy(other.begin(), other.size(), begin());
                        other._data = reinterpret_cast<T*>(other.storage.data()); // Reset pointer
                        _size = std::exchange(other._size, 0);
                        _capacity = std::exchange(other._capacity, N);
                    }
                } else {
                    _data = std::exchange(other._data, nullptr);
                    _size = std::exchange(other._size, 0);
                    _capacity = std::exchange(other._capacity, N);
                }
            }
            return *this;
        }

        ~SmallVector() {
			// As we use an array of std::byte for the stack storage, we have to destruct those manually too.
			std::destroy(begin(), end());

            if (!isUsingStack() && _data) {
                // Not using the stack, we'll have to free.
	            allocator.deallocate(_data, _capacity);
            }
        }

	    [[nodiscard]] iterator begin() noexcept { return _data; }
        [[nodiscard]] const_iterator begin() const noexcept { return _data; }
	    [[nodiscard]] const_iterator cbegin() const noexcept { return _data; }
	    [[nodiscard]] iterator end() noexcept { return begin() + size(); }
	    [[nodiscard]] const_iterator end() const noexcept { return begin() + size(); }
	    [[nodiscard]] const_iterator cend() const noexcept { return begin() + size(); }

        [[nodiscard]] std::reverse_iterator<T*> rbegin() { return end(); }
        [[nodiscard]] std::reverse_iterator<const T*> rbegin() const { return end(); }
        [[nodiscard]] std::reverse_iterator<const T*> crbegin() const { return end(); }
        [[nodiscard]] std::reverse_iterator<T*> rend() { return begin(); }
        [[nodiscard]] std::reverse_iterator<const T*> rend() const { return begin(); }
        [[nodiscard]] std::reverse_iterator<const T*> crend() const { return begin(); }

        [[nodiscard]] T* data() noexcept { return _data; }
        [[nodiscard]] const T* data() const noexcept { return _data; }
        [[nodiscard]] std::size_t size() const noexcept { return _size; }
        [[nodiscard]] std::size_t size_in_bytes() const noexcept { return _size * sizeof(T); }
        [[nodiscard]] std::size_t capacity() const noexcept { return _capacity; }

        [[nodiscard]] bool empty() const noexcept { return _size == 0; }
        [[nodiscard]] bool isUsingStack() const noexcept { return data() == reinterpret_cast<const T*>(storage.data()); }

        void reserve(std::size_t newCapacity) {
	        static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>, "T needs to be copy constructible.");

            // We don't want to reduce capacity with reserve, only with shrink_to_fit.
            if (newCapacity <= capacity()) {
                return;
            }

            // If the new capacity is lower than what we can hold on the stack, we ignore this request.
            if (newCapacity <= N && isUsingStack()) {
                _capacity = newCapacity;
                if (_size > _capacity) {
                    _size = _capacity;
                }
                return;
            }

            // We use geometric growth, similarly to std::vector.
            newCapacity = std::size_t(1) << (std::numeric_limits<decltype(newCapacity)>::digits - clz(newCapacity));

			T* alloc = allocator.allocate(newCapacity);

			// Copy/Move the old data into the new memory
			for (std::size_t i = 0; i < size(); ++i) {
				auto& x = (*this)[i];
				if constexpr (std::is_nothrow_move_constructible_v<T>) {
					new(alloc + i) T(std::move(x));
				} else if constexpr (std::is_copy_constructible_v<T>) {
					new(alloc + i) T(x);
				} else {
					new(alloc + i) T(std::move(x));
				}
			}

			// Destroy all objects in the old allocation
			std::destroy(begin(), end());

            if (!isUsingStack() && _data && size() != 0) {
				allocator.deallocate(_data, _capacity);
            }

            _data = alloc;
            _capacity = newCapacity;
        }

        void resize(std::size_t newSize) {
			static_assert(std::is_constructible_v<T>, "T has to be constructible");
			if (newSize == size()) {
				return;
			}

			if (newSize < size()) {
				// Just destroy the "overflowing" elements.
				std::destroy(begin() + newSize, end());
			} else {
                // Reserve enough capacity and copy the new value over.
                auto oldSize = _size;
                reserve(newSize);
                for (auto it = begin() + oldSize; it != begin() + newSize; ++it) {
                    new (it) T();
                }
            }

            _size = newSize;
        }

		void resize(std::size_t newSize, const T& value) {
			static_assert(std::is_copy_constructible_v<T>, "T needs to be copy constructible.");
			if (newSize == size()) {
				return;
			}

			if (newSize < size()) {
				// Just destroy the "overflowing" elements.
				std::destroy(begin() + newSize, end());
			} else {
				// Reserve enough capacity and copy the new value over.
				auto oldSize = _size;
				reserve(newSize);
				for (auto it = begin() + oldSize; it != begin() + newSize; ++it) {
					if (it == nullptr)
						break;

					if constexpr (std::is_move_constructible_v<T>) {
						new (it) T(std::move(value));
					} else if constexpr (std::is_trivially_copyable_v<T>) {
						std::memcpy(it, std::addressof(value), sizeof(T));
					} else {
						new (it) T(value);
					}
				}
			}

			_size = newSize;
		}

		void shrink_to_fit() {
			// Only have to shrink if there's any unused capacity.
			if (capacity() == size() || size() == 0) {
				return;
			}

			// If we can use the object's memory again, we'll copy everything over.
			if (size() <= N) {
				copy(begin(), size(), reinterpret_cast<T*>(storage.data()));
				_data = reinterpret_cast<T*>(storage.data());
			} else {
				// We have to use heap allocated memory.
				auto* alloc = allocator.allocate(size());
				for (std::size_t i = 0; i < size(); ++i) {
					new(alloc + i) T((*this)[i]);
				}

				if (_data && !isUsingStack()) {
					std::destroy(begin(), end());
					allocator.deallocate(_data, _capacity);
				}

				_data = alloc;
			}

			_capacity = _size;
		}

		void assign(std::size_t count, const T& value) {
			clear();
			resize(count, value);
		}

		void assign(std::initializer_list<T> init) {
			static_assert(std::is_trivially_copyable_v<T> || std::is_copy_constructible_v<T>, "T needs to be trivially copyable or be copy constructible");
			clear();
			reserve(init.size());
			_size = init.size();

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(begin(), init.begin(), init.size() * sizeof(T));
			} else if constexpr (std::is_copy_constructible_v<T>) {
				for (auto it = init.begin(); it != init.end(); ++it) {
					new (_data + std::distance(init.begin(), it)) T(*it);
				}
			}
		}

		void clear() noexcept {
			std::destroy(begin(), end());

			if (!isUsingStack() && size() != 0) {
				allocator.deallocate(_data, _capacity);
				_data = reinterpret_cast<T*>(storage.data());
			}

			_size = 0;
		}

        template <typename... Args>
        decltype(auto) emplace_back(Args&&... args) {
            // We reserve enough capacity for the new element, and then just increment the size.
			reserve(_size + 1);
			++_size;
            T& result = *(new (std::addressof(back())) T(std::forward<Args>(args)...));
            return (result);
        }

        [[nodiscard]] T& at(std::size_t idx) {
            if (idx >= size()) {
#ifdef __cpp_exceptions
                throw std::out_of_range("Index is out of range for SmallVector");
#else
				std::abort();
#endif
            }
            return begin()[idx];
        }
        [[nodiscard]] const T& at(std::size_t idx) const {
            if (idx >= size()) {
#ifdef __cpp_exceptions
                throw std::out_of_range("Index is out of range for SmallVector");
#else
	            std::abort();
#endif
            }
            return begin()[idx];
        }

        [[nodiscard]] T& operator[](std::size_t idx) {
            assert(idx < size());
            return begin()[idx];
        }
        [[nodiscard]] const T& operator[](std::size_t idx) const {
            assert(idx < size());
            return begin()[idx];
        }

        [[nodiscard]] T& front() {
            assert(!empty());
            return begin()[0];
        }
        [[nodiscard]] const T& front() const {
            assert(!empty());
            return begin()[0];
        }

        [[nodiscard]] T& back() {
            assert(!empty());
            return end()[-1];
        }
        [[nodiscard]] const T& back() const {
            assert(!empty());
            return end()[-1];
        }
    };

	namespace pmr {
		template<typename T, std::size_t N>
		using SmallVector = SmallVector<T, N, std::pmr::polymorphic_allocator<T>>;
	} // namespace pmr

#ifndef FASTGLTF_USE_CUSTOM_SMALLVECTOR
#define FASTGLTF_USE_CUSTOM_SMALLVECTOR 0
#endif

#if FASTGLTF_USE_CUSTOM_SMALLVECTOR
	template <typename T, std::size_t N = initialSmallVectorStorage>
	using MaybeSmallVector = SmallVector<T, N>;
#else
	template <typename T, std::size_t N = 0>
	using MaybeSmallVector = std::vector<T>;
#endif

	namespace pmr {
#if FASTGLTF_USE_CUSTOM_SMALLVECTOR
		template <typename T, std::size_t N = initialSmallVectorStorage>
		using MaybeSmallVector = pmr::SmallVector<T, N>;
#else
		template <typename T, std::size_t N = 0>
		using MaybeSmallVector = std::pmr::vector<T>;
#endif
	} // namespace pmr

	template<typename, typename = void>
	struct OptionalFlagValue {
		static constexpr std::nullopt_t missing_value = std::nullopt;
	};

	template<>
	struct OptionalFlagValue<std::size_t> {
		static constexpr auto missing_value = std::numeric_limits<std::size_t>::max();
	};

	template<>
	struct OptionalFlagValue<float, std::enable_if_t<std::numeric_limits<float>::is_iec559>> {
		// This float is a quiet NaN with a specific bit pattern to be able to differentiate
		// between this flag value and any result from FP operations.
		static constexpr auto missing_value = static_cast<float>(0x7fedb6db);
	};

	template<>
	struct OptionalFlagValue<double, std::enable_if_t<std::numeric_limits<double>::is_iec559>> {
		static constexpr auto missing_value = static_cast<double>(0x7ffdb6db6db6db6d);
	};

	template<>
	struct OptionalFlagValue<Filter> {
		static constexpr auto missing_value = static_cast<Filter>(std::numeric_limits<std::underlying_type_t<Filter>>::max());
	};

	template<>
	struct OptionalFlagValue<BufferTarget> {
		static constexpr auto missing_value = static_cast<BufferTarget>(std::numeric_limits<std::underlying_type_t<BufferTarget>>::max());
	};

	/**
	 * A custom optional class for fastgltf,
	 * which uses so-called "flag values" which are specific values of T that will never be present as an actual value.
	 * We can therefore use those values as flags for whether there is an actual value stored,
	 * instead of the additional bool used by std::optional.
	 *
	 * These flag values are obtained from the specializations of OptionalFlagValue.
	 * If no specialization for T of OptionalFlagValue is provided, a static assert will be triggered.
	 * In those cases, use std::optional or fastgltf::Optional instead.
	 */
	template<typename T>
	class OptionalWithFlagValue final {
		static_assert(!std::is_same_v<std::nullopt_t, std::remove_const_t<decltype(OptionalFlagValue<T>::missing_value)>>,
			"OptionalWithFlagValue can only be used when there is an appropriate specialization of OptionalFlagValue<T>.");

		struct NonTrivialDummy {
			constexpr NonTrivialDummy() noexcept {}
		};

		union {
			NonTrivialDummy dummy;
			std::remove_const_t<T> _value;
		};

	public:
		OptionalWithFlagValue() noexcept {
			reset();
		}

		OptionalWithFlagValue(std::nullopt_t) noexcept {
			reset();
		}

		template <typename U = T, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
		OptionalWithFlagValue(const OptionalWithFlagValue<U>& other) {
			if (other.has_value()) {
				new (std::addressof(_value)) T(*other);
			} else {
				reset();
			}
		}

		template <typename U = T, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
		OptionalWithFlagValue(OptionalWithFlagValue<U>&& other) {
			if (other.has_value()) {
				new (std::addressof(_value)) T(std::move(*other));
			} else {
				reset();
			}
		}

		template<typename... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		explicit OptionalWithFlagValue(std::in_place_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
			: _value(std::forward<Args>(args)...) {}

		template <typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
		OptionalWithFlagValue(U&& _new) noexcept(std::is_nothrow_assignable_v<T&, U> && std::is_nothrow_constructible_v<T, U>) {
			new (std::addressof(_value)) T(std::forward<U>(_new));
		}

		~OptionalWithFlagValue() {
			reset();
		}

		OptionalWithFlagValue& operator=(std::nullopt_t) noexcept {
			reset();
			return *this;
		}

		template<typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
		OptionalWithFlagValue& operator=(U&& _new) noexcept(std::is_nothrow_assignable_v<T&, U> && std::is_nothrow_constructible_v<T, U>) {
			if (has_value()) {
				_value = std::forward<U>(_new);
			} else {
				new(std::addressof(_value)) T(std::forward<U>(_new));
			}
			return *this;
		}

		template <typename U, std::enable_if_t<std::conjunction_v<std::is_constructible<T, const U&>, std::is_assignable<T&, const U&>>, int> = 0>
		OptionalWithFlagValue& operator=(const OptionalWithFlagValue<U>& other) {
			if (other.has_value()) {
				if (has_value()) {
					_value = other._value;
				} else {
					new(std::addressof(_value)) T(other._value);
				}
			} else {
				reset();
			}
			return *this;
		}

		template <typename U, std::enable_if_t<std::conjunction_v<std::is_constructible<T, U>, std::is_assignable<T&, U>>, int> = 0>
		OptionalWithFlagValue& operator=(OptionalWithFlagValue<U>&& other) noexcept(std::is_nothrow_assignable_v<T&, T> && std::is_nothrow_constructible_v<T, T>) {
			if (other.has_value()) {
				if (has_value()) {
					_value = std::move(other._value);
				} else {
					new(std::addressof(_value)) T(other._value);
				}
			} else {
				reset();
			}
			return *this;
		}

		[[nodiscard]] bool has_value() const {
			return this->_value != OptionalFlagValue<T>::missing_value;
		}

		[[nodiscard]] T& value()& {
			if (!has_value()) {
				throw std::bad_optional_access();
			}
			return _value;
		}

		[[nodiscard]] const T& value() const& {
			if (!has_value()) {
				throw std::bad_optional_access();
			}
			return _value;
		}

		[[nodiscard]] T&& value()&& {
			if (!has_value()) {
				throw std::bad_optional_access();
			}
			return std::move(_value);
		}

		[[nodiscard]] const T&& value() const&& {
			if (!has_value()) {
				throw std::bad_optional_access();
			}
			return std::move(_value);
		}

		template<typename U>
		[[nodiscard]] T value_or(U&& default_value) const& {
			return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
		}

		template<typename U>
		[[nodiscard]] T value_or(U&& default_value)&& {
			return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
		}

		void swap(OptionalWithFlagValue<T>& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
		                                                    std::is_nothrow_swappable_v<T>) {
			static_assert(std::is_move_constructible_v<T>);
			if (has_value() && other.has_value()) {
				std::swap(_value, other._value);
			} else if (has_value() && !other.has_value()) {
				other._value = std::move(_value);
			} else if (!has_value() && other.has_value()) {
				_value = std::move(other._value);
			}
		}

		void reset() noexcept {
			this->_value = OptionalFlagValue<T>::missing_value;
		}

		template <typename... Args>
		T& emplace(Args&&... args) {
			new (std::addressof(_value)) T(std::forward<Args>(args)...);
		}

		template <typename U, typename... Args>
		T& emplace(std::initializer_list<U> list, Args&&... args) {
			static_assert(std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>);
			new (std::addressof(_value)) T(list, std::forward<Args>(args)...);
		}

		explicit operator bool() const noexcept {
			return has_value();
		}

		T* operator->() noexcept {
			return std::addressof(_value);
		}

		const T* operator->() const noexcept {
			return std::addressof(_value);
		}

		T& operator*()& noexcept {
			return _value;
		}

		const T& operator*() const& noexcept {
			return _value;
		}

		T&& operator*()&& noexcept {
			return std::move(_value);
		}

		const T&& operator*() const&& noexcept {
			return std::move(_value);
		}
	};

	template <typename T, typename U>
	bool operator==(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) == value;
	}

	template <typename T, typename U>
	bool operator!=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return !(opt == value);
	}

	template <typename T, typename U>
	bool operator<(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) < value;
	}

	template <typename T, typename U>
	bool operator<=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) <= value;
	}

	template <typename T, typename U>
	bool operator>(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) > value;
	}

	template <typename T, typename U>
	bool operator>=(const OptionalWithFlagValue<T>& opt, const U& value) {
		return opt.has_value() && (*opt) >= value;
	}

	/**
	 * A type alias which checks if there is a specialization of OptionalFlagValue for T and "switches"
	 * between fastgltf::OptionalWithFlagValue and std::optional.
	 */
	template <typename T>
	using Optional = std::conditional_t<
		!std::is_same_v<std::nullopt_t, std::remove_const_t<decltype(OptionalFlagValue<T>::missing_value)>>,
		OptionalWithFlagValue<T>,
		std::optional<T>>;
#pragma endregion

#pragma region Structs
	class URI;

	/**
	 * Custom URI class for fastgltf's needs. glTF 2.0 only allows two types of URIs:
	 *  (1) Data URIs as specified in RFC 2397.
	 *  (2) Relative paths as specified in RFC 3986.
	 *
	 * See https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#uris for details.
	 * However, the glTF spec allows more broader URIs in client implementations. Therefore,
	 * this supports all types of URIs as defined in RFC 3986.
	 *
	 * This class, unlike fastgltf::URI, only holds a std::string_view to the URI and therefore
	 * doesn't own the allocation.
	 */
	class URIView {
		friend class URI;

		std::string_view view;

		std::string_view _scheme;
		std::string_view _path;

		std::string_view _userinfo;
		std::string_view _host;
		std::string_view _port;

		std::string_view _query;
		std::string_view _fragment;

		bool _valid = true;

		void parse();

		[[nodiscard]] auto data() const noexcept -> const char*;

	public:
		explicit URIView() noexcept;
		explicit URIView(std::string_view uri) noexcept;
		URIView(const URIView& other) noexcept;

		URIView& operator=(const URIView& other);
		URIView& operator=(std::string_view other);

		[[nodiscard]] auto string() const noexcept -> std::string_view;

		[[nodiscard]] auto scheme() const noexcept -> std::string_view;
		[[nodiscard]] auto userinfo() const noexcept -> std::string_view;
		[[nodiscard]] auto host() const noexcept -> std::string_view;
		[[nodiscard]] auto port() const noexcept -> std::string_view;
		[[nodiscard]] auto path() const noexcept -> std::string_view;
		[[nodiscard]] auto query() const noexcept -> std::string_view;
		[[nodiscard]] auto fragment() const noexcept -> std::string_view;

		[[nodiscard]] auto fspath() const -> std::filesystem::path;
		[[nodiscard]] bool valid() const noexcept;
		[[nodiscard]] bool isLocalPath() const noexcept;
		[[nodiscard]] bool isDataUri() const noexcept;
	};

	/**
	 * Custom URI class for fastgltf's needs. glTF 2.0 only allows two types of URIs:
	 *  (1) Data URIs as specified in RFC 2397.
	 *  (2) Relative paths as specified in RFC 3986.
	 *
	 * See https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#uris for details.
	 * However, the glTF spec allows more broader URIs in client implementations. Therefore,
	 * this supports all types of URIs as defined in RFC 3986.
	 *
	 * This class, unlike fastgltf::URIView, holds a std::string which contains the URI. It
	 * also decodes any percent-encoded characters.
	 */
	class URI {
		std::pmr::string uri;
		URIView view;

		void readjustViews(const URIView& other);

	public:
		explicit URI() noexcept;

		explicit URI(std::string uri) noexcept;
		explicit URI(std::pmr::string uri) noexcept;
		explicit URI(std::string_view uri) noexcept;
		explicit URI(URIView view) noexcept;

		URI(const URI& other);
		URI(URI&& other) noexcept;

		URI& operator=(const URI& other);
		URI& operator=(const URIView& other);
		URI& operator=(URI&& other) noexcept;

		operator URIView() const noexcept;

		static void decodePercents(std::pmr::string& x) noexcept;

		[[nodiscard]] auto string() const noexcept -> std::string_view;

		[[nodiscard]] auto scheme() const noexcept -> std::string_view;
		[[nodiscard]] auto userinfo() const noexcept -> std::string_view;
		[[nodiscard]] auto host() const noexcept -> std::string_view;
		[[nodiscard]] auto port() const noexcept -> std::string_view;
		[[nodiscard]] auto path() const noexcept -> std::string_view;
		[[nodiscard]] auto query() const noexcept -> std::string_view;
		[[nodiscard]] auto fragment() const noexcept -> std::string_view;

		[[nodiscard]] auto fspath() const -> std::filesystem::path;
		[[nodiscard]] bool valid() const noexcept;
		[[nodiscard]] bool isLocalPath() const noexcept;
		[[nodiscard]] bool isDataUri() const noexcept;
    };

    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    /**
     * Custom span class imitating C++20's std::span for referencing bytes without owning the
     * allocation. Can also directly be converted to a std::span or used by itself.
     */
    template <typename T, std::size_t Extent = dynamic_extent>
    class span {
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

        pointer _ptr = nullptr;
        size_type _size = 0;

    public:
        constexpr span() noexcept = default;

        template <typename Iterator>
        explicit constexpr span(Iterator first, size_type count) : _ptr(first), _size(count) {}

        constexpr span(const span& other) noexcept = default;
        constexpr span& operator=(const span& other) noexcept = default;

        [[nodiscard]] constexpr reference operator[](size_type idx) const {
            return data()[idx];
        }

        [[nodiscard]] constexpr pointer data() const noexcept {
            return _ptr;
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]] constexpr size_type size_bytes() const noexcept {
            return size() * sizeof(element_type);
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return size() == 0;
        }

        [[nodiscard]] constexpr span<T, Extent> first(size_type count) const {
            return span(_ptr, count);
        }

#if FASTGLTF_CPP_20
        operator std::span<T>() const {
            return std::span(data(), size());
        }
#endif
    };

    using CustomBufferId = std::uint64_t;

    /**
     * Namespace for structs that describe individual sources of data for images and/or buffers.
     */
    namespace sources {
        struct BufferView {
            std::size_t bufferViewIndex;
            MimeType mimeType;
        };

        struct URI {
            std::size_t fileByteOffset;
            fastgltf::URI uri;
            MimeType mimeType;
        };

        struct Vector {
            std::vector<std::uint8_t> bytes;
            MimeType mimeType;
        };

        struct CustomBuffer {
            CustomBufferId id;
            MimeType mimeType;
        };

        struct ByteView {
            span<const std::byte> bytes;
            MimeType mimeType;
        };
    } // namespace sources

    /**
     * Represents the data source of a buffer or image. These could be a buffer view, a file path
     * (including offsets), a ordinary vector (if #Options::LoadExternalBuffers or #Options::LoadGLBBuffers
     * was specified), or the ID of a custom buffer.
     *
     * @note As a user, you should never encounter this variant holding the std::monostate, as that would be a ill-formed glTF,
     * which fastgltf already checks for while parsing.
     *
     * @note For buffers, this variant will never hold a sources::BufferView, as only images are able to reference buffer views as a source.
     */
    using DataSource = std::variant<std::monostate, sources::BufferView, sources::URI, sources::Vector, sources::CustomBuffer, sources::ByteView>;

    struct AnimationChannel {
        std::size_t samplerIndex;
        std::size_t nodeIndex;
        AnimationPath path;
    };

    struct AnimationSampler {
        std::size_t inputAccessor;
        std::size_t outputAccessor;
        AnimationInterpolation interpolation;
    };

    struct Animation {
	    pmr::MaybeSmallVector<AnimationChannel> channels;
	    pmr::MaybeSmallVector<AnimationSampler> samplers;

        std::pmr::string name;
    };

    struct AssetInfo {
        std::pmr::string gltfVersion;
        std::pmr::string copyright;
        std::pmr::string generator;
    };

    struct Camera {
        struct Orthographic {
            float xmag;
            float ymag;
            float zfar;
            float znear;
        };
        struct Perspective {
            Optional<float> aspectRatio;
            float yfov;
            // If omitted, use an infinite projection matrix.
            Optional<float> zfar;
            float znear;
        };

        /**
         * Variant holding either a perspective or a orthographic camera. Use std::holds_alternative
         * and/or std::get_if to figure out which camera type is being used.
         */
        std::variant<Perspective, Orthographic> camera;
        std::pmr::string name;
    };

    struct Skin {
	    pmr::MaybeSmallVector<std::size_t> joints;
	    Optional<std::size_t> skeleton;
	    Optional<std::size_t> inverseBindMatrices;

        std::pmr::string name;
    };

    struct Sampler {
	    Optional<Filter> magFilter;
	    Optional<Filter> minFilter;
        Wrap wrapS;
        Wrap wrapT;

        std::pmr::string name;
    };

    struct Scene {
	    pmr::MaybeSmallVector<std::size_t> nodeIndices;

        std::pmr::string name;
    };

    struct Node {
        Optional<std::size_t> meshIndex;
	    Optional<std::size_t> skinIndex;
	    Optional<std::size_t> cameraIndex;

        /**
         * Only ever non-empty when KHR_lights_punctual is enabled and used by the asset.
         */
        Optional<std::size_t> lightIndex;

	    pmr::MaybeSmallVector<std::size_t> children;
	    pmr::MaybeSmallVector<float> weights;

        struct TRS {
            std::array<float, 3> translation;
            std::array<float, 4> rotation;
            std::array<float, 3> scale;
        };
        using TransformMatrix = std::array<float, 16>;

        /**
         * Variant holding either the three TRS components; transform, rotation, and scale, or a
         * transformation matrix, which cannot skew or shear. The latter can be decomposed into
         * the TRS components by specifying Options::DecomposeNodeMatrices.
         */
        std::variant<TRS, TransformMatrix> transform;

        /**
         * Only ever non-empty when EXT_mesh_gpu_instancing is enabled and used by the asset.
         */
        std::pmr::vector<std::pair<std::pmr::string, std::size_t>> instancingAttributes;

        std::pmr::string name;
 
        [[nodiscard]] auto findInstancingAttribute(std::string_view attributeName) noexcept {
            for (auto it = instancingAttributes.begin(); it != instancingAttributes.end(); ++it) {
                if (it->first == attributeName)
                    return it;
            }
            return instancingAttributes.end();
        }

        [[nodiscard]] auto findInstancingAttribute(std::string_view attributeName) const noexcept {
            for (auto it = instancingAttributes.cbegin(); it != instancingAttributes.cend(); ++it) {
                if (it->first == attributeName)
                    return it;
            }
            return instancingAttributes.cend();
        }
   };

    struct Primitive {
		using attribute_type = std::pair<std::pmr::string, std::size_t>;

		// Instead of a map, we have a list of attributes here. Each pair contains
		// the name of the attribute and the corresponding accessor index.
		pmr::SmallVector<attribute_type, 4> attributes;
        PrimitiveType type;

        std::pmr::vector<pmr::SmallVector<attribute_type, 4>> targets;

        Optional<std::size_t> indicesAccessor;
        Optional<std::size_t> materialIndex;

		[[nodiscard]] auto findAttribute(std::string_view name) noexcept {
			for (decltype(attributes)::iterator it = attributes.begin(); it != attributes.end(); ++it) {
				if (it->first == name)
					return it;
			}
			return attributes.end();
		}

		[[nodiscard]] auto findAttribute(std::string_view name) const noexcept {
			for (decltype(attributes)::const_iterator it = attributes.cbegin(); it != attributes.cend(); ++it) {
				if (it->first == name)
					return it;
			}
			return attributes.cend();
		}

		[[nodiscard]] auto findTargetAttribute(std::size_t targetIndex, std::string_view name) noexcept {
			auto& targetAttributes = targets[targetIndex];
			for (std::remove_reference_t<decltype(targetAttributes)>::iterator it = targetAttributes.begin(); it != targetAttributes.end(); ++it) {
				if (it->first == name)
					return it;
			}
			return targetAttributes.end();
		}

		[[nodiscard]] auto findTargetAttribute(std::size_t targetIndex, std::string_view name) const noexcept {
			const auto& targetAttributes = targets[targetIndex];
			for (std::remove_reference_t<decltype(targetAttributes)>::const_iterator it = targetAttributes.cbegin(); it != targetAttributes.cend(); ++it) {
				if (it->first == name)
					return it;
			}
			return targetAttributes.cend();
		}
	};

    struct Mesh {
		pmr::MaybeSmallVector<Primitive, 2> primitives;
		pmr::MaybeSmallVector<float> weights;

        std::pmr::string name;
    };

    /**
     * Texture transform information as per KHR_texture_transform.
     */
    struct TextureTransform {
        /**
         * The offset of the UV coordinate origin as a factor of the texture dimensions.
         */
        float rotation;

        /**
         * Rotate the UVs by this many radians counter-clockwise around the origin. This is equivalent to a similar rotation of the image clockwise.
         */
        std::array<float, 2> uvOffset;

        /**
         * The scale factor applied to the components of the UV coordinates.
         */
        std::array<float, 2> uvScale;

        /**
         * Overrides the textureInfo texCoord value if supplied.
         */
        Optional<std::size_t> texCoordIndex;
    };

    struct TextureInfo {
        std::size_t textureIndex;
        std::size_t texCoordIndex;

        /**
         * Data from KHR_texture_transform, and nullptr if the extension wasn't enabled or used.
         */
        std::unique_ptr<TextureTransform> transform;
    };

	struct NormalTextureInfo : TextureInfo {
		float scale;
	};

	struct OcclusionTextureInfo : TextureInfo {
		float strength;
	};

    struct PBRData {
        /**
         * The factors for the base color of then material.
         */
        std::array<float, 4> baseColorFactor = {{ 1, 1, 1, 1 }};

        /**
         * The factor for the metalness of the material.
         */
        float metallicFactor = 1.0f;

        /**
         * The factor for the roughness of the material.
         */
        float roughnessFactor = 1.0f;

        Optional<TextureInfo> baseColorTexture;
        Optional<TextureInfo> metallicRoughnessTexture;
    };

	struct MaterialAnisotropy {
		float anisotropyStrength;
		float anisotropyRotation;
		Optional<TextureInfo> anisotropyTexture;
	};

    /**
     * Specular information from KHR_materials_specular.
     */
    struct MaterialSpecular {
        float specularFactor;
        Optional<TextureInfo> specularTexture;
        std::array<float, 3> specularColorFactor;
        Optional<TextureInfo> specularColorTexture;
    };

    /**
     * Iridescence information from KHR_materials_iridescence
     */
    struct MaterialIridescence {
        float iridescenceFactor;
        Optional<TextureInfo> iridescenceTexture;
        float iridescenceIor;
        float iridescenceThicknessMinimum;
        float iridescenceThicknessMaximum;
        Optional<TextureInfo> iridescenceThicknessTexture;
    };

    /**
     * Volume information from KHR_materials_volume
     */
    struct MaterialVolume {
        float thicknessFactor;
        Optional<TextureInfo> thicknessTexture;
        float attenuationDistance;
        std::array<float, 3> attenuationColor;
    };

    struct MaterialTransmission {
        float transmissionFactor;
        Optional<TextureInfo> transmissionTexture;
    };

    struct MaterialClearcoat {
        float clearcoatFactor;
        Optional<TextureInfo> clearcoatTexture;
        float clearcoatRoughnessFactor;
        Optional<TextureInfo> clearcoatRoughnessTexture;
        Optional<TextureInfo> clearcoatNormalTexture;
    };

    struct MaterialSheen {
        std::array<float, 3> sheenColorFactor;
        Optional<TextureInfo> sheenColorTexture;
        float sheenRoughnessFactor;
        Optional<TextureInfo> sheenRoughnessTexture;
    };

#if FASTGLTF_ENABLE_DEPRECATED_EXT
    /**
     * Specular/Glossiness information from KHR_materials_pbrSpecularGlossiness.
     */
    struct MaterialSpecularGlossiness {
        std::array<float, 4> diffuseFactor;
        Optional<TextureInfo> diffuseTexture;
        std::array<float, 3> specularFactor;
        float glossinessFactor;
        Optional<TextureInfo> specularGlossinessTexture;
    };
#endif

    struct Material {
        /**
         * A set of parameter values that are used to define the metallic-roughness material model
         * from Physically Based Rendering (PBR) methodology.
         */
        PBRData pbrData;

        /**
         * The tangent space normal texture.
         */
        Optional<NormalTextureInfo> normalTexture;
        Optional<OcclusionTextureInfo> occlusionTexture;
        Optional<TextureInfo> emissiveTexture;

        /**
         * The factors for the emissive color of the material. Defaults to 0,0,0
         */
        std::array<float, 3> emissiveFactor;

        /**
         * The values used to determine the transparency of the material.
         * Defaults to #AlphaMode::Opaque.
         */
        AlphaMode alphaMode;

		/**
		 * The alpha value that determines the upper limit for fragments that
		 * should be discarded for transparency. Defaults to 0.5.
		 */
        float alphaCutoff;

        /**
         * Determines whether back-face culling should be disabled when using this material.
         */
        bool doubleSided;

		std::unique_ptr<MaterialAnisotropy> anisotropy;

        std::unique_ptr<MaterialClearcoat> clearcoat;

        /**
         * Iridescence information from KHR_materials_iridescence.
         */
        std::unique_ptr<MaterialIridescence> iridescence;

        std::unique_ptr<MaterialSheen> sheen;

        /**
         * Specular information from KHR_materials_specular.
         */
        std::unique_ptr<MaterialSpecular> specular;

#if FASTGLTF_ENABLE_DEPRECATED_EXT
        /**
         * Specular/Glossiness information from KHR_materials_pbrSpecularGlossiness.
         */
        std::unique_ptr<MaterialSpecularGlossiness> specularGlossiness;
#endif

        /**
         * Specular information from KHR_materials_transmission.
         */
        std::unique_ptr<MaterialTransmission> transmission;

        /**
         * Volume information from KHR_materials_volume
         */
        std::unique_ptr<MaterialVolume> volume;

        /**
         * The emissive strength from the KHR_materials_emissive_strength extension.
         */
        Optional<float> emissiveStrength;

        /**
         * The index of refraction as specified through KHR_materials_ior.
         */
        Optional<float> ior;

        /**
         * Only applicable if KHR_materials_unlit is enabled.
         */
        bool unlit;

        std::pmr::string name;
    };

    struct Texture {
        /**
         * When empty, an extension or other mechanism SHOULD supply an alternate texture source,
         * otherwise behavior is undefined.
         */
        Optional<std::size_t> imageIndex;

        /**
         * If the imageIndex is specified by the KTX2 or DDS glTF extensions, this is supposed to
         * be used as a fallback if those file containers are not supported.
         */
        Optional<std::size_t> fallbackImageIndex;

        /**
         * If no sampler is specified, use a default sampler with repeat wrap and auto filter.
         */
        Optional<std::size_t> samplerIndex;

        std::pmr::string name;
    };

    struct Image {
        DataSource data;

        std::pmr::string name;
    };

    struct SparseAccessor {
        std::size_t count;
        std::size_t indicesBufferView;
        std::size_t indicesByteOffset;
        std::size_t valuesBufferView;
        std::size_t valuesByteOffset;
        ComponentType indexComponentType;
    };

    struct Accessor {
        std::size_t byteOffset;
        std::size_t count;
        AccessorType type;
        ComponentType componentType;
        bool normalized;
        
        std::variant<std::monostate, std::pmr::vector<double>, std::pmr::vector<std::int64_t>> max;
        std::variant<std::monostate, std::pmr::vector<double>, std::pmr::vector<std::int64_t>> min;

        // Could have no value for sparse morph targets
        Optional<std::size_t> bufferViewIndex;

        Optional<SparseAccessor> sparse;

        std::pmr::string name;
    };

    struct CompressedBufferView {
        std::size_t bufferIndex;
        std::size_t byteOffset;
        std::size_t byteLength;
        std::size_t count;
        MeshoptCompressionMode mode;
        MeshoptCompressionFilter filter;

        std::size_t byteStride;
    };

    struct BufferView {
        std::size_t bufferIndex;
        std::size_t byteOffset;
        std::size_t byteLength;

        Optional<std::size_t> byteStride;
        Optional<BufferTarget> target;

        /**
         * Data from EXT_meshopt_compression, and nullptr if the extension was not enabled or used.
         */
        std::unique_ptr<CompressedBufferView> meshoptCompression;

        std::pmr::string name;
    };

    struct Buffer {
        std::size_t byteLength;

        DataSource data;

        std::pmr::string name;
    };

    struct Light {
        LightType type;
        /** RGB light color in linear space. */
        std::array<float, 3> color;

        /** Point and spot lights use candela (lm/sr) while directional use lux (lm/m^2) */
        float intensity;
        /** Range for point and spot lights. If not present, range is infinite. */
        Optional<float> range;

        Optional<float> innerConeAngle;
        Optional<float> outerConeAngle;

        std::pmr::string name;
    };

	class ChunkMemoryResource;
	class Parser;

	class Asset {
		friend class Parser;

		// This has to be first in this struct so that it gets destroyed last, leaving all allocations
		// alive until the end.
		std::shared_ptr<ChunkMemoryResource> memoryResource;

	public:
        /**
         * This will only ever have no value if #Options::DontRequireValidAssetMember was specified.
         */
        Optional<AssetInfo> assetInfo;
		std::pmr::vector<std::pmr::string> extensionsUsed;
		std::pmr::vector<std::pmr::string> extensionsRequired;

        Optional<std::size_t> defaultScene;
        std::vector<Accessor> accessors;
        std::vector<Animation> animations;
        std::vector<Buffer> buffers;
        std::vector<BufferView> bufferViews;
        std::vector<Camera> cameras;
        std::vector<Image> images;
        std::vector<Light> lights;
        std::vector<Material> materials;
        std::vector<Mesh> meshes;
        std::vector<Node> nodes;
        std::vector<Sampler> samplers;
        std::vector<Scene> scenes;
        std::vector<Skin> skins;
        std::vector<Texture> textures;

        // Keeps tracked of categories that were actually parsed.
        Category availableCategories = Category::None;

        explicit Asset() = default;
        explicit Asset(const Asset& other) = delete;
        Asset(Asset&& other) noexcept :
				memoryResource(std::move(other.memoryResource)),
				assetInfo(std::move(other.assetInfo)),
				extensionsUsed(std::move(other.extensionsUsed)),
				extensionsRequired(std::move(other.extensionsRequired)),
				defaultScene(other.defaultScene),
				accessors(std::move(other.accessors)),
				animations(std::move(other.animations)),
				buffers(std::move(other.buffers)),
				bufferViews(std::move(other.bufferViews)),
				cameras(std::move(other.cameras)),
				images(std::move(other.images)),
				lights(std::move(other.lights)),
				materials(std::move(other.materials)),
				meshes(std::move(other.meshes)),
				nodes(std::move(other.nodes)),
				samplers(std::move(other.samplers)),
				scenes(std::move(other.scenes)),
				skins(std::move(other.skins)),
				textures(std::move(other.textures)),
				availableCategories(other.availableCategories) {}

		Asset& operator=(const Asset& other) = delete;
		Asset& operator=(Asset&& other) noexcept {
			memoryResource = std::move(other.memoryResource);
			assetInfo = std::move(other.assetInfo);
			extensionsUsed = std::move(other.extensionsUsed);
			extensionsRequired = std::move(other.extensionsRequired);
			defaultScene = other.defaultScene;
			accessors = std::move(other.accessors);
			animations = std::move(other.animations);
			buffers = std::move(other.buffers);
			bufferViews = std::move(other.bufferViews);
			cameras = std::move(other.cameras);
			images = std::move(other.images);
			lights = std::move(other.lights);
			materials = std::move(other.materials);
			meshes = std::move(other.meshes);
			nodes = std::move(other.nodes);
			samplers = std::move(other.samplers);
			scenes = std::move(other.scenes);
			skins = std::move(other.skins);
			textures = std::move(other.textures);
			availableCategories = other.availableCategories;
			return *this;
		}
    };
#pragma endregion
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif
