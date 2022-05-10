#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <new>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

namespace hot {
    namespace singlethreaded {


        template<class ValueType, class TIDType>
        constexpr unsigned int determineStackSize() {
            return (hardware_destructive_interference_size - sizeof(ValueType) - sizeof(size_t) -
                    sizeof(std::vector<TIDType>)) /
                   sizeof(TIDType);
        }
//TODO: set semantic, disallow duplicate TIDs
        template<class ValueType, class TIDType, unsigned int StackSize = determineStackSize<ValueType, TIDType>()>
        struct TIDSpan {
            static_assert(StackSize > 0);
            ValueType value;
            size_t _size;
            std::array<TIDType, StackSize> stackValues;
            std::vector<TIDType> furtherValues;

            TIDSpan() = default;

            TIDSpan(const ValueType value, const TIDType tid) : value{value}, _size{1}, stackValues{}, furtherValues{} {
                stackValues[0] = tid;
            }

            TIDType &operator[](const size_t idx) {
                if (idx < StackSize)
                    return stackValues[idx];
                else
                    return furtherValues[idx - StackSize];
            }

            void push_back(TIDType &val) {
                if (_size < StackSize)
                    stackValues[_size] = val;
                else
                    furtherValues.push_back(val);
                ++_size;
            }

            size_t size() const {
                return _size;
            }

            bool empty() const {
                return _size == 0;
            }
        };
    }
}