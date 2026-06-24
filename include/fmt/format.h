/*
  Formatting library for C++

  Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  --- Optional exception to the license ---

  As an exception, if, as a result of your compiling your source code, portions
  of this Software are embedded into a machine-executable object form of such
  source code, you may redistribute such embedded portions in such object form
  without including the above copyright and permission notices.
 */

#ifndef FMT_FORMAT_H_
#define FMT_FORMAT_H_

#ifndef _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#  define _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#  define FMT_REMOVE_TRANSITIVE_INCLUDES
#endif

#include "base.h"

// libc++ supports string_view in pre-c++17.
#if FMT_HAS_INCLUDE(<string_view>) && \
    (FMT_CPLUSPLUS >= 201703L || defined(_LIBCPP_VERSION))
#  define FMT_USE_STRING_VIEW
#endif

#ifndef FMT_MODULE
#  include <stdint.h>  // uint32_t
#  include <stdlib.h>  // malloc, free
#  include <string.h>  // memcpy

#  include <cmath>   // std::signbit
#  include <limits>  // std::numeric_limits
#  if defined(__GLIBCXX__) && !defined(_GLIBCXX_USE_DUAL_ABI)
// Workaround for pre gcc 5 libstdc++.
#    include <memory>  // std::allocator_traits
#  endif
#  include <stdexcept>     // std::runtime_error
#  include <string>        // std::string
#  include <system_error>  // std::system_error

// Check FMT_CPLUSPLUS to avoid a warning in MSVC.
#  if FMT_HAS_INCLUDE(<bit>) && FMT_CPLUSPLUS > 201703L
#    include <bit>  // std::bit_cast
#  endif

#  if defined(FMT_USE_STRING_VIEW)
#    include <string_view>
#  endif

#  if FMT_MSC_VERSION
#    include <intrin.h>  // _BitScanReverse[64], _umul128
#  endif
#endif  // FMT_MODULE

#if defined(FMT_USE_NONTYPE_TEMPLATE_ARGS)
// Use the provided definition.
#elif defined(__NVCOMPILER)
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#elif FMT_GCC_VERSION >= 903 && FMT_CPLUSPLUS >= 201709L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#elif defined(__cpp_nontype_template_args) && \
    __cpp_nontype_template_args >= 201911L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#elif FMT_CLANG_VERSION >= 1200 && FMT_CPLUSPLUS >= 202002L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#else
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#endif

#if defined __cpp_inline_variables && __cpp_inline_variables >= 201606L
#  define FMT_INLINE_VARIABLE inline
#else
#  define FMT_INLINE_VARIABLE
#endif

// Check if RTTI is disabled.
#ifdef FMT_USE_RTTI
// Use the provided definition.
#elif defined(__GXX_RTTI) || FMT_HAS_FEATURE(cxx_rtti) || defined(_CPPRTTI) || \
    defined(__INTEL_RTTI__) || defined(__RTTI)
// __RTTI is for EDG compilers. _CPPRTTI is for MSVC.
#  define FMT_USE_RTTI 1
#else
#  define FMT_USE_RTTI 0
#endif

// Visibility when compiled as a shared library/object.
#if defined(FMT_LIB_EXPORT) || defined(FMT_SHARED)
#  define FMT_SO_VISIBILITY(value) FMT_VISIBILITY(value)
#else
#  define FMT_SO_VISIBILITY(value)
#endif

#if FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_NOINLINE __attribute__((noinline))
#else
#  define FMT_NOINLINE
#endif

// Detect constexpr std::string.
#if !FMT_USE_CONSTEVAL
#  define FMT_USE_CONSTEXPR_STRING 0
#elif defined(__cpp_lib_constexpr_string) && \
    __cpp_lib_constexpr_string >= 201907L
#  if FMT_CLANG_VERSION && FMT_GLIBCXX_RELEASE
// clang + libstdc++ are able to work only starting with gcc13.3
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=113294
#    if FMT_GLIBCXX_RELEASE < 13
#      define FMT_USE_CONSTEXPR_STRING 0
#    elif FMT_GLIBCXX_RELEASE == 13 && __GLIBCXX__ < 20240521
#      define FMT_USE_CONSTEXPR_STRING 0
#    else
#      define FMT_USE_CONSTEXPR_STRING 1
#    endif
#  else
#    define FMT_USE_CONSTEXPR_STRING 1
#  endif
#else
#  define FMT_USE_CONSTEXPR_STRING 0
#endif
#if FMT_USE_CONSTEXPR_STRING
#  define FMT_CONSTEXPR_STRING constexpr
#else
#  define FMT_CONSTEXPR_STRING
#endif

// GCC 4.9 doesn't support qualified names in specializations.
namespace std {
template <typename T> struct iterator_traits<fmt::basic_appender<T>> {
  using iterator_category = output_iterator_tag;
  using value_type = T;
  using difference_type =
      decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
  using pointer = void;
  using reference = void;
};
}  // namespace std

#ifdef FMT_THROW
// Use the provided definition.
#elif FMT_USE_EXCEPTIONS
#  define FMT_THROW(x) throw x
#else
#  define FMT_THROW(x) ::fmt::assert_fail(__FILE__, __LINE__, (x).what())
#endif

#ifdef __clang_analyzer__
#  define FMT_CLANG_ANALYZER 1
#else
#  define FMT_CLANG_ANALYZER 0
#endif

// Defining FMT_REDUCE_INT_INSTANTIATIONS to 1, will reduce the number of
// integer formatter template instantiations to just one by only using the
// largest integer type. This results in a reduction in binary size but will
// cause a decrease in integer formatting performance.
#if !defined(FMT_REDUCE_INT_INSTANTIATIONS)
#  define FMT_REDUCE_INT_INSTANTIATIONS 0
#endif

FMT_BEGIN_NAMESPACE

template <typename Char, typename Traits, typename Allocator>
struct is_contiguous<std::basic_string<Char, Traits, Allocator>>
    : std::true_type {};

namespace detail {

// __builtin_clz is broken in clang with Microsoft codegen:
// https://github.com/fmtlib/fmt/issues/519.
#if !FMT_MSC_VERSION
#  if FMT_HAS_BUILTIN(__builtin_clz) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
#  endif
#  if FMT_HAS_BUILTIN(__builtin_clzll) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#  endif
#endif

// Some compilers masquerade as both MSVC and GCC but otherwise support
// __builtin_clz and __builtin_clzll, so only define FMT_BUILTIN_CLZ using the
// MSVC intrinsics if the clz and clzll builtins are not available.
#if FMT_MSC_VERSION && !defined(FMT_BUILTIN_CLZLL)
// Avoid Clang with Microsoft CodeGen's -Wunknown-pragmas warning.
#  ifndef __clang__
#    pragma intrinsic(_BitScanReverse)
#    ifdef _WIN64
#      pragma intrinsic(_BitScanReverse64)
#    endif
#  endif

inline auto clz(uint32_t x) -> int {
  FMT_ASSERT(x != 0, "");
  unsigned long r = 0;
  _BitScanReverse(&r, x);
  return 31 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZ(n) detail::clz(n)

inline auto clzll(uint64_t x) -> int {
  FMT_ASSERT(x != 0, "");
  unsigned long r = 0;
#  ifdef _WIN64
  _BitScanReverse64(&r, x);
#  else
  // Scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    return 63 ^ static_cast<int>(r + 32);
  // Scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(x));
#  endif
  return 63 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZLL(n) detail::clzll(n)
#endif  // FMT_MSC_VERSION && !defined(FMT_BUILTIN_CLZLL)

FMT_CONSTEXPR inline void abort_fuzzing_if(bool condition) {
    return {};
}

#if defined(FMT_USE_STRING_VIEW)
template <typename Char> using std_string_view = std::basic_string_view<Char>;
#else
template <typename Char> struct std_string_view {
  operator basic_string_view<Char>() const;
};
#endif

template <typename Char, Char... C> struct string_literal {
  static constexpr Char value[sizeof...(C)] = {C...};
  constexpr operator basic_string_view<Char>() const {
    return {};
}
};
#if FMT_CPLUSPLUS < 201703L
template <typename Char, Char... C>
constexpr Char string_literal<Char, C...>::value[sizeof...(C)];
#endif

// Implementation of std::bit_cast for pre-C++20.
template <typename To, typename From, FMT_ENABLE_IF(sizeof(To) == sizeof(From))>
FMT_CONSTEXPR20 auto bit_cast(const From& from) -> To {
    throw std::runtime_error("STUB: not implemented");
}

inline auto is_big_endian() -> bool {
    throw std::runtime_error("STUB: not implemented");
}

class uint128 {
 private:
  uint64_t lo_, hi_;

 public:
  constexpr uint128(uint64_t hi, uint64_t lo) : lo_(lo), hi_(hi) {
    return {};
}
  constexpr uint128(uint64_t value = 0) : lo_(value), hi_(0) {
    return {};
}

  constexpr auto high() const noexcept -> uint64_t {
    return {};
}
  constexpr auto low() const noexcept -> uint64_t {
    return {};
}

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  constexpr explicit operator T() const {
    return {};
}

  friend constexpr auto operator==(const uint128& lhs, const uint128& rhs)
      -> bool {
    return {};
}
  friend constexpr auto operator!=(const uint128& lhs, const uint128& rhs)
      -> bool {
    return {};
}
  friend constexpr auto operator>(const uint128& lhs, const uint128& rhs)
      -> bool {
    return {};
}
  friend constexpr auto operator|(const uint128& lhs, const uint128& rhs)
      -> uint128 {
    return {};
}
  friend constexpr auto operator&(const uint128& lhs, const uint128& rhs)
      -> uint128 {
    return {};
}
  friend constexpr auto operator~(const uint128& n) -> uint128 {
    return {};
}
  friend FMT_CONSTEXPR auto operator+(const uint128& lhs, const uint128& rhs)
      -> uint128 {
    return {};
}
  friend FMT_CONSTEXPR auto operator*(const uint128& lhs, uint32_t rhs)
      -> uint128 {
    return {};
}
  friend constexpr auto operator-(const uint128& lhs, uint64_t rhs) -> uint128 {
    return {};
}
  FMT_CONSTEXPR auto operator>>(int shift) const -> uint128 {
    return {};
}
  FMT_CONSTEXPR auto operator<<(int shift) const -> uint128 {
    return {};
}
  FMT_CONSTEXPR auto operator>>=(int shift) -> uint128& {
    return {};
}
  FMT_CONSTEXPR void operator+=(uint128 n) {
    return {};
}
  FMT_CONSTEXPR void operator&=(uint128 n) {
    return {};
}

  FMT_CONSTEXPR20 auto operator+=(uint64_t n) noexcept -> uint128& {
    abort();
}
};

using uint128_t = conditional_t<FMT_USE_INT128, native_uint128, uint128>;

#ifdef UINTPTR_MAX
using uintptr_t = ::uintptr_t;
#else
using uintptr_t = uint128_t;
#endif

// Returns the largest possible value for type T. Same as
// std::numeric_limits<T>::max() but shorter and not affected by the max macro.
template <typename T> constexpr auto max_value() -> T {
    return {};
}
template <typename T> constexpr auto num_bits() -> int {
    return {};
}
// std::numeric_limits<T>::digits may return 0 for 128-bit ints.
template <> constexpr auto num_bits<native_int128>() -> int {
    return {};
}
template <> constexpr auto num_bits<native_uint128>() -> int {
    return {};
}
template <> constexpr auto num_bits<uint128>() -> int {
    return {};
}

// A heterogeneous bit_cast used for converting 96-bit long double to uint128_t
// and 128-bit pointers to uint128.
template <typename To, typename From, FMT_ENABLE_IF(sizeof(To) > sizeof(From))>
inline auto bit_cast(const From& from) -> To {
    throw std::runtime_error("STUB: not implemented");
}

template <typename UInt>
FMT_CONSTEXPR20 inline auto countl_zero_fallback(UInt n) -> int {
    throw std::runtime_error("STUB: not implemented");
}

FMT_CONSTEXPR20 inline auto countl_zero(uint32_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}

FMT_CONSTEXPR20 inline auto countl_zero(uint64_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}

FMT_INLINE void assume(bool condition) {
    throw std::runtime_error("STUB: not implemented");
}

// Attempts to reserve space for n extra characters in the output range.
// Returns a pointer to the reserved range or a reference to it.
template <typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value&&
                            is_contiguous<typename OutputIt::container>::value)>
#if FMT_CLANG_VERSION >= 307 && !FMT_ICC_VERSION
__attribute__((no_sanitize("undefined")))
#endif
FMT_CONSTEXPR20 inline auto reserve(OutputIt it, size_t n) ->
    typename OutputIt::value_type* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T>
FMT_CONSTEXPR20 inline auto reserve(basic_appender<T> it, size_t n)
    -> basic_appender<T> {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Iterator>
constexpr auto reserve(Iterator& it, size_t) -> Iterator& {
    return {};
}

template <typename OutputIt>
using reserve_iterator =
    remove_reference_t<decltype(reserve(std::declval<OutputIt&>(), 0))>;

template <typename T, typename OutputIt>
constexpr auto to_pointer(OutputIt, size_t) -> T* {
    return {};
}
template <typename T> FMT_CONSTEXPR auto to_pointer(T*& ptr, size_t n) -> T* {
    return {};
}
template <typename T>
FMT_CONSTEXPR20 auto to_pointer(basic_appender<T> it, size_t n) -> T* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value&&
                            is_contiguous<typename OutputIt::container>::value)>
inline auto base_iterator(OutputIt it,
                          typename OutputIt::container_type::value_type*)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Iterator>
constexpr auto base_iterator(Iterator, Iterator it) -> Iterator {
    return {};
}

// <algorithm> is spectacularly slow to compile in C++20 so use a simple fill_n
// instead (#1998).
template <typename OutputIt, typename Size, typename T>
FMT_CONSTEXPR auto fill_n(OutputIt out, Size count, const T& value)
    -> OutputIt {
    return {};
}
template <typename T, typename Size>
FMT_CONSTEXPR20 auto fill_n(T* out, Size count, char value) -> T* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, typename V, typename OutputIt>
FMT_CONSTEXPR auto copy(basic_string_view<V> s, OutputIt out) -> OutputIt {
    return {};
}

template <typename OutChar, typename InputIt, typename OutputIt>
FMT_CONSTEXPR FMT_NOINLINE auto copy_noinline(InputIt begin, InputIt end,
                                              OutputIt out) -> OutputIt {
    return {};
}

// A public domain branchless UTF-8 decoder by Christopher Wellons:
// https://github.com/skeeto/branchless-utf8
/* Decode the next character, c, from s, reporting errors in e.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ have at least three bytes of zero padding
 * following the end of the data stream.
 *
 * Errors are reported in e, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */
FMT_CONSTEXPR inline auto utf8_decode(const char* s, uint32_t* c, int* e)
    -> const char* {
    return {};
}

constexpr FMT_INLINE_VARIABLE uint32_t invalid_code_point = ~uint32_t();

// Invokes f(cp, sv) for every code point cp in s with sv being the string view
// corresponding to the code point. cp is invalid_code_point on error.
template <typename F>
FMT_CONSTEXPR void for_each_codepoint(string_view s, F f) {
    return {};
}

FMT_CONSTEXPR inline auto display_width_of(uint32_t cp) noexcept -> size_t {
    return {};
}

template <typename T> struct is_integral : std::is_integral<T> {};
template <> struct is_integral<native_int128> : std::true_type {};
template <> struct is_integral<uint128_t> : std::true_type {};

template <typename T>
using is_signed =
    std::integral_constant<bool, std::numeric_limits<T>::is_signed ||
                                     std::is_same<T, native_int128>::value>;

template <typename T>
using is_integer =
    bool_constant<is_integral<T>::value && !std::is_same<T, bool>::value &&
                  !std::is_same<T, char>::value &&
                  !std::is_same<T, wchar_t>::value>;

#if defined(FMT_USE_FLOAT128)
// Use the provided definition.
#elif FMT_CLANG_VERSION >= 309 && FMT_HAS_INCLUDE(<quadmath.h>)
#  define FMT_USE_FLOAT128 1
#elif FMT_GCC_VERSION && defined(_GLIBCXX_USE_FLOAT128) && \
    !defined(__STRICT_ANSI__)
#  define FMT_USE_FLOAT128 1
#else
#  define FMT_USE_FLOAT128 0
#endif
#if FMT_USE_FLOAT128
using float128 = __float128;
#else
struct float128 {};
#endif

template <typename T> using is_float128 = std::is_same<T, float128>;

template <typename T> struct is_floating_point : std::is_floating_point<T> {};
template <> struct is_floating_point<float128> : std::true_type {};

template <typename T, bool = is_floating_point<T>::value>
struct is_fast_float : bool_constant<std::numeric_limits<T>::is_iec559 &&
                                     sizeof(T) <= sizeof(double)> {};
template <typename T> struct is_fast_float<T, false> : std::false_type {};

template <typename T>
using fast_float_t = conditional_t<sizeof(T) == sizeof(double), double, float>;

template <typename T>
using is_double_double = bool_constant<std::numeric_limits<T>::digits == 106>;

FMT_API auto allocate(size_t size) -> void*;

// An allocator that uses malloc/free to allow removing dependency on the C++
// standard library runtime. std::decay is used for back_inserter to be found by
// ADL when applied to memory_buffer.
template <typename T> struct allocator : private std::decay<void> {
  using value_type = T;

  auto allocate(size_t n) -> T* {
    throw std::runtime_error("STUB: not implemented");
}

  void deallocate(T* p, size_t) {
    throw std::runtime_error("STUB: not implemented");
}

  constexpr friend auto operator==(allocator, allocator) noexcept -> bool {
    return {};
}
  constexpr friend auto operator!=(allocator, allocator) noexcept -> bool {
    return {};
}
};

template <typename Formatter>
FMT_CONSTEXPR auto maybe_set_debug_format(Formatter& f, bool set)
    -> decltype(f.set_debug_format(set)) {
    return {};
}
template <typename Formatter>
FMT_CONSTEXPR void maybe_set_debug_format(Formatter&, ...) {
    return {};
}

}  // namespace detail

FMT_BEGIN_EXPORT

// The number of characters to store in the basic_memory_buffer object itself
// to avoid dynamic memory allocation.
enum { inline_buffer_size = 500 };

/**
 * A dynamically growing memory buffer for trivially copyable/constructible
 * types with the first `SIZE` elements stored in the object itself. Most
 * commonly used via the `memory_buffer` alias for `char`.
 *
 * **Example**:
 *
 *     auto out = fmt::memory_buffer();
 *     fmt::format_to(std::back_inserter(out), "The answer is {}.", 42);
 *
 * This will append "The answer is 42." to `out`. The buffer content can be
 * converted to `std::string` with `to_string(out)`.
 */
template <typename T, size_t SIZE = inline_buffer_size,
          typename Allocator = detail::allocator<T>>
class basic_memory_buffer : public detail::buffer<T> {
 private:
  T store_[SIZE];

  // Don't inherit from Allocator to avoid generating type_info for it.
  FMT_NO_UNIQUE_ADDRESS Allocator alloc_;

  // Deallocate memory allocated by the buffer.
  FMT_CONSTEXPR20 void deallocate() {
    throw std::runtime_error("STUB: not implemented");
}

  static FMT_CONSTEXPR20 void grow(detail::buffer<T>& buf, size_t size) {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  using value_type = T;
  using const_reference = const T&;

  FMT_CONSTEXPR explicit basic_memory_buffer(
      const Allocator& alloc = Allocator())
      : detail::buffer<T>(grow), alloc_(alloc) {
    return {};
}
  FMT_CONSTEXPR20 ~basic_memory_buffer() { deallocate(); }

 private:
  template <typename Alloc = Allocator,
            FMT_ENABLE_IF(std::allocator_traits<Alloc>::
                              propagate_on_container_move_assignment::value)>
  FMT_CONSTEXPR20 auto move_alloc(basic_memory_buffer& other) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  // If the allocator does not propagate then copy the data from other.
  template <typename Alloc = Allocator,
            FMT_ENABLE_IF(!std::allocator_traits<Alloc>::
                              propagate_on_container_move_assignment::value)>
  FMT_CONSTEXPR20 auto move_alloc(basic_memory_buffer& other) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

  // Move data from other to this buffer.
  FMT_CONSTEXPR20 void move(basic_memory_buffer& other) {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  /// Constructs a `basic_memory_buffer` object moving the content of the other
  /// object to it.
  FMT_CONSTEXPR20 basic_memory_buffer(basic_memory_buffer&& other) noexcept
      : detail::buffer<T>(grow) {
    move(other);
  }

  /// Moves the content of the other `basic_memory_buffer` object to this one.
  auto operator=(basic_memory_buffer&& other) noexcept -> basic_memory_buffer& {
    abort();
}

  // Returns a copy of the allocator associated with this buffer.
  auto get_allocator() const -> Allocator {
    throw std::runtime_error("STUB: not implemented");
}

  /// Resizes the buffer to contain `count` elements. If T is a POD type new
  /// elements may not be initialized.
  FMT_CONSTEXPR void resize(size_t count) {
    return {};
}

  /// Increases the buffer capacity to `new_capacity`.
  void reserve(size_t new_capacity) {
    throw std::runtime_error("STUB: not implemented");
}

  using detail::buffer<T>::append;
  template <typename ContiguousRange>
  FMT_CONSTEXPR20 void append(const ContiguousRange& range) {
    throw std::runtime_error("STUB: not implemented");
}
};

using memory_buffer = basic_memory_buffer<char>;

template <size_t SIZE>
FMT_NODISCARD auto to_string(const basic_memory_buffer<char, SIZE>& buf)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

// A writer to a buffered stream. It doesn't own the underlying stream.
class writer {
 private:
  detail::buffer<char>* buf_;

  // We cannot create a file buffer in advance because any write to a FILE may
  // invalidate it.
  FILE* file_;

 public:
  inline writer(FILE* f) : buf_(nullptr), file_(f) {
    throw std::runtime_error("STUB: not implemented");
}
  inline writer(detail::buffer<char>& buf) : buf_(&buf) {
    throw std::runtime_error("STUB: not implemented");
}

  /// Formats `args` according to specifications in `fmt` and writes the
  /// output to the file.
  template <typename... T> void print(format_string<T...> fmt, T&&... args) {
    throw std::runtime_error("STUB: not implemented");
}
};

class string_buffer {
 private:
  std::string str_;
  detail::container_buffer<std::string> buf_;

 public:
  inline string_buffer() : buf_(str_) {
    throw std::runtime_error("STUB: not implemented");
}

  inline operator writer() {
    throw std::runtime_error("STUB: not implemented");
}
  inline auto str() -> std::string& {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename T, size_t SIZE, typename Allocator>
struct is_contiguous<basic_memory_buffer<T, SIZE, Allocator>> : std::true_type {
};

// Suppress a misleading warning in older versions of clang.
FMT_PRAGMA_CLANG(diagnostic ignored "-Wweak-vtables")

/// An error reported from a formatting function.
class FMT_SO_VISIBILITY("default") format_error : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class loc_value;

FMT_END_EXPORT
namespace detail {
FMT_API auto write_console(int fd, string_view text) -> bool;
FMT_API void print(FILE*, string_view);
}  // namespace detail

namespace detail {
template <typename Char, size_t N> struct fixed_string {
  FMT_CONSTEXPR fixed_string(const Char (&s)[N]) {
    return {};
}
  Char data[N] = {};
};

// Converts a compile-time string to basic_string_view.
FMT_EXPORT template <typename Char, size_t N>
constexpr auto compile_string_to_view(const Char (&s)[N])
    -> basic_string_view<Char> {
    return {};
}
FMT_EXPORT template <typename Char>
constexpr auto compile_string_to_view(basic_string_view<Char> s)
    -> basic_string_view<Char> {
    return {};
}

// Returns true if value is negative, false otherwise.
// Same as `value < 0` but doesn't produce warnings if T is an unsigned type.
template <typename T, FMT_ENABLE_IF(is_signed<T>::value)>
constexpr auto is_negative(T value) -> bool {
    return {};
}
template <typename T, FMT_ENABLE_IF(!is_signed<T>::value)>
constexpr auto is_negative(T) -> bool {
    return {};
}

// Smallest of uint32_t, uint64_t, uint128_t that is large enough to
// represent all values of an integral type T.
template <typename T>
using uint32_or_64_or_128_t =
    conditional_t<num_bits<T>() <= 32 && !FMT_REDUCE_INT_INSTANTIATIONS,
                  uint32_t,
                  conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>>;
template <typename T>
using uint64_or_128_t = conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>;

#define FMT_POWERS_OF_10(factor)                                  \
  factor * 10, (factor) * 100, (factor) * 1000, (factor) * 10000, \
      (factor) * 100000, (factor) * 1000000, (factor) * 10000000, \
      (factor) * 100000000, (factor) * 1000000000

// Converts value in the range [0, 100) to a string. GCC generates a bit better
// code when value is pointer-size (https://www.godbolt.org/z/5fEPMT1cc).
inline auto digits2(size_t value) noexcept -> const char* {
    abort();
}

// Given i in [0, 100), let x be the first 7 digits after
// the decimal point of i / 100 in base 2, the first 2 bytes
// after digits2_i(x) is the string representation of i.
inline auto digits2_i(size_t value) noexcept -> const char* {
    abort();
}

template <typename Char> constexpr auto getsign(sign s) -> Char {
    return {};
}

template <typename T> FMT_CONSTEXPR auto count_digits_fallback(T n) -> int {
    return {};
}
#if FMT_USE_INT128
FMT_CONSTEXPR inline auto count_digits(native_uint128 n) -> int {
    return {};
}
#endif

#ifdef FMT_BUILTIN_CLZLL
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
inline auto do_count_digits(uint64_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}
#endif

// Returns the number of decimal digits in n. Leading zeros are not counted
// except for n == 0 in which case count_digits returns 1.
FMT_CONSTEXPR20 inline auto count_digits(uint64_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}

// Counts the number of digits in n. BITS = log2(radix).
template <int BITS, typename UInt>
FMT_CONSTEXPR auto count_digits(UInt n) -> int {
    return {};
}

#ifdef FMT_BUILTIN_CLZ
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
FMT_INLINE auto do_count_digits(uint32_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}
#endif

// Optional version of count_digits for better performance on 32-bit platforms.
FMT_CONSTEXPR20 inline auto count_digits(uint32_t n) -> int {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Int> constexpr auto digits10() noexcept -> int {
    return {};
}
template <> constexpr auto digits10<native_int128>() noexcept -> int {
    return {};
}
template <> constexpr auto digits10<uint128_t>() noexcept -> int { return 38; }

template <typename Char> struct thousands_sep_result {
  std::string grouping;
  Char thousands_sep;
};

template <typename Char>
FMT_API auto thousands_sep_impl(locale_ref loc) -> thousands_sep_result<Char>;
template <typename Char>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<Char> {
    throw std::runtime_error("STUB: not implemented");
}
template <>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<wchar_t> {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char>
FMT_API auto decimal_point_impl(locale_ref loc) -> Char;
template <typename Char> inline auto decimal_point(locale_ref loc) -> Char {
    throw std::runtime_error("STUB: not implemented");
}
template <> inline auto decimal_point(locale_ref loc) -> wchar_t {
    throw std::runtime_error("STUB: not implemented");
}

#ifndef FMT_HEADER_ONLY
FMT_BEGIN_EXPORT
extern template FMT_API auto thousands_sep_impl<char>(locale_ref)
    -> thousands_sep_result<char>;
extern template FMT_API auto thousands_sep_impl<wchar_t>(locale_ref)
    -> thousands_sep_result<wchar_t>;
extern template FMT_API auto decimal_point_impl(locale_ref) -> char;
extern template FMT_API auto decimal_point_impl(locale_ref) -> wchar_t;
FMT_END_EXPORT
#endif  // FMT_HEADER_ONLY

// Compares two characters for equality.
template <typename Char> auto equal2(const Char* lhs, const char* rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
inline auto equal2(const char* lhs, const char* rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

// Writes a two-digit value to out.
template <typename Char>
FMT_CONSTEXPR20 FMT_INLINE void write2digits(Char* out, size_t value) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char>
FMT_INLINE void write2digits_i(Char* out, size_t value) {
    throw std::runtime_error("STUB: not implemented");
}

// Formats a decimal unsigned integer value writing to out pointing to a buffer
// of specified size. The caller must ensure that the buffer is large enough.
template <typename Char, typename UInt>
FMT_CONSTEXPR20 auto do_format_decimal(Char* out, UInt value, int size)
    -> Char* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename UInt>
FMT_CONSTEXPR FMT_INLINE auto format_decimal(Char* out, UInt value,
                                             int num_digits) -> Char* {
    return {};
}

template <typename Char, typename UInt, typename OutputIt,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<OutputIt>>::value)>
FMT_CONSTEXPR auto format_decimal(OutputIt out, UInt value, int num_digits)
    -> OutputIt {
    return {};
}

template <typename Char, typename UInt>
FMT_CONSTEXPR auto do_format_base2e(int base_bits, Char* out, UInt value,
                                    int size, bool upper = false) -> Char* {
    return {};
}

// Formats an unsigned integer in the power of two base (binary, octal, hex).
template <typename Char, typename UInt>
FMT_CONSTEXPR auto format_base2e(int base_bits, Char* out, UInt value,
                                 int num_digits, bool upper = false) -> Char* {
    return {};
}

template <typename Char, typename OutputIt, typename UInt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value)>
FMT_CONSTEXPR inline auto format_base2e(int base_bits, OutputIt out, UInt value,
                                        int num_digits, bool upper = false)
    -> OutputIt {
    return {};
}

// A converter from UTF-8 to UTF-16.
class utf8_to_utf16 {
 private:
  basic_memory_buffer<wchar_t> buffer_;

 public:
  FMT_API explicit utf8_to_utf16(string_view s);
  inline operator basic_string_view<wchar_t>() const {
    throw std::runtime_error("STUB: not implemented");
}
  inline auto size() const -> size_t {
    throw std::runtime_error("STUB: not implemented");
}
  inline auto c_str() const -> const wchar_t* {
    throw std::runtime_error("STUB: not implemented");
}
  inline auto str() const -> std::wstring {
    throw std::runtime_error("STUB: not implemented");
}
};

enum class to_utf8_error_policy { abort, replace, wtf };

inline void to_utf8_3bytes(buffer<char>& buf, uint32_t cp) {
    throw std::runtime_error("STUB: not implemented");
}

// A converter from UTF-16/UTF-32 (host endian) to UTF-8.
template <typename WChar, typename Buffer = memory_buffer> class to_utf8 {
 private:
  Buffer buffer_;

 public:
  to_utf8() {
    throw std::runtime_error("STUB: not implemented");
}
  explicit to_utf8(basic_string_view<WChar> s,
                   to_utf8_error_policy policy = to_utf8_error_policy::abort) {
    throw std::runtime_error("STUB: not implemented");
}
  operator string_view() const {
    throw std::runtime_error("STUB: not implemented");
}
  auto size() const -> size_t {
    throw std::runtime_error("STUB: not implemented");
}
  auto c_str() const -> const char* {
    throw std::runtime_error("STUB: not implemented");
}
  auto str() const -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

  // Performs conversion returning a bool instead of throwing exception on
  // conversion error. This method may still throw in case of memory allocation
  // error.
  auto convert(basic_string_view<WChar> s,
               to_utf8_error_policy policy = to_utf8_error_policy::abort)
      -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  static auto convert(Buffer& buf, basic_string_view<WChar> s,
                      to_utf8_error_policy policy = to_utf8_error_policy::abort)
      -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

// Computes 128-bit result of multiplication of two 64-bit unsigned integers.
FMT_INLINE auto umul128(uint64_t x, uint64_t y) noexcept -> uint128 {
    abort();
}

namespace dragonbox {
// Computes floor(log10(pow(2, e))) for e in [-2620, 2620] using the method from
// https://fmt.dev/papers/Dragonbox.pdf#page=28, section 6.1.
inline auto floor_log10_pow2(int e) noexcept -> int {
    abort();
}

inline auto floor_log2_pow10(int e) noexcept -> int {
    abort();
}

// Computes upper 64 bits of multiplication of two 64-bit unsigned integers.
inline auto umul128_upper64(uint64_t x, uint64_t y) noexcept -> uint64_t {
    abort();
}

// Computes upper 128 bits of multiplication of a 64-bit unsigned integer and a
// 128-bit unsigned integer.
inline auto umul192_upper128(uint64_t x, uint128 y) noexcept -> uint128 {
    abort();
}

FMT_API auto get_cached_power(int k) noexcept -> uint128;

// Type-specific information that Dragonbox uses.
template <typename T, typename Enable = void> struct float_info;

template <> struct float_info<float> {
  using carrier_uint = uint32_t;
  static constexpr int exponent_bits = 8;
  static constexpr int kappa = 1;
  static constexpr int big_divisor = 100;
  static constexpr int small_divisor = 10;
  static constexpr int min_k = -31;
  enum { max_k = 46 };
  static constexpr int shorter_interval_tie_lower_threshold = -35;
  static constexpr int shorter_interval_tie_upper_threshold = -35;
};

template <> struct float_info<double> {
  using carrier_uint = uint64_t;
  static constexpr int exponent_bits = 11;
  static constexpr int kappa = 2;
  static constexpr int big_divisor = 1000;
  static constexpr int small_divisor = 100;
  static constexpr int min_k = -292;
  enum { max_k = 341 };
  static constexpr int shorter_interval_tie_lower_threshold = -77;
  static constexpr int shorter_interval_tie_upper_threshold = -77;
};

// An 80- or 128-bit floating point number.
template <typename T>
struct float_info<T, enable_if_t<std::numeric_limits<T>::digits == 64 ||
                                 std::numeric_limits<T>::digits == 113 ||
                                 is_float128<T>::value>> {
  using carrier_uint = detail::uint128_t;
  static const int exponent_bits = 15;
};

// A double-double floating point number.
template <typename T>
struct float_info<T, enable_if_t<is_double_double<T>::value>> {
  using carrier_uint = detail::uint128_t;
};

template <typename T> struct decimal_fp {
  using significand_type = typename float_info<T>::carrier_uint;
  significand_type significand;
  int exponent;
};

template <typename T> FMT_API auto to_decimal(T x) noexcept -> decimal_fp<T>;
}  // namespace dragonbox

// Returns true iff Float has the implicit bit which is not stored.
template <typename Float> constexpr auto has_implicit_bit() -> bool {
    return {};
}

// Returns the number of significand bits stored in Float. The implicit bit is
// not counted since it is not stored.
template <typename Float> constexpr auto num_significand_bits() -> int {
    return {};
}

template <typename Float>
constexpr auto exponent_mask() ->
    typename dragonbox::float_info<Float>::carrier_uint {
    return {};
}
template <typename Float> constexpr auto exponent_bias() -> int {
    return {};
}

FMT_CONSTEXPR inline auto compute_exp_size(int exp) -> int {
    return {};
}

// Writes the exponent exp in the form "[+-]d{2,3}" to buffer.
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_exponent(int exp, OutputIt out) -> OutputIt {
    return {};
}

// A floating-point number f * pow(2, e) where F is an unsigned type.
template <typename F> struct basic_fp {
  F f;
  int e;

  static constexpr int num_significand_bits =
      static_cast<int>(sizeof(F) * num_bits<unsigned char>());

  constexpr basic_fp() : f(0), e(0) {
    return {};
}
  constexpr basic_fp(uint64_t f_val, int e_val) : f(f_val), e(e_val) {
    return {};
}

  // Constructs fp from an IEEE754 floating-point number.
  template <typename Float> FMT_CONSTEXPR basic_fp(Float n) {
    return {};
}

  // Assigns n to this and return true iff predecessor is closer than successor.
  template <typename Float, FMT_ENABLE_IF(!is_double_double<Float>::value)>
  FMT_CONSTEXPR auto assign(Float n) -> bool {
    return {};
}

  template <typename Float, FMT_ENABLE_IF(is_double_double<Float>::value)>
  FMT_CONSTEXPR auto assign(Float n) -> bool {
    return {};
}
};

using fp = basic_fp<ullong>;

// Normalizes the value converted from double and multiplied by (1 << SHIFT).
template <int SHIFT = 0, typename F>
FMT_CONSTEXPR auto normalize(basic_fp<F> value) -> basic_fp<F> {
    return {};
}

// Computes lhs * rhs / pow(2, 64) rounded to nearest with half-up tie breaking.
FMT_CONSTEXPR inline auto multiply(uint64_t lhs, uint64_t rhs) -> uint64_t {
    return {};
}

FMT_CONSTEXPR inline auto operator*(fp x, fp y) -> fp {
    return {};
}

template <typename T, bool doublish = num_bits<T>() == num_bits<double>()>
using convert_float_result =
    conditional_t<std::is_same<T, float>::value || doublish, double, T>;

template <typename T>
constexpr auto convert_float(T value) -> convert_float_result<T> {
    return {};
}

template <bool C, typename T, typename F, FMT_ENABLE_IF(C)>
auto select(T true_value, F) -> T {
    throw std::runtime_error("STUB: not implemented");
}
template <bool C, typename T, typename F, FMT_ENABLE_IF(!C)>
auto select(T, F false_value) -> F {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR FMT_NOINLINE auto fill(OutputIt it, size_t n,
                                     const basic_specs& specs) -> OutputIt {
    return {};
}

// Writes the output of f, padded according to format specifications in specs.
// size: output size in code units.
// width: output display width in (terminal) column positions.
template <typename Char, align default_align = align::left, typename OutputIt,
          typename F>
FMT_CONSTEXPR auto write_padded(OutputIt out, const format_specs& specs,
                                size_t size, size_t width, F&& f) -> OutputIt {
    return {};
}

template <typename Char, align default_align = align::left, typename OutputIt,
          typename F>
constexpr auto write_padded(OutputIt out, const format_specs& specs,
                            size_t size, F&& f) -> OutputIt {
    return {};
}

template <typename Char, align default_align = align::left, typename OutputIt>
FMT_CONSTEXPR auto write_bytes(OutputIt out, string_view bytes,
                               const format_specs& specs = {}) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt, typename UIntPtr>
auto write_ptr(OutputIt out, UIntPtr value, const format_specs* specs)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

// Returns true iff the code point cp is printable.
FMT_API auto is_printable(uint32_t cp) -> bool;

inline auto needs_escape(uint32_t cp) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char> struct find_escape_result {
  const Char* begin;
  const Char* end;
  uint32_t cp;
};

template <typename Char>
auto find_escape(const Char* begin, const Char* end)
    -> find_escape_result<Char> {
    throw std::runtime_error("STUB: not implemented");
}

inline auto find_escape(const char* begin, const char* end)
    -> find_escape_result<char> {
    throw std::runtime_error("STUB: not implemented");
}

template <size_t width, typename Char, typename OutputIt>
auto write_codepoint(OutputIt out, char prefix, uint32_t cp) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt, typename Char>
auto write_escaped_cp(OutputIt out, const find_escape_result<Char>& escape)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
auto write_escaped_string(OutputIt out, basic_string_view<Char> str)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
auto write_escaped_char(OutputIt out, Char v) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_char(OutputIt out, Char value,
                              const format_specs& specs) -> OutputIt {
    return {};
}

template <typename Char> class digit_grouping {
 private:
  std::string grouping_;
  std::basic_string<Char> thousands_sep_;

  struct next_state {
    std::string::const_iterator group;
    int pos;
  };
  auto initial_state() const -> next_state {
    throw std::runtime_error("STUB: not implemented");
}

  // Returns the next digit group separator position.
  auto next(next_state& state) const -> int {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  explicit digit_grouping(locale_ref loc, bool localized = true) {
    throw std::runtime_error("STUB: not implemented");
}
  digit_grouping(std::string grouping, std::basic_string<Char> sep)
      : grouping_(std::move(grouping)), thousands_sep_(std::move(sep)) {
    throw std::runtime_error("STUB: not implemented");
}

  auto has_separator() const -> bool {
    throw std::runtime_error("STUB: not implemented");
}

  auto count_separators(int num_digits) const -> int {
    throw std::runtime_error("STUB: not implemented");
}

  // Applies grouping to digits and writes the output to out.
  template <typename Out, typename C>
  auto apply(Out out, basic_string_view<C> digits) const -> Out {
    throw std::runtime_error("STUB: not implemented");
}
};

FMT_CONSTEXPR inline void prefix_append(unsigned& prefix, unsigned value) {
    return {};
}

// Writes a decimal integer with digit grouping.
template <typename OutputIt, typename UInt, typename Char>
auto write_int(OutputIt out, UInt value, unsigned prefix,
               const format_specs& specs, const digit_grouping<Char>& grouping)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

#if FMT_USE_LOCALE
// Writes a localized value.
FMT_API auto write_loc(appender out, loc_value value, const format_specs& specs,
                       locale_ref loc) -> bool;
auto write_loc(basic_appender<wchar_t> out, loc_value value,
               const format_specs& specs, locale_ref loc) -> bool;
#endif
template <typename OutputIt>
inline auto write_loc(OutputIt, const loc_value&, const format_specs&,
                      locale_ref) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

template <typename UInt> struct write_int_arg {
  UInt abs_value;
  unsigned prefix;
};

template <typename T>
FMT_CONSTEXPR auto make_write_int_arg(T value, sign s)
    -> write_int_arg<uint32_or_64_or_128_t<T>> {
    return {};
}

template <typename Char = char> struct loc_writer {
  basic_appender<Char> out;
  const format_specs& specs;
  std::basic_string<Char> sep;
  std::string grouping;
  std::basic_string<Char> decimal_point;

  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  auto operator()(T value) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  auto operator()(T) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

// Size and padding computation separate from write_int to avoid template bloat.
struct size_padding {
  unsigned size;
  unsigned padding;

  FMT_CONSTEXPR size_padding(int num_digits, unsigned prefix,
                             const format_specs& specs)
      : size((prefix >> 24) + to_unsigned(num_digits)), padding(0) {
    return {};
}
};

template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_INLINE auto write_int(OutputIt out, write_int_arg<T> arg,
                                        const format_specs& specs) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_NOINLINE auto write_int_noinline(OutputIt out,
                                                   write_int_arg<T> arg,
                                                   const format_specs& specs)
    -> OutputIt {
    return {};
}

template <typename Char, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value)>
FMT_CONSTEXPR FMT_INLINE auto write(basic_appender<Char> out, T value,
                                    const format_specs& specs, locale_ref loc)
    -> basic_appender<Char> {
    return {};
}

// An inlined version of write used in format string compilation.
template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<{
    return {};
}&
                        !std::is_same<OutputIt, basic_appender<{
    return {};
}PR FMT_INLINE auto write(OutputIt out, T value,
                    {
    return {};
}t_specs& specs, locale_ref loc)
    -> OutputIt {
    return {};
}

te{
    return {};
}& specs,
                         locale_ref loc = {}) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(std::is_same<Char, char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs, locale_ref) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, const Char* s, const format_specs& specs,
                         locale_ref) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
    return {};
}

template <typename Char>
FMT_CONSTEXPR auto parse_align(const Char* begin, const Char* end,
                               format_specs& specs) -> const Char* {
    return {};
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR20 auto write_nonfinite(OutputIt out, bool isnan,
                                     format_specs specs, sign s) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

// A decimal floating-point number significand * pow(10, exp).
struct big_decimal_fp {
  const char* significand;
  int significand_size;
  int exponent;
};

constexpr auto get_significand_size(const big_decimal_fp& f) -> int {
    return {};
}
template <typename T>
inline auto get_significand_size(const dragonbox::decimal_fp<T>& f) -> int {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
constexpr auto write_significand(OutputIt out, const char* significand,
                                 int significand_size) -> OutputIt {
    return {};
}
template <typename Char, typename OutputIt, typename UInt>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}
template <typename Char, typename OutputIt, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int exponent,
                                       const Grouping& grouping) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename UInt,
          FMT_ENABLE_IF(std::is_integral<UInt>::value)>
inline auto write_significand(Char* out, UInt significand, int significand_size,
                              int integral_size, Char decimal_point) -> Char* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt, typename UInt, typename Char,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<OutputIt>>::value)>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size, int integral_size,
                              Char decimal_point) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt, typename Char>
FMT_CONSTEXPR auto write_significand(OutputIt out, const char* significand,
                                     int significand_size, int integral_size,
                                     Char decimal_point) -> OutputIt {
    return {};
}

template <typename OutputIt, typename Char, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int integral_size,
                                       Char decimal_point,
                                       const Grouping& grouping) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

// Numbers with exponents greater or equal to the returned value will use
// the exponential notation.
template <typename T> FMT_CONSTEVAL auto exp_upper() -> int {
    throw std::runtime_error("STUB: not implemented");
}

// Use the fixed notation if the exponent is in [-4, exp_upper),
// e.g. 0.0001 instead of 1e-04. Otherwise use the exponent notation.
constexpr auto use_fixed(int exp, int exp_upper) -> bool {
    return {};
}

template <typename Char> class fallback_digit_grouping {
 public:
  constexpr fallback_digit_grouping(locale_ref, bool) {
    return {};
}

  constexpr auto has_separator() const -> bool {
    return {};
}

  constexpr auto count_separators(int) const -> int {
    return {};
}

  template <typename Out, typename C>
  constexpr auto apply(Out out, basic_string_view<C>) const -> Out {
    return {};
}
};

template <typename Char, typename Grouping, typename OutputIt,
          typename DecimalFP>
FMT_CONSTEXPR20 auto write_fixed(OutputIt out, const DecimalFP& f,
                                 int significand_size, Char decimal_point,
                                 const format_specs& specs, sign s,
                                 locale_ref loc = {}) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename Grouping, typename OutputIt,
          typename DecimalFP>
FMT_CONSTEXPR20 auto do_write_float(OutputIt out, const DecimalFP& f,
                                    const format_specs& specs, sign s,
                                    int exp_upper, locale_ref loc) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename DecimalFP>
FMT_CONSTEXPR20 auto write_float(OutputIt out, const DecimalFP& f,
                                 const format_specs& specs, sign s,
                                 int exp_upper, locale_ref loc) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T> constexpr auto isnan(T value) -> bool {
    return {};
}

template <typename T, typename Enable = void>
struct has_isfinite : std::false_type {};

template <typename T>
struct has_isfinite<T, enable_if_t<sizeof(std::isfinite(T())) != 0>>
    : std::true_type {};

template <typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value&& has_isfinite<T>::value)>
FMT_CONSTEXPR20 auto isfinite(T value) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, FMT_ENABLE_IF(!has_isfinite<T>::value)>
FMT_CONSTEXPR auto isfinite(T value) -> bool {
    return {};
}

template <typename T, FMT_ENABLE_IF(is_floating_point<T>::value)>
FMT_INLINE FMT_CONSTEXPR auto signbit(T value) -> bool {
    return {};
}

inline FMT_CONSTEXPR20 void adjust_precision(int& precision, int exp10) {
    throw std::runtime_error("STUB: not implemented");
}

class bigint {
 private:
  // A bigint is a number in the form bigit_[N - 1] ... bigit_[0] * 32^exp_.
  using bigit = uint32_t;  // A big digit.
  using double_bigit = uint64_t;
  enum { bigit_bits = num_bits<bigit>() };
  enum { bigits_capacity = 32 };
  basic_memory_buffer<bigit, bigits_capacity> bigits_;
  int exp_;

  friend struct formatter<bigint>;

  FMT_CONSTEXPR auto get_bigit(int i) const -> bigit {
    return {};
}

  FMT_CONSTEXPR void subtract_bigits(int index, bigit other, bigit& borrow) {
    return {};
}

  FMT_CONSTEXPR void remove_leading_zeros() {
    return {};
}

  // Computes *this -= other assuming aligned bigints and *this >= other.
  FMT_CONSTEXPR void subtract_aligned(const bigint& other) {
    return {};
}

  FMT_CONSTEXPR void multiply(uint32_t value) {
    return {};
}

  template <typename UInt, FMT_ENABLE_IF(std::is_same<UInt, uint64_t>::value ||
                                         std::is_same<UInt, uint128_t>::value)>
  FMT_CONSTEXPR void multiply(UInt value) {
    return {};
}

  template <typename UInt, FMT_ENABLE_IF(std::is_same<UInt, uint64_t>::value ||
                                         std::is_same<UInt, uint128_t>::value)>
  FMT_CONSTEXPR void assign(UInt n) {
    return {};
}

 public:
  FMT_CONSTEXPR bigint() : exp_(0) {
    return {};
}
  explicit bigint(uint64_t n) {
    throw std::runtime_error("STUB: not implemented");
}

  bigint(const bigint&) = delete;
  void operator=(const bigint&) = delete;

  FMT_CONSTEXPR void assign(const bigint& other) {
    return {};
}

  template <typename Int> FMT_CONSTEXPR void operator=(Int n) {
    return {};
}

  FMT_CONSTEXPR auto num_bigits() const -> int {
    return {};
}

  FMT_CONSTEXPR auto operator<<=(int shift) -> bigint& {
    return {};
}

  template <typename Int> FMT_CONSTEXPR auto operator*=(Int value) -> bigint& {
    return {};
}

  friend FMT_CONSTEXPR auto compare(const bigint& b1, const bigint& b2) -> int {
    return {};
}

  // Returns compare(lhs1 + lhs2, rhs).
  friend FMT_CONSTEXPR auto add_compare(const bigint& lhs1, const bigint& lhs2,
                                        const bigint& rhs) -> int {
    return {};
}

  // Assigns pow(10, exp) to this bigint.
  FMT_CONSTEXPR20 void assign_pow10(int exp) {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_CONSTEXPR20 void square() {
    throw std::runtime_error("STUB: not implemented");
}

  // If this bigint has a bigger exponent than other, adds trailing zero to make
  // exponents equal. This simplifies some operations such as subtraction.
  FMT_CONSTEXPR void align(const bigint& other) {
    return {};
}

  // Divides this bignum by divisor, assigning the remainder to this and
  // returning the quotient.
  FMT_CONSTEXPR auto divmod_assign(const bigint& divisor) -> int {
    return {};
}
};

// format_dragon flags.
enum dragon {
  predecessor_closer = 1,
  fixup = 2,  // Run fixup to correct exp10 which can be off by one.
  fixed = 4,
};

// Formats a floating-point number using a variation of the Fixed-Precision
// Positive Floating-Point Printout ((FPP)^2) algorithm by Steele & White:
// https://fmt.dev/papers/p372-steele.pdf.
FMT_CONSTEXPR20 inline void format_dragon(basic_fp<uint128_t> value,
                                          unsigned flags, int num_digits,
                                          buffer<char>& buf, int& exp10) {
    throw std::runtime_error("STUB: not implemented");
}

// Formats a floating-point number using the hexfloat format.
template <typename Float, FMT_ENABLE_IF(!is_double_double<Float>::value)>
FMT_CONSTEXPR20 void format_hexfloat(Float value, format_specs specs,
                                     buffer<char>& buf) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Float, FMT_ENABLE_IF(is_double_double<Float>::value)>
FMT_CONSTEXPR20 void format_hexfloat(Float value, format_specs specs,
                                     buffer<char>& buf) {
    throw std::runtime_error("STUB: not implemented");
}

constexpr auto fractional_part_rounding_thresholds(int index) -> uint32_t {
    return {};
}

template <typename Float>
FMT_CONSTEXPR20 auto format_float(Float value, int precision,
                                  const format_specs& specs, bool binary32,
                                  buffer<char>& buf) -> int {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value, format_specs specs,
                           locale_ref loc = {}) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_fast_float<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value &&
                        !is_fast_float<T>::value)>
inline auto write(OutputIt out, T value) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
auto write(OutputIt out, monostate, format_specs = {}, locale_ref = {})
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> value)
    -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(has_to_string_view<T>::value)>
constexpr auto write(OutputIt out, const T& value) -> OutputIt {
    return {};
}

// FMT_ENABLE_IF() condition separated to workaround an MSVC bug.
template <
    typename Char, typename OutputIt, typename T,
    bool check = std::is_enum<T>::value && !std::is_same<T, Char>::value &&
                 mapped_type_constant<T, Char>::value != type::custom_type,
    FMT_ENABLE_IF(check)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, bool>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value, const format_specs& specs = {},
                         locale_ref = {}) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, Char value) -> OutputIt {
    return {};
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR20 auto write(OutputIt out, const Char* value) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, void>::value)>
auto write(OutputIt out, const T* value, const format_specs& specs = {},
           locale_ref = {}) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(mapped_type_constant<T, Char>::value ==
                            type::custom_type &&
                        !std::is_fundamental<T>::value)>
FMT_CONSTEXPR auto write(OutputIt out, const T& value) -> OutputIt {
    return {};
}

template <typename T>
using is_builtin =
    bool_constant<std::is_same<T, int>::value || FMT_BUILTIN_TYPES>;

// An argument visitor that formats the argument and writes it via the output
// iterator. It's a class and not a generic lambda for compatibility with C++11.
template <typename Char> struct default_arg_formatter {
  using context = buffered_context<Char>;

  basic_appender<Char> out;

  void operator()(monostate) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(is_builtin<T>::value)>
  void operator()(T value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!is_builtin<T>::value)>
  void operator()(T) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(typename basic_format_arg<context>::handle h) {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename Char> struct arg_formatter {
  basic_appender<Char> out;
  const format_specs& specs;
  FMT_NO_UNIQUE_ADDRESS locale_ref locale;

  template <typename T, FMT_ENABLE_IF(is_builtin<T>::value)>
  FMT_CONSTEXPR FMT_INLINE void operator()(T value) {
    return {};
}

  template <typename T, FMT_ENABLE_IF(!is_builtin<T>::value)>
  void operator()(T) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(typename basic_format_arg<buffered_context<Char>>::handle) {
    throw std::runtime_error("STUB: not implemented");
}
};

struct dynamic_spec_getter {
  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T value) -> ullong {
    return {};
}

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T) -> ullong {
    return {};
}
};

template <typename Context>
FMT_CONSTEXPR void handle_dynamic_spec(
    arg_id_kind kind, int& value,
    const arg_ref<typename Context::char_type>& ref, Context& ctx) {
    return {};
}

#if FMT_USE_NONTYPE_TEMPLATE_ARGS
template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct static_named_arg : view {
  static constexpr auto name = Str.data;

  const T& value;
  static_named_arg(const T& v) : value(v) {}
};

template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct is_named_arg<static_named_arg<T, Char, N, Str>> : std::true_type {};

template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct is_static_named_arg<static_named_arg<T, Char, N, Str>> : std::true_type {
};

template <typename Char, size_t N, fmt::detail::fixed_string<Char, N> Str>
struct udl_arg {
  template <typename T> auto operator=(T&& value) const {
    return static_named_arg<T, Char, N, Str>(std::forward<T>(value));
  }
};
#else
template <typename Char> struct udl_arg {
  const Char* str;

  template <typename T> auto operator=(T&& value) const -> named_arg<T, Char> {
    throw std::runtime_error("STUB: not implemented");
}
};
#endif  // FMT_USE_NONTYPE_TEMPLATE_ARGS

template <typename Char = char> struct format_handler {
  parse_context<Char> parse_ctx;
  buffered_context<Char> ctx;

  void on_text(const Char* begin, const Char* end) {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_CONSTEXPR auto on_arg_id() -> int {
    return {};
}
  FMT_CONSTEXPR auto on_arg_id(int id) -> int {
    return {};
}
  FMT_CONSTEXPR auto on_arg_id(basic_string_view<Char> id) -> int {
    return {};
}

  FMT_INLINE void on_replacement_field(int id, const Char*) {
    throw std::runtime_error("STUB: not implemented");
}

  auto on_format_specs(int id, const Char* begin, const Char* end)
      -> const Char* {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_NORETURN void on_error(const char* message) {
    throw std::runtime_error("STUB: not implemented");
}
};

// It is used in format-inl.h and os.cc.
using format_func = void (*)(detail::buffer<char>&, int, const char*);
FMT_API void do_report_error(format_func func, int error_code,
                             const char* message) noexcept;

FMT_API void format_error_code(buffer<char>& out, int error_code,
                               string_view message) noexcept;

template <typename T, typename Char, type TYPE>
template <typename FormatContext>
FMT_CONSTEXPR auto native_formatter<T, Char, TYPE>::format(
    const T& val, FormatContext& ctx) const -> decltype(ctx.out()) {
  if (!specs_.dynamic())
    return write<Char>(ctx.out(), val, specs_, ctx.locale());
  auto specs = format_specs(specs_);
  handle_dynamic_spec(specs.dynamic_width(), specs.width, specs_.width_ref,
                      ctx);
  handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                      specs_.precision_ref, ctx);
  return write<Char>(ctx.out(), val, specs, ctx.locale());
}
}  // namespace detail

FMT_BEGIN_EXPORT

// A generic formatting context with custom output iterator and character
// (code unit) support. Char is the format string code unit type which can be
// different from OutputIt::value_type.
template <typename OutputIt, typename Char> class generic_context {
 private:
  OutputIt out_;
  basic_format_args<generic_context> args_;
  locale_ref loc_;

 public:
  using char_type = Char;
  using iterator = OutputIt;
  enum { builtin_types = FMT_BUILTIN_TYPES };

  constexpr generic_context(OutputIt out,
                            basic_format_args<generic_context> args,
                            locale_ref loc = {})
      : out_(out), args_(args), loc_(loc) {
    return {};
}
  generic_context(generic_context&&) = default;
  generic_context(const generic_context&) = delete;
  void operator=(const generic_context&) = delete;

  constexpr auto arg(int id) const -> basic_format_arg<generic_context> {
    return {};
}
  auto arg(basic_string_view<Char> name) const
      -> basic_format_arg<generic_context> {
    throw std::runtime_error("STUB: not implemented");
}
  constexpr auto arg_id(basic_string_view<Char> name) const -> int {
    return {};
}

  constexpr auto out() const -> iterator {
    return {};
}

  FMT_CONSTEXPR void advance_to(iterator it) {
    return {};
}

  constexpr auto locale() const -> locale_ref {
    return {};
}
};

class loc_value {
 private:
  basic_format_arg<context> value_;

 public:
  template <typename T, FMT_ENABLE_IF(!detail::is_float128<T>::value)>
  loc_value(T value) : value_(value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(detail::is_float128<T>::value)>
  loc_value(T) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Visitor> auto visit(Visitor&& vis) -> decltype(vis(0)) {
    throw std::runtime_error("STUB: not implemented");
}
};

// A locale facet that formats values in UTF-8.
// It is parameterized on the locale to avoid the heavy <locale> include.
template <typename Locale> class format_facet : public Locale::facet {
 private:
  std::string separator_;
  std::string grouping_;
  std::string decimal_point_;

 protected:
  virtual auto do_put(appender out, loc_value val,
                      const format_specs& specs) const -> bool;

 public:
  static FMT_API typename Locale::id id;

  explicit format_facet(Locale& loc);
  explicit format_facet(string_view sep = "", std::string grouping = "\3",
                        std::string decimal_point = ".")
      : separator_(sep.data(), sep.size()),
        grouping_(std::move(grouping)),
        decimal_point_(std::move(decimal_point)) {
    throw std::runtime_error("STUB: not implemented");
}

  auto put(appender out, loc_value val, const format_specs& specs) const
      -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

#define FMT_FORMAT_AS(Type, Base)                                   \
  template <typename Char>                                          \
  struct formatter<Type, Char> : formatter<Base, Char> {            \
    template <typename FormatContext>                               \
    FMT_CONSTEXPR auto format(Type value, FormatContext& ctx) const \
        -> decltype(ctx.out()) {                                    \
      return formatter<Base, Char>::format(value, ctx);             \
    }                                                               \
  }

FMT_FORMAT_AS(signed char, int);
FMT_FORMAT_AS(unsigned char, unsigned);
FMT_FORMAT_AS(short, int);
FMT_FORMAT_AS(unsigned short, unsigned);
FMT_FORMAT_AS(long, detail::long_type);
FMT_FORMAT_AS(unsigned long, detail::ulong_type);
FMT_FORMAT_AS(Char*, const Char*);
FMT_FORMAT_AS(detail::std_string_view<Char>, basic_string_view<Char>);
FMT_FORMAT_AS(std::nullptr_t, const void*);
FMT_FORMAT_AS(void*, const void*);

template <typename Char, size_t N>
struct formatter<Char[N], Char> : formatter<basic_string_view<Char>, Char> {};

template <typename Char, typename Traits, typename Allocator>
class formatter<std::basic_string<Char, Traits, Allocator>, Char>
    : public formatter<basic_string_view<Char>, Char> {};

template <typename Char>
struct formatter<detail::float128, Char>
    : detail::native_formatter<detail::float128, Char,
                               detail::type::float_type> {};

template <typename T, typename Char>
struct formatter<T, Char, void_t<detail::format_as_result<T>>>
    : formatter<detail::format_as_result<T>, Char> {
  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const T& value, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto&& val = format_as(value);  // Make an lvalue reference for format.
    return formatter<detail::format_as_result<T>, Char>::format(val, ctx);
  }
};

/**
 * Converts `p` to `const void*` for pointer formatting.
 *
 * **Example**:
 *
 *     auto s = fmt::format("{}", fmt::ptr(p));
 */
template <typename T> auto ptr(T p) -> const void* {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Converts `e` to the underlying type.
 *
 * **Example**:
 *
 *     enum class color { red, green, blue };
 *     auto s = fmt::format("{}", fmt::underlying(color::red));  // s == "0"
 */
template <typename Enum>
constexpr auto underlying(Enum e) noexcept -> underlying_t<Enum> {
    return {};
}

namespace enums {
template <typename Enum, FMT_ENABLE_IF(std::is_enum<Enum>::value)>
constexpr auto format_as(Enum e) noexcept -> underlying_t<Enum> {
    return {};
}
}  // namespace enums

struct bytes {
  string_view data;

  inline explicit bytes(string_view s) : data(s) {
    throw std::runtime_error("STUB: not implemented");
}
};

template <> struct formatter<bytes> {
 private:
  detail::dynamic_format_specs<> specs_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<>& ctx) -> const char* {
    return parse_format_specs(ctx.begin(), ctx.end(), specs_, ctx,
                              detail::type::string_type);
  }

  template <typename FormatContext>
  auto format(bytes b, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto specs = specs_;
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width,
                                specs.width_ref, ctx);
    detail::handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                                specs.precision_ref, ctx);
    return detail::write_bytes<char>(ctx.out(), b.data, specs);
  }
};

// group_digits_view is not derived from view because it copies the argument.
template <typename T> struct group_digits_view {
  T value;
};

/**
 * Returns a view that formats an integer value using ',' as a
 * locale-independent thousands separator.
 *
 * **Example**:
 *
 *     fmt::print("{}", fmt::group_digits(12345));
 *     // Output: "12,345"
 */
template <typename T> auto group_digits(T value) -> group_digits_view<T> {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T> struct formatter<group_digits_view<T>> : formatter<T> {
 private:
  detail::dynamic_format_specs<> specs_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<>& ctx) -> const char* {
    return parse_format_specs(ctx.begin(), ctx.end(), specs_, ctx,
                              detail::type::int_type);
  }

  template <typename FormatContext>
  auto format(group_digits_view<T> view, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto specs = specs_;
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width,
                                specs.width_ref, ctx);
    detail::handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                                specs.precision_ref, ctx);
    auto arg = detail::make_write_int_arg(view.value, specs.sign());
    return detail::write_int(
        ctx.out(), static_cast<detail::uint64_or_128_t<T>>(arg.abs_value),
        arg.prefix, specs, detail::digit_grouping<char>("\3", ","));
  }
};

template <typename T, typename Char> struct nested_view {
  const formatter<T, Char>* fmt;
  const T* value;
};

template <typename T, typename Char>
struct formatter<nested_view<T, Char>, Char> {
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(nested_view<T, Char> view, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return view.fmt->format(*view.value, ctx);
  }
};

template <typename T, typename Char = char> struct nested_formatter {
 private:
  basic_specs specs_;
  int width_;
  formatter<T, Char> formatter_;

 public:
  constexpr nested_formatter() : width_(0) {
    return {};
}

  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return {};
}

  template <typename FormatContext, typename F>
  auto write_padded(FormatContext& ctx, F write) const -> decltype(ctx.out()) {
    throw std::runtime_error("STUB: not implemented");
}

  auto nested(const T& value) const -> nested_view<T, Char> {
    throw std::runtime_error("STUB: not implemented");
}
};

inline namespace literals {
#if FMT_USE_NONTYPE_TEMPLATE_ARGS
/**
 * User-defined literal equivalent of `fmt::arg`, but with compile-time checks.
 *
 * **Example**:
 *
 *     using namespace fmt::literals;
 *     fmt::print("The answer is {answer}.", "answer"_a=42);
 */
template <detail::fixed_string S> constexpr auto operator""_a() {
  using char_t = remove_cvref_t<decltype(*S.data)>;
  return detail::udl_arg<char_t, sizeof(S.data) / sizeof(char_t), S>();
}
#else
/**
 * User-defined literal equivalent of `fmt::arg`.
 *
 * **Example**:
 *
 *     using namespace fmt::literals;
 *     fmt::print("The answer is {answer}.", "answer"_a=42);
 */
constexpr auto operator""_a(const char* s, size_t) -> detail::udl_arg<char> {
    return {};
}
#endif  // FMT_USE_NONTYPE_TEMPLATE_ARGS
}  // namespace literals

/// A fast integer formatter.
class format_int {
 private:
  // Buffer should be large enough to hold all digits (digits10 + 1),
  // a sign and a null character.
  enum { buffer_size = std::numeric_limits<ullong>::digits10 + 3 };
  mutable char buffer_[buffer_size];
  char* str_;

  template <typename UInt>
  FMT_CONSTEXPR20 auto format_unsigned(UInt value) -> char* {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Int>
  FMT_CONSTEXPR20 auto format_signed(Int value) -> char* {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  FMT_CONSTEXPR20 explicit format_int(int value) : str_(format_signed(value)) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR20 explicit format_int(long value)
      : str_(format_signed(value)) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR20 explicit format_int(long long value)
      : str_(format_signed(value)) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR20 explicit format_int(unsigned value)
      : str_(format_unsigned(value)) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR20 explicit format_int(unsigned long value)
      : str_(format_unsigned(value)) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR20 explicit format_int(ullong value)
      : str_(format_unsigned(value)) {
    throw std::runtime_error("STUB: not implemented");
}

  /// Returns the number of characters written to the output buffer.
  FMT_CONSTEXPR20 auto size() const -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

  /// Returns a pointer to the output buffer content. No terminating null
  /// character is appended.
  FMT_CONSTEXPR20 auto data() const -> const char* {
    throw std::runtime_error("STUB: not implemented");
}

  /// Returns a pointer to the output buffer content with terminating null
  /// character appended.
  FMT_CONSTEXPR20 auto c_str() const -> const char* {
    throw std::runtime_error("STUB: not implemented");
}

  /// Returns the content of the output buffer as an `std::string`.
  inline auto str() const -> std::string {
    throw std::runtime_error("STUB: not implemented");
}
};

#if FMT_CLANG_ANALYZER
#  define FMT_STRING_IMPL(s, base) s
#else
#  define FMT_STRING_IMPL(s, base)                                           \
    [] {                                                                     \
      /* Use the hidden visibility as a workaround for a GCC bug (#1973). */ \
      /* Use a macro-like name to avoid shadowing warnings. */               \
      struct FMT_VISIBILITY("hidden") FMT_COMPILE_STRING : base {            \
        using char_type = fmt::remove_cvref_t<decltype(s[0])>;               \
        constexpr explicit operator fmt::basic_string_view<char_type>()      \
            const {                                                          \
          return fmt::detail::compile_string_to_view<char_type>(s);          \
        }                                                                    \
      };                                                                     \
      using FMT_STRING_VIEW =                                                \
          fmt::basic_string_view<typename FMT_COMPILE_STRING::char_type>;    \
      fmt::detail::ignore_unused(FMT_STRING_VIEW(FMT_COMPILE_STRING()));     \
      return FMT_COMPILE_STRING();                                           \
    }()
#endif  // FMT_CLANG_ANALYZER

/**
 * Constructs a legacy compile-time format string from a string literal `s`.
 *
 * **Example**:
 *
 *     // A compile-time error because 'd' is an invalid specifier for strings.
 *     std::string s = fmt::format(FMT_STRING("{:d}"), "foo");
 */
#if FMT_USE_CONSTEVAL
#  define FMT_STRING(s) s
#else
#  define FMT_STRING(s) FMT_STRING_IMPL(s, fmt::detail::compile_string)
#endif  // FMT_USE_CONSTEVAL

FMT_API auto vsystem_error(int error_code, string_view fmt, format_args args)
    -> std::system_error;

/**
 * Constructs `std::system_error` with a message formatted with
 * `fmt::format(fmt, args...)`.
 * `error_code` is a system error code as given by `errno`.
 *
 * **Example**:
 *
 *     // This throws std::system_error with the description
 *     //   cannot open file 'madeup': No such file or directory
 *     // or similar (system message may vary).
 *     const char* filename = "madeup";
 *     FILE* file = fopen(filename, "r");
 *     if (!file)
 *       throw fmt::system_error(errno, "cannot open file '{}'", filename);
 */
template <typename... T>
auto system_error(int error_code, format_string<T...> fmt, T&&... args)
    -> std::system_error {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats an error message for an error returned by an operating system or a
 * language runtime, for example a file opening error, and writes it to `out`.
 * The format is the same as the one used by `std::system_error(ec, message)`
 * where `ec` is `std::error_code(error_code, std::generic_category())`.
 * It is implementation-defined but normally looks like:
 *
 *     <message>: <system-message>
 *
 * where `<message>` is the passed message and `<system-message>` is the system
 * message corresponding to the error code.
 * `error_code` is a system error code as given by `errno`.
 */
FMT_API void format_system_error(detail::buffer<char>& out, int error_code,
                                 const char* message) noexcept;

// Reports a system error without throwing an exception.
// Can be used to report errors from destructors.
FMT_API void report_system_error(int error_code, const char* message) noexcept;

inline auto vformat(locale_ref loc, string_view fmt, format_args args)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

template <typename... T>
FMT_INLINE auto format(locale_ref loc, format_string<T...> fmt, T&&... args)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to(OutputIt out, locale_ref loc, string_view fmt, format_args args)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
FMT_INLINE auto format_to(OutputIt out, locale_ref loc, format_string<T...> fmt,
                          T&&... args) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename... T>
FMT_NODISCARD FMT_INLINE auto formatted_size(locale_ref loc,
                                             format_string<T...> fmt,
                                             T&&... args) -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

FMT_API auto vformat(string_view fmt, format_args args) -> std::string;

/**
 * Formats `args` according to specifications in `fmt` and returns the result
 * as a string.
 *
 * **Example**:
 *
 *     #include <fmt/format.h>
 *     std::string message = fmt::format("The answer is {}.", 42);
 */
template <typename... T>
FMT_NODISCARD FMT_INLINE auto format(format_string<T...> fmt, T&&... args)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Converts `value` to `std::string` using the default format for type `T`.
 *
 * **Example**:
 *
 *     std::string answer = fmt::to_string(42);
 */
template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(T value) -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, FMT_ENABLE_IF(detail::use_format_as<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(const T& value)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value &&
                                    !detail::use_format_as<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(const T& value)
    -> std::string {
    throw std::runtime_error("STUB: not implemented");
}

FMT_END_EXPORT
FMT_END_NAMESPACE

#ifdef FMT_HEADER_ONLY
#  define FMT_FUNC inline
#  include "format-inl.h"
#endif

// Restore _LIBCPP_REMOVE_TRANSITIVE_INCLUDES.
#ifdef FMT_REMOVE_TRANSITIVE_INCLUDES
#  undef _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#endif

#endif  // FMT_FORMAT_H_
