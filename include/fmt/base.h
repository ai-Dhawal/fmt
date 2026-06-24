#include <stdexcept>
#include <cstdlib>
// Formatting library for C++ - the base API for char/UTF-8
//
// Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_BASE_H_
#define FMT_BASE_H_

#if defined(FMT_IMPORT_STD) && !defined(FMT_MODULE)
#  define FMT_MODULE
#endif

#ifndef FMT_MODULE
#  include <limits.h>  // CHAR_BIT
#  include <stdio.h>   // FILE
#  include <string.h>  // memcmp

#  include <type_traits>  // std::enable_if
#endif

// The fmt library version in the form major * 10000 + minor * 100 + patch.
#define FMT_VERSION 120200

// Detect compiler versions.
#if defined(__clang__) && !defined(__ibmxl__)
#  define FMT_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#  define FMT_CLANG_VERSION 0
#endif
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#  define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#  define FMT_GCC_VERSION 0
#endif
#if defined(__ICL)
#  define FMT_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#  define FMT_ICC_VERSION __INTEL_COMPILER
#else
#  define FMT_ICC_VERSION 0
#endif
#if defined(_MSC_VER)
#  define FMT_MSC_VERSION _MSC_VER
#else
#  define FMT_MSC_VERSION 0
#endif

// Detect standard library versions.
#ifdef _GLIBCXX_RELEASE
#  define FMT_GLIBCXX_RELEASE _GLIBCXX_RELEASE
#else
#  define FMT_GLIBCXX_RELEASE 0
#endif
#ifdef _LIBCPP_VERSION
#  define FMT_LIBCPP_VERSION _LIBCPP_VERSION
#else
#  define FMT_LIBCPP_VERSION 0
#endif

#ifdef _MSVC_LANG
#  define FMT_CPLUSPLUS _MSVC_LANG
#else
#  define FMT_CPLUSPLUS __cplusplus
#endif

// Detect __has_*.
#ifdef __has_feature
#  define FMT_HAS_FEATURE(x) __has_feature(x)
#else
#  define FMT_HAS_FEATURE(x) 0
#endif
#ifdef __has_include
#  define FMT_HAS_INCLUDE(x) __has_include(x)
#else
#  define FMT_HAS_INCLUDE(x) 0
#endif
#ifdef __has_builtin
#  define FMT_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define FMT_HAS_BUILTIN(x) 0
#endif
#ifdef __has_cpp_attribute
#  define FMT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define FMT_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define FMT_HAS_CPP14_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201402L && FMT_HAS_CPP_ATTRIBUTE(attribute))

#define FMT_HAS_CPP17_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201703L && FMT_HAS_CPP_ATTRIBUTE(attribute))

// Detect C++14 relaxed constexpr.
#ifdef FMT_USE_CONSTEXPR
// Use the provided definition.
#elif FMT_GCC_VERSION >= 702 && FMT_CPLUSPLUS >= 201402L
// GCC only allows constexpr member functions in non-literal types since 7.2:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66297.
#  define FMT_USE_CONSTEXPR 1
#elif FMT_ICC_VERSION
#  define FMT_USE_CONSTEXPR 0  // https://github.com/fmtlib/fmt/issues/1628
#elif FMT_HAS_FEATURE(cxx_relaxed_constexpr) || FMT_MSC_VERSION >= 1912
#  define FMT_USE_CONSTEXPR 1
#else
#  define FMT_USE_CONSTEXPR 0
#endif
#if FMT_USE_CONSTEXPR
#  define FMT_CONSTEXPR constexpr
#else
#  define FMT_CONSTEXPR
#endif

// Detect consteval, C++20 constexpr extensions and std::is_constant_evaluated.
#ifdef FMT_USE_CONSTEVAL
// Use the provided definition.
#elif !defined(__cpp_lib_is_constant_evaluated)
#  define FMT_USE_CONSTEVAL 0
#elif FMT_CPLUSPLUS < 201709L
#  define FMT_USE_CONSTEVAL 0
#elif FMT_GLIBCXX_RELEASE && FMT_GLIBCXX_RELEASE < 10
#  define FMT_USE_CONSTEVAL 0
#elif FMT_LIBCPP_VERSION && FMT_LIBCPP_VERSION < 10000
#  define FMT_USE_CONSTEVAL 0
#elif defined(__apple_build_version__) && __apple_build_version__ < 14000029L
#  define FMT_USE_CONSTEVAL 0  // consteval is broken in Apple clang < 14.
#elif FMT_MSC_VERSION && FMT_MSC_VERSION < 1940
#  define FMT_USE_CONSTEVAL 0  // consteval is broken in some MSVC2022 versions.
#elif defined(__cpp_consteval)
#  define FMT_USE_CONSTEVAL 1
#elif FMT_GCC_VERSION >= 1002 || FMT_CLANG_VERSION >= 1101
#  define FMT_USE_CONSTEVAL 1
#else
#  define FMT_USE_CONSTEVAL 0
#endif
#if FMT_USE_CONSTEVAL
#  define FMT_CONSTEVAL consteval
#  define FMT_CONSTEXPR20 constexpr
#else
#  define FMT_CONSTEVAL
#  define FMT_CONSTEXPR20
#endif

// Check if exceptions are disabled.
#ifdef FMT_USE_EXCEPTIONS
// Use the provided definition.
#elif defined(__GNUC__) && !defined(__EXCEPTIONS)
#  define FMT_USE_EXCEPTIONS 0
#elif defined(__clang__) && !defined(__cpp_exceptions)
#  define FMT_USE_EXCEPTIONS 0
#elif FMT_MSC_VERSION && !_HAS_EXCEPTIONS
#  define FMT_USE_EXCEPTIONS 0
#else
#  define FMT_USE_EXCEPTIONS 1
#endif
#if FMT_USE_EXCEPTIONS
#  define FMT_TRY try
#  define FMT_CATCH(x) catch (x)
#else
#  define FMT_TRY if (true)
#  define FMT_CATCH(x) if (false)
#endif

#ifdef FMT_NO_UNIQUE_ADDRESS
// Use the provided definition.
#elif FMT_CPLUSPLUS < 202002L
// Not supported.
#elif FMT_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define FMT_NO_UNIQUE_ADDRESS [[no_unique_address]]
// VS2019 v16.10 and later except clang-cl (https://reviews.llvm.org/D110485).
#elif FMT_MSC_VERSION >= 1929 && !FMT_CLANG_VERSION
#  define FMT_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#endif
#ifndef FMT_NO_UNIQUE_ADDRESS
#  define FMT_NO_UNIQUE_ADDRESS
#endif

#if FMT_HAS_CPP17_ATTRIBUTE(fallthrough)
#  define FMT_FALLTHROUGH [[fallthrough]]
#elif defined(__clang__)
#  define FMT_FALLTHROUGH [[clang::fallthrough]]
#elif FMT_GCC_VERSION >= 700 && \
    (!defined(__EDG_VERSION__) || __EDG_VERSION__ >= 520)
#  define FMT_FALLTHROUGH [[gnu::fallthrough]]
#else
#  define FMT_FALLTHROUGH
#endif

// Disable [[noreturn]] on MSVC/NVCC because of bogus unreachable code warnings.
#if FMT_HAS_CPP_ATTRIBUTE(noreturn) && !FMT_MSC_VERSION && !defined(__NVCC__)
#  define FMT_NORETURN [[noreturn]]
#else
#  define FMT_NORETURN
#endif

#ifdef FMT_NODISCARD
// Use the provided definition.
#elif FMT_HAS_CPP17_ATTRIBUTE(nodiscard)
#  define FMT_NODISCARD [[nodiscard]]
#else
#  define FMT_NODISCARD
#endif

#if FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_VISIBILITY(value) __attribute__((visibility(value)))
#else
#  define FMT_VISIBILITY(value)
#endif

// Detect pragmas.
#define FMT_PRAGMA_IMPL(x) _Pragma(#x)
#if FMT_GCC_VERSION >= 504 && !defined(__NVCOMPILER)
// Workaround a _Pragma bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59884
// and an nvhpc warning: https://github.com/fmtlib/fmt/pull/2582.
#  define FMT_PRAGMA_GCC(x) FMT_PRAGMA_IMPL(GCC x)
#else
#  define FMT_PRAGMA_GCC(x)
#endif
#if FMT_CLANG_VERSION
#  define FMT_PRAGMA_CLANG(x) FMT_PRAGMA_IMPL(clang x)
#else
#  define FMT_PRAGMA_CLANG(x)
#endif

#ifndef FMT_USE_OPTIMIZE_PRAGMA
#  define FMT_USE_OPTIMIZE_PRAGMA 1
#endif

// Enable minimal optimizations for more compact code in debug mode.
FMT_PRAGMA_GCC(push_options)
#if FMT_USE_OPTIMIZE_PRAGMA && !defined(__OPTIMIZE__) && \
    !defined(__CUDACC__) && !defined(FMT_MODULE)
FMT_PRAGMA_GCC(optimize("Og"))
#endif

#ifdef FMT_DEPRECATED
// Use the provided definition.
#elif FMT_HAS_CPP14_ATTRIBUTE(deprecated)
#  define FMT_DEPRECATED [[deprecated]]
#else
#  define FMT_DEPRECATED /* deprecated */
#endif

#ifdef FMT_ALWAYS_INLINE
// Use the provided definition.
#elif FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define FMT_ALWAYS_INLINE inline
#endif
// A version of FMT_ALWAYS_INLINE to prevent code bloat in debug mode.
#ifdef NDEBUG
#  define FMT_INLINE FMT_ALWAYS_INLINE
#else
#  define FMT_INLINE inline
#endif

#ifndef FMT_BEGIN_NAMESPACE
#  define FMT_BEGIN_NAMESPACE \
    namespace fmt {           \
    inline namespace v12 {
#  define FMT_END_NAMESPACE \
    }                       \
    }
#endif

#ifndef FMT_EXPORT
#  define FMT_EXPORT
#  define FMT_BEGIN_EXPORT
#  define FMT_END_EXPORT
#endif

#ifdef _WIN32
#  define FMT_WIN32 1
#else
#  define FMT_WIN32 0
#endif

#if !defined(FMT_HEADER_ONLY) && FMT_WIN32
#  if defined(FMT_LIB_EXPORT)
#    define FMT_API __declspec(dllexport)
#  elif defined(FMT_SHARED)
#    define FMT_API __declspec(dllimport)
#  endif
#elif defined(FMT_LIB_EXPORT) || defined(FMT_SHARED)
#  define FMT_API FMT_VISIBILITY("default")
#endif
#ifndef FMT_API
#  define FMT_API
#endif

#ifndef FMT_OPTIMIZE_SIZE
#  define FMT_OPTIMIZE_SIZE 0
#endif

// FMT_BUILTIN_TYPE=0 may result in smaller library size at the cost of higher
// per-call binary size by passing built-in types through the extension API.
#ifndef FMT_BUILTIN_TYPES
#  define FMT_BUILTIN_TYPES 1
#endif

#define FMT_APPLY_VARIADIC(expr) \
  using unused = int[];          \
  (void)unused { 0, (expr, 0)... }

FMT_BEGIN_NAMESPACE

// Implementations of enable_if_t and other metafunctions for older systems.
template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;
template <bool B> using bool_constant = std::integral_constant<bool, B>;
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;
template <typename T>
using remove_cvref_t = typename std::remove_cv<remove_reference_t<T>>::type;
template <typename T>
using make_unsigned_t = typename std::make_unsigned<T>::type;
template <typename T>
using underlying_t = typename std::underlying_type<T>::type;
template <typename T> using decay_t = typename std::decay<T>::type;
using nullptr_t = decltype(nullptr);

using ullong = unsigned long long;

#if (FMT_GCC_VERSION && FMT_GCC_VERSION < 500) || FMT_MSC_VERSION
// A workaround for gcc 4.9 & MSVC v141 to make void_t work in a SFINAE context.
template <typename...> struct void_t_impl {
  using type = void;
};
template <typename... T> using void_t = typename void_t_impl<T...>::type;
#else
template <typename...> using void_t = void;
#endif

struct monostate {
  constexpr monostate() {
    return {};
}
};

// An enable_if helper to be used in template parameters which results in much
// shorter symbols: https://godbolt.org/z/sWw4vP. Extra parentheses are needed
// to workaround a bug in MSVC 2019 (see #1140 and #1186).
#ifdef FMT_DOC
#  define FMT_ENABLE_IF(...)
#else
#  define FMT_ENABLE_IF(...) fmt::enable_if_t<(__VA_ARGS__), int> = 0
#endif

template <typename T> constexpr auto min_of(T a, T b) -> T {
    return {};
}
template <typename T> constexpr auto max_of(T a, T b) -> T {
    return {};
}

FMT_NORETURN FMT_API void assert_fail(const char* file, int line,
                                      const char* message);

namespace detail {
// Suppresses "unused variable" warnings with the method described in
// https://herbsutter.com/2009/10/18/mailbag-shutting-up-compiler-warnings/.
// (void)var does not work on many Intel compilers.
template <typename... T> FMT_CONSTEXPR void ignore_unused(const T&...) {
    return {};
}

constexpr auto is_constant_evaluated(bool default_value = false) noexcept
    -> bool {
    return {};
}

#ifdef FMT_ASSERT
// Use the provided definition.
#elif defined(NDEBUG)
// FMT_ASSERT is not empty to avoid -Wempty-body.
#  define FMT_ASSERT(condition, message) \
    fmt::detail::ignore_unused((condition), (message))
#else
#  define FMT_ASSERT(condition, message)                                    \
    ((condition) /* void() fails with -Winvalid-constexpr on clang 4.0.1 */ \
         ? (void)0                                                          \
         : ::fmt::assert_fail(__FILE__, __LINE__, (message)))
#endif

#ifdef FMT_USE_INT128
// Use the provided definition.
#elif defined(__SIZEOF_INT128__) && !defined(__NVCC__) && \
    !(FMT_CLANG_VERSION && FMT_MSC_VERSION)
#  define FMT_USE_INT128 1
using native_int128 = __int128_t;
using native_uint128 = __uint128_t;
inline auto map(native_int128 x) -> native_int128 {
    throw std::runtime_error("STUB: not implemented");
}
inline auto map(native_uint128 x) -> native_uint128 {
    throw std::runtime_error("STUB: not implemented");
}
#else
#  define FMT_USE_INT128 0
#endif
#if !FMT_USE_INT128
// Fallbacks to reduce conditional compilation and SFINAE.
enum class native_int128 {};
enum class native_uint128 {};
inline auto map(native_int128) -> monostate { return {}; }
inline auto map(native_uint128) -> monostate { return {}; }
#endif

// Casts a nonnegative integer to unsigned.
template <typename Int>
FMT_CONSTEXPR auto to_unsigned(Int value) -> make_unsigned_t<Int> {
    return {};
}

template <typename Char>
using unsigned_char = conditional_t<sizeof(Char) == 1, unsigned char, unsigned>;

// A heuristic to detect std::string and std::[experimental::]string_view.
// It is mainly used to avoid dependency on <[experimental/]string_view>.
template <typename T, typename Enable = void>
struct is_std_string_like : std::false_type {};
template <typename T>
struct is_std_string_like<T, void_t<decltype(std::declval<T>().find_first_of(
                                 typename T::value_type(), 0))>>
    : std::is_convertible<decltype(std::declval<T>().data()),
                          const typename T::value_type*> {};

// Check if the literal encoding is UTF-8.
enum { is_utf8_enabled = "\u00A7"[1] == '\xA7' };
enum { use_utf8 = !FMT_WIN32 || is_utf8_enabled };

#ifndef FMT_UNICODE
#  define FMT_UNICODE 1
#endif

static_assert(!FMT_UNICODE || use_utf8,
              "Unicode support requires compiling with /utf-8");

template <typename T> constexpr auto narrow(T*) -> char* {
    return {};
}
constexpr FMT_ALWAYS_INLINE auto narrow(const char* s) -> const char* {
    return {};
}

template <typename Char>
FMT_CONSTEXPR auto compare(const Char* s1, const Char* s2, size_t n) -> int {
    return {};
}

namespace adl {
using namespace std;

template <typename Container>
auto invoke_back_inserter()
    -> decltype(back_inserter(std::declval<Container&>()));
}  // namespace adl

template <typename It, typename Enable = std::true_type>
struct is_back_insert_iterator : std::false_type {};

template <typename It>
struct is_back_insert_iterator<
    It, bool_constant<std::is_same<
            decltype(adl::invoke_back_inserter<typename It::container_type>()),
            It>::value>> : std::true_type {};

// Extracts a reference to the container from *insert_iterator.
template <typename OutputIt>
inline FMT_CONSTEXPR auto get_container{
    return {};
}utputIt it) ->
    typename OutputIt::container_type& {
    return {};
}

template <typename T, typename Enable = void>
struct is_contiguous : std::false_type {};
template <typename T>
struct is_contiguous<T, void_t<decltype(std::declval<T&>().data()),
                               decltype(std::declval<T&>().size()),
                               decltype(std::declval<T&>()[size_t()])>>
    : std::true_type {};
}  // namespace detail

// Parsing-related public API and forward declarations.
FMT_BEGIN_EXPORT

/**
 * An implementation of `std::basic_string_view` for pre-C++17 providing a
 * subset of the API. `fmt::basic_string_view` is used in the public API even
 * if `std::basic_string_view` is available to prevent issues when a library is
 * compiled with a different `-std` option than the client code (which is not
 * recommended).
 */
template <typename Char> class basic_string_view {
 private:
  const Char* data_;
  size_t size_;

 public:
  using value_type = Char;
  using iterator = const Char*;

  constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {
    return {};
}
  constexpr basic_string_view(const Char* s, size_t count) noexcept
      : data_(s), size_(count) {
    return {};
}

#if FMT_GCC_VERSION
  FMT_ALWAYS_INLINE
#endif
  FMT_CONSTEXPR basic_string_view(const Char* s) : data_(s) {
    return {};
}

  template <
      typename S,
      FMT_ENABLE_IF(detail::is_std_string_like<S>::value&&  //
                        std::is_same<typename S::value_type, Char>::value)>
  constexpr basic_string_view(const S& s) noexcept
      : data_(s.data()), size_(s.size()) {
    return {};
}

  constexpr auto data() const noexcept -> const Char* {
    return {};
}
  constexpr auto size() const noexcept -> size_t {
    return {};
}

  constexpr auto begin() const noexcept -> iterator {
    return {};
}
  constexpr auto end() const noexcept -> iterator {
    return {};
}

  constexpr auto operator[](size_t pos) const noexcept -> const Char& {
    return {};
}

  FMT_CONSTEXPR void remove_prefix(size_t n) noexcept {
    return {};
}

  FMT_CONSTEXPR auto starts_with(basic_string_view sv) const noexcept -> bool {
    return {};
}
  FMT_CONSTEXPR auto starts_with(Char c) const noexcept -> bool {
    return {};
}
  FMT_CONSTEXPR auto starts_with(const Char* s) const -> bool {
    return {};
}

  FMT_CONSTEXPR auto compare(basic_string_view other) const -> int {
    return {};
}

  FMT_CONSTEXPR friend auto operator==(basic_string_view lhs,
                                       basic_string_view rhs) -> bool {
    return {};
}
  friend auto operator!=(basic_string_view lhs, basic_string_view rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  friend auto operator<(basic_string_view lhs, basic_string_view rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  friend auto operator<=(basic_string_view lhs, basic_string_view rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  friend auto operator>(basic_string_view lhs, basic_string_view rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  friend auto operator>=(basic_string_view lhs, basic_string_view rhs) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};
using string_view = basic_string_view<char>;

template <typename T> class basic_appender;
using appender = basic_appender<char>;

class context;
template <typename OutputIt, typename Char> class generic_context;
template <typename Char> class parse_context;

// Longer aliases for C++20 compatibility.
template <typename Char> using basic_format_parse_context = parse_context<Char>;
using format_parse_context = parse_context<char>;
template <typename OutputIt, typename Char>
using basic_format_context =
    conditional_t<std::is_same<OutputIt, appender>::value, context,
                  generic_context<OutputIt, Char>>;
using format_context = context;

template <typename Char>
using buffered_context =
    conditional_t<std::is_same<Char, char>::value, context,
                  generic_context<basic_appender<Char>, Char>>;

template <typename T> struct is_contiguous : detail::is_contiguous<T> {};

template <typename Context> class basic_format_arg;
template <typename Context> class basic_format_args;

// A separate type would result in shorter symbols but break ABI compatibility
// between clang and gcc on ARM (#1919).
using format_args = basic_format_args<context>;

// A formatter for objects of type T.
template <typename T, typename Char = char, typename Enable = void>
struct formatter {
  // A deleted default constructor indicates a disabled formatter.
  formatter() = delete;
};

template <typename T, typename Enable = void>
struct locking : std::false_type {};

/// Reports a format error at compile time or, via a `format_error` exception,
/// at runtime.
// This function is intentionally not constexpr to give a compile-time error.
FMT_NORETURN FMT_API void report_error(const char* message);

enum class presentation_type : unsigned char {
  // Common specifiers:
  none = 0,
  debug = 1,   // '?'
  string = 2,  // 's' (string, bool)

  // Integral, bool and character specifiers:
  dec = 3,  // 'd'
  hex,      // 'x' or 'X'
  oct,      // 'o'
  bin,      // 'b' or 'B'
  chr,      // 'c'

  // String and pointer specifiers:
  pointer = 3,  // 'p'

  // Floating-point specifiers:
  exp = 1,  // 'e' or 'E' (1 since there is no FP debug presentation)
  fixed,    // 'f' or 'F'
  general,  // 'g' or 'G'
  hexfloat  // 'a' or 'A'
};

enum class align { none, left, right, center, numeric };
enum class sign { none, minus, plus, space };
enum class arg_id_kind { none, index, name };

// Basic format specifiers for built-in and string types.
class basic_specs {
 private:
  // Data is arranged as follows:
  //
  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |type |align| w | p | s |u|#|L|  f  |          unused           |
  // +-----+-----+---+---+---+-+-+-+-----+---------------------------+
  //
  //   w - dynamic width info
  //   p - dynamic precision info
  //   s - sign
  //   u - uppercase (e.g. 'X' for 'x')
  //   # - alternate form ('#')
  //   L - localized
  //   f - fill size
  //
  // Bitfields are not used because of compiler bugs such as gcc bug 61414.
  enum : unsigned {
    type_mask = 0x00007,
    align_mask = 0x00038,
    width_mask = 0x000C0,
    precision_mask = 0x00300,
    sign_mask = 0x00C00,
    uppercase_mask = 0x01000,
    alternate_mask = 0x02000,
    localized_mask = 0x04000,
    fill_size_mask = 0x38000,

    align_shift = 3,
    width_shift = 6,
    precision_shift = 8,
    sign_shift = 10,
    fill_size_shift = 15,

    max_fill_size = 4
  };

  unsigned data_ = 1 << fill_size_shift;
  static_assert(sizeof(basic_specs::data_) * CHAR_BIT >= 18, "");

  // Character (code unit) type is erased to prevent template bloat.
  char fill_data_[max_fill_size] = {' '};

  FMT_CONSTEXPR void set_fill_size(size_t size) {
    return {};
}

 public:
  constexpr auto type() const -> presentation_type {
    return {};
}
  FMT_CONSTEXPR void set_type(presentation_type t) {
    return {};
}

  constexpr auto align() const -> align {
    return {};
}
  FMT_CONSTEXPR void set_align(fmt::align a) {
    return {};
}

  constexpr auto dynamic_width() const -> arg_id_kind {
    return {};
}
  FMT_CONSTEXPR void set_dynamic_width(arg_id_kind w) {
    return {};
}

  FMT_CONSTEXPR auto dynamic_precision() const -> arg_id_kind {
    return {};
}
  FMT_CONSTEXPR void set_dynamic_precision(arg_id_kind p) {
    return {};
}

  constexpr auto dynamic() const -> bool {
    return {};
}

  constexpr auto sign() const -> sign {
    return {};
}
  FMT_CONSTEXPR void set_sign(fmt::sign s) {
    return {};
}

  constexpr auto upper() const -> bool {
    return {};
}
  FMT_CONSTEXPR void set_upper() {
    return {};
}

  constexpr auto alt() const -> bool {
    return {};
}
  FMT_CONSTEXPR void set_alt() {
    return {};
}
  FMT_CONSTEXPR void clear_alt() {
    return {};
}

  constexpr auto localized() const -> bool {
    return {};
}
  FMT_CONSTEXPR void set_localized() {
    return {};
}

  constexpr auto fill_size() const -> size_t {
    return {};
}

  template <typename Char, FMT_ENABLE_IF(std::is_same<Char, char>::value)>
  constexpr auto fill() const -> const Char* {
    return {};
}
  template <typename Char, FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
  constexpr auto fill() const -> const Char* {
    return {};
}

  template <typename Char> constexpr auto fill_unit() const -> Char {
    return {};
}

  FMT_CONSTEXPR void set_fill(char c) {
    return {};
}

  template <typename Char>
  FMT_CONSTEXPR void set_fill(basic_string_view<Char> s) {
    return {};
}

  FMT_CONSTEXPR void copy_fill_from(const basic_specs& specs) {
    return {};
}
};

// Format specifiers for built-in and string types.
struct format_specs : basic_specs {
  int width;
  int precision;

  constexpr format_specs() : width(0), precision(-1) {
    return {};
}
};

/**
 * Parsing context consisting of a format string range being parsed and an
 * argument counter for automatic indexing.
 */
template <typename Char = char> class parse_context {
 private:
  basic_string_view<Char> fmt_;
  int next_arg_id_;

  enum { use_constexpr_cast = !FMT_GCC_VERSION || FMT_GCC_VERSION >= 1200 };

  FMT_CONSTEXPR void do_check_arg_id(int arg_id);

 public:
  using char_type = Char;
  using iterator = const Char*;

  constexpr explicit parse_context(basic_string_view<Char> fmt,
                                   int next_arg_id = 0)
      : fmt_(fmt), next_arg_id_(next_arg_id) {
    return {};
}

  /// Returns an iterator to the beginning of the format string range being
  /// parsed.
  constexpr auto begin() const noexcept -> iterator {
    return {};
}

  /// Returns an iterator past the end of the format string range being parsed.
  constexpr auto end() const noexcept -> iterator {
    return {};
}

  /// Advances the begin iterator to `it`.
  FMT_CONSTEXPR void advance_to(iterator it) {
    return {};
}

  /// Reports an error if using the manual argument indexing; otherwise returns
  /// the next argument index and switches to the automatic indexing.
  FMT_CONSTEXPR auto next_arg_id() -> int {
    return {};
}

  /// Reports an error if using the automatic argument indexing; otherwise
  /// switches to the manual indexing.
  FMT_CONSTEXPR void check_arg_id(int id) {
    return {};
}
  FMT_CONSTEXPR void check_arg_id(basic_string_view<Char>) {
    return {};
}
  FMT_CONSTEXPR void check_dynamic_spec(int arg_id);
};

#ifndef FMT_USE_LOCALE
#  define FMT_USE_LOCALE (FMT_OPTIMIZE_SIZE <= 1)
#endif

// A type-erased reference to std::locale to avoid the heavy <locale> include.
class locale_ref {
#if FMT_USE_LOCALE
 private:
  const void* locale_;  // A type-erased pointer to std::locale.

 public:
  constexpr locale_ref() : locale_(nullptr) {
    return {};
}

  template <typename Locale, FMT_ENABLE_IF(sizeof(Locale::collate) != 0)>
  locale_ref(const Locale& loc) : locale_(&loc) {
    throw std::runtime_error("STUB: not implemented");
}

  inline explicit operator bool() const noexcept {
    abort();
}
#else
 public:
  inline explicit operator bool() const noexcept { return false; }
#endif  // FMT_USE_LOCALE

 public:
  template <typename Locale> auto get() const -> Locale;
};

FMT_END_EXPORT

namespace detail {

// Specifies if `T` is a code unit type.
template <typename T> struct is_code_unit : std::false_type {};
template <> struct is_code_unit<char> : std::true_type {};
template <> struct is_code_unit<wchar_t> : std::true_type {};
template <> struct is_code_unit<char16_t> : std::true_type {};
template <> struct is_code_unit<char32_t> : std::true_type {};
#ifdef __cpp_char8_t
template <> struct is_code_unit<char8_t> : bool_constant<is_utf8_enabled> {};
#endif

// Constructs fmt::basic_string_view<Char> from types implicitly convertible
// to it, deducing Char. Explicitly convertible types such as the ones returned
// from FMT_STRING are intentionally excluded.
template <typename Char, FMT_ENABLE_IF(is_code_unit<Char>::value)>
constexpr auto to_string_view(const Char* s) -> basic_string_view<Char> {
    return {};
}
template <typename T, FMT_ENABLE_IF(is_std_string_like<T>::value)>
constexpr auto to_string_view(const T& s)
    -> basic_string_view<typename T::value_type> {
    return {};
}
template <typename Char>
constexpr auto to_string_view(basic_string_view<Char> s)
    -> basic_string_view<Char> {
    return {};
}

template <typename T, typename Enable = void>
struct has_to_string_view : std::false_type {};
// detail:: is intentional since to_string_view is not an extension point.
template <typename T>
struct has_to_string_view<
    T, void_t<decltype(detail::to_string_view(std::declval<T>()))>>
    : std::true_type {};

/// String's character (code unit) type. detail:: is intentional to prevent ADL.
template <typename S,
          typename V = decltype(detail::to_string_view(std::declval<S>()))>
using char_t = typename V::value_type;

enum class type {
  none_type,
  // Integer types should go first,
  int_type,
  uint_type,
  long_long_type,
  ulong_long_type,
  int128_type,
  uint128_type,
  bool_type,
  char_type,
  last_integer_type = char_type,
  // followed by floating-point types.
  float_type,
  double_type,
  long_double_type,
  last_numeric_type = long_double_type,
  cstring_type,
  string_type,
  pointer_type,
  custom_type
};

// Maps core type T to the corresponding type enum constant.
template <typename T, typename Char>
struct type_constant : std::integral_constant<type, type::custom_type> {};

#define FMT_TYPE_CONSTANT(Type, constant) \
  template <typename Char>                \
  struct type_constant<Type, Char>        \
      : std::integral_constant<type, type::constant> {}

FMT_TYPE_CONSTANT(int, int_type);
FMT_TYPE_CONSTANT(unsigned, uint_type);
FMT_TYPE_CONSTANT(long long, long_long_type);
FMT_TYPE_CONSTANT(ullong, ulong_long_type);
FMT_TYPE_CONSTANT(native_int128, int128_type);
FMT_TYPE_CONSTANT(native_uint128, uint128_type);
FMT_TYPE_CONSTANT(bool, bool_type);
FMT_TYPE_CONSTANT(Char, char_type);
FMT_TYPE_CONSTANT(float, float_type);
FMT_TYPE_CONSTANT(double, double_type);
FMT_TYPE_CONSTANT(long double, long_double_type);
FMT_TYPE_CONSTANT(const Char*, cstring_type);
FMT_TYPE_CONSTANT(basic_string_view<Char>, string_type);
FMT_TYPE_CONSTANT(const void*, pointer_type);

constexpr auto is_integral_type(type t) -> bool {
    return {};
}
constexpr auto is_arithmetic_type(type t) -> bool {
    return {};
}

constexpr auto set(type rhs) -> int {
    return {};
}
constexpr auto in(type t, int set) -> bool {
    return {};
}

// Bitsets of types.
enum {
  sint_set =
      set(type::int_type) | set(type::long_long_type) | set(type::int128_type),
  uint_set = set(type::uint_type) | set(type::ulong_long_type) |
             set(type::uint128_type),
  bool_set = set(type::bool_type),
  char_set = set(type::char_type),
  float_set = set(type::float_type) | set(type::double_type) |
              set(type::long_double_type),
  string_set = set(type::string_type),
  cstring_set = set(type::cstring_type),
  pointer_set = set(type::pointer_type)
};

struct view {};

template <typename T, typename Enable = std::true_type>
struct is_view : std::false_type {};
template <typename T>
struct is_view<T, bool_constant<sizeof(T) != 0>> : std::is_base_of<view, T> {};

// DEPRECATED! named_arg will be moved to the fmt namespace.
template <typename T, typename Char> struct named_arg;
template <typename T> struct is_named_arg : std::false_type {};
template <typename T> struct is_static_named_arg : std::false_type {};

template <typename T, typename Char>
struct is_named_arg<named_arg<T, Char>> : std::true_type {};

template <typename T, typename Char = char> struct named_arg : view {
  const Char* name;
  const T& value;

  named_arg(const Char* n, const T& v) : name(n), value(v) {
    throw std::runtime_error("STUB: not implemented");
}
  static_assert(!is_named_arg<T>::value, "nested named arguments");
};

template <bool B = false> constexpr auto count() -> int {
    return {};
}
template <bool B1, bool B2, bool... Tail> constexpr auto count() -> int {
    return {};
}

template <typename... T> constexpr auto count_named_args() -> int {
    return {};
}
template <typename... T> constexpr auto count_static_named_args() -> int {
    return {};
}

template <typename Char> struct named_arg_info {
  const Char* name;
  int id;
};

// named_args is non-const to suppress a bogus -Wmaybe-uninitialized in gcc 13.
template <typename Char>
FMT_CONSTEXPR void check_for_duplicate(named_arg_info<Char>* named_args,
                                       int named_arg_index,
                                       basic_string_view<Char> arg_name) {
    return {};
}

template <typename Char, typename T, FMT_ENABLE_IF(!is_named_arg<T>::value)>
void init_named_arg(named_arg_info<Char>*, int& arg_index, int&, const T&) {
    throw std::runtime_error("STUB: not implemented");
}
template <typename Char, typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
void init_named_arg(named_arg_info<Char>* named_args, int& arg_index,
                    int& named_arg_index, const T& arg) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, typename Char,
          FMT_ENABLE_IF(!is_static_named_arg<T>::value)>
FMT_CONSTEXPR void init_static_named_arg(named_arg_info<Char>*, int& arg_index,
                                         int&) {
    return {};
}
template <typename T, typename Char,
          FMT_ENABLE_IF(is_static_named_arg<T>::value)>
FMT_CONSTEXPR void init_static_named_arg(named_arg_info<Char>* named_args,
                                         int& arg_index, int& named_arg_index) {
    return {};
}

// To minimize the number of types we need to deal with, long is translated
// either to int or to long long depending on its size.
enum { long_short = sizeof(long) == sizeof(int) && FMT_BUILTIN_TYPES };
using long_type = conditional_t<long_short, int, long long>;
using ulong_type = conditional_t<long_short, unsigned, ullong>;

template <typename T>
using format_as_result =
    remove_cvref_t<decltype(format_as(std::declval<const T&>()))>;
template <typename T>
using format_as_member_result =
    remove_cvref_t<decltype(formatter<T>::format_as(std::declval<const T&>()))>;

template <typename T, typename Enable = std::true_type>
struct use_format_as : std::false_type {};
// format_as member is only used to avoid injection into the std namespace.
template <typename T, typename Enable = std::true_type>
struct use_format_as_member : std::false_type {};

// Only map owning types because mapping views can be unsafe.
template <typename T>
struct use_format_as<
    T, bool_constant<std::is_arithmetic<format_as_result<T>>::value>>
    : std::true_type {};
template <typename T>
struct use_format_as_member<
    T, bool_constant<std::is_arithmetic<format_as_member_result<T>>::value>>
    : std::true_type {};

template <typename T, typename U = remove_const_t<T>>
using use_formatter =
    bool_constant<(std::is_class<T>::value || std::is_enum<T>::value ||
                   std::is_union<T>::value || std::is_array<T>::value) &&
                  !has_to_string_view<T>::value && !is_named_arg<T>::value &&
                  !use_format_as<T>::value && !use_format_as_member<U>::value>;

template <typename Char, typename T, typename U = remove_const_t<T>>
auto has_formatter_impl(T* p, buffered_context<Char>* ctx = nullptr)
    -> decltype(formatter<U, Char>().format(*p, *ctx), std::true_type());
template <typename Char> auto has_formatter_impl(...) -> std::false_type;

// T can be const-qualified to check if it is const-formattable.
template <typename T, typename Char> constexpr auto has_formatter() -> bool {
    return {};
}

// Maps formatting argument types to natively supported types or user-defined
// types with formatters. Returns void on errors to be SFINAE-friendly.
template <typename Char> struct type_mapper {
  static auto map(signed char) -> int;
  static auto map(unsigned char) -> unsigned;
  static auto map(short) -> int;
  static auto map(unsigned short) -> unsigned;
  static auto map(int) -> int;
  static auto map(unsigned) -> unsigned;
  static auto map(long) -> long_type;
  static auto map(unsigned long) -> ulong_type;
  static auto map(long long) -> long long;
  static auto map(ullong) -> ullong;
  static auto map(native_int128) -> native_int128;
  static auto map(native_uint128) -> native_uint128;
  static auto map(bool) -> bool;

  template <typename T, FMT_ENABLE_IF(is_code_unit<T>::value)>
  static auto map(T) -> conditional_t<
      std::is_same<T, char>::value || std::is_same<T, Char>::value, Char, void>;

  static auto map(float) -> float;
  static auto map(double) -> double;
  static auto map(long double) -> long double;

  static auto map(Char*) -> const Char*;
  static auto map(const Char*) -> const Char*;
  template <typename T, typename C = char_t<T>,
            FMT_ENABLE_IF(!std::is_pointer<T>::value)>
  static auto map(const T&) -> conditional_t<std::is_same<C, Char>::value,
                                             basic_string_view<C>, void>;

  static auto map(void*) -> const void*;
  static auto map(const void*) -> const void*;
  static auto map(volatile void*) -> const void*;
  static auto map(const volatile void*) -> const void*;
  static auto map(nullptr_t) -> const void*;
  template <typename T, FMT_ENABLE_IF(std::is_pointer<T>::value ||
                                      std::is_member_pointer<T>::value)>
  static auto map(const T&) -> void;

  template <typename T, FMT_ENABLE_IF(use_format_as<T>::value)>
  static auto map(const T& x) -> decltype(map(format_as(x)));
  template <typename T, FMT_ENABLE_IF(use_format_as_member<T>::value)>
  static auto map(const T& x) -> decltype(map(formatter<T>::format_as(x)));

  template <typename T, FMT_ENABLE_IF(use_formatter<T>::value)>
  static auto map(T&) -> conditional_t<has_formatter<T, Char>(), T&, void>;

  template <typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
  static auto map(const T& named_arg) -> decltype(map(named_arg.value));
};

// detail:: is used to workaround a bug in MSVC 2017.
template <typename T, typename Char>
using mapped_t = decltype(detail::type_mapper<Char>::map(std::declval<T&>()));

// A type constant after applying type_mapper.
template <typename T, typename Char = char>
using mapped_type_constant = type_constant<mapped_t<T, Char>, Char>;

template <typename T, typename Context,
          type TYPE =
              mapped_type_constant<T, typename Context::char_type>::value>
using stored_type_constant = std::integral_constant<
    type, Context::builtin_types || TYPE == type::int_type ? TYPE
                                                           : type::custom_type>;
// A parse context {
    return {};
}ext<Char> {
 private:
  int num_args_;
  const type* types_;
  using base = parse_context<Char>;

 public:
  constexpr explicit compile_parse_context(basic_string_view<Char> fmt,
                                           int num_args, const type* types,
                           {
    return {};
}auto num_args() const -> int {
    return {};
}
  constexpr auto arg_type(int id) const -> type {
    return {};
}

  FMT_CONSTEXPR auto next_arg_id() -> int {
    return {};
}

  FMT_CONSTEXPR void check_arg_id(int id) {
    return {};
}
  using base::check_arg_id;

  FMT_CONSTEXPR void check_dynamic_spec(int arg_id) {
    return {};
}
};

// An argument reference.
template <typename Char> union arg_ref {
  FMT_CONSTEXPR arg_ref(int idx = 0) : index(idx) {
    return {};
}
  FMT_CONSTEXPR arg_ref(basic_string_view<Char> n) : name(n) {
    return {};
}

  int index;
  basic_string_view<Char> name;
};

// Format specifiers with width and precision resolved at formatting rather
// than parsing time to allow reusing the same parsed specifiers with
// different sets of arguments (precompilation of format strings).
template <typename Char = char> struct dynamic_format_specs : format_specs {
  arg_ref<Char> width_ref;
  arg_ref<Char> precision_ref;
};

// Converts a character to ASCII. Returns '\0' on conversion failure.
template <typename Char, FMT_ENABLE_IF(std::is_integral<Char>::value)>
constexpr auto to_ascii(Char c) -> char {
    return {};
}

// Returns the number of code units in a code point or 1 on error.
template <typename Char>
FMT_CONSTEXPR auto code_point_length(const Char* begin) -> int {
    return {};
}

// Parses the range [begin, end) as an unsigned integer. This function assumes
// that the range is non-empty and the first character is a digit.
template <typename Char>
FMT_CONSTEXPR auto parse_nonnegative_int(const Char*& begin, const Char* end,
                                         int error_value) noexcept -> int {
    return {};
}

FMT_CONSTEXPR inline auto parse_align(char c) -> align {
    return {};
}

template <typename Char> constexpr auto is_name_start(Char c) -> bool {
    return {};
}

template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_arg_id(const Char* begin, const Char* end,
                                Handler&& handler) -> const Char* {
    return {};
}

template <typename Char> struct dynamic_spec_handler {
  parse_context<Char>& ctx;
  arg_ref<Char>& ref;
  arg_id_kind& kind;

  FMT_CONSTEXPR void on_index(int id) {
    return {};
}
  FMT_CONSTEXPR void on_name(basic_string_view<Char> id) {
    return {};
}
};

template <typename Char> struct parse_dynamic_spec_result {
  const Char* end;
  arg_id_kind kind;
};

// Parses integer | "{" [arg_id] "}".
template <typename Char>
FMT_CONSTEXPR auto parse_dynamic_spec(const Char* begin, const Char* end,
                                      int& value, arg_ref<Char>& ref,
                                      parse_context<Char>& ctx)
    -> parse_dynamic_spec_result<Char> {
    return {};
}

template <typename Char>
FMT_CONSTEXPR auto parse_width(const Char* begin, const Char* end,
                               format_specs& specs, arg_ref<Char>& width_ref,
                               parse_context<Char>& ctx) -> const Char* {
    return {};
}

template <typename Char>
FMT_CONSTEXPR auto parse_precision(const Char* begin, const Char* end,
                                   format_specs& specs,
                                   arg_ref<Char>& precision_ref,
                                   parse_context<Char>& ctx) -> const Char* {
    return {};
}

enum cla{
    return {};
}, zero, width, precision, locale };

// Parses standard format{
    return {};
}PR auto parse_format_specs(const Char* begin, const Char* end,
                                      dynamic_format_specs<Char>& specs,
                                      parse_context<Char>& ctx, type arg_type)
    -> const Char* {
    return {};
}

template <typename Char, typename Handler>
FMT_CONSTEXPR FMT_INLINE auto parse_replacement_field(const Char* begin,
                                                      const Char* end,
                                                      Handler&& handler)
    -> const Char* {
    return {};
}

template <typename Char, typename Handler>
FMT_CONSTEXPR void parse_format_string(basic_string_view<Char> fmt,
                                       Handler&& handler) {
    return {};
}

// Checks char specs and returns true iff the presentation type is char-like.
FMT_CONSTEXPR inline auto check_char_specs(const format_specs& specs) -> bool {
    return {};
}

// A base class for compile-time strings.
struct compile_string {};

template <typename T, typename Char>
FMT_VISIBILITY("hidden")  // Suppress an ld warning on macOS (#3769).
FMT_CONSTEXPR auto invoke_parse(parse_context<Char>& ctx) -> const Char* {
    return {};
}

template <typename... T> struct arg_pack {};

template <typename Char, int NUM_ARGS, int NUM_NAMED_ARGS, bool DYNAMIC_NAMES>
class format_string_checker {
 private:
  type types_[max_of<size_t>(1, NUM_ARGS)];
  named_arg_info<Char> named_args_[max_of<size_t>(1, NUM_NAMED_ARGS)];
  compile_parse_context<Char> context_;

  using parse_func = auto (*)(parse_context<Char>&) -> const Char*;
  parse_func parse_funcs_[max_of<size_t>(1, NUM_ARGS)];

 public:
  template <typename... T>
  FMT_CONSTEXPR explicit format_string_checker(basic_string_view<Char> fmt,
                                               arg_pack<T...>)
      : types_{mapped_type_constant<T, Char>::value...},
        named_args_{},
        context_(fmt, NUM_ARGS, types_),
        parse_funcs_{&invoke_parse<T, Char>...} {
    return {};
}

  FMT_CONSTEXPR void on_text(const Char*, const Char*) {
    return {};
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

  FMT_CONSTEXPR void on_replacement_field(int id, const Char* begin) {
    return {};
}

  FMT_CONSTEXPR auto on_format_specs(int id, const Char* begin, const Char* end)
      -> const Char* {
    return {};
}

  FMT_NORETURN FMT_CONSTEXPR void on_error(const char* message) {
    return {};
}
};

/// A contiguous memory buffer with an optional growing ability. It is an
/// internal class and shouldn't be used directly, only via `memory_buffer`.
template <typename T> class buffer {
 private:
  T* ptr_;
  size_t size_;
  size_t capacity_;

  using grow_fun = void (*)(buffer& buf, size_t capacity);
  grow_fun grow_;

 protected:
  // Don't initialize ptr_ since it is not accessed to save a few cycles.
  FMT_CONSTEXPR buffer(grow_fun grow, size_t sz) noexcept
      : size_(sz), capacity_(sz), grow_(grow) {
    return {};
}

  constexpr buffer(grow_fun grow, T* p = nullptr, size_t sz = 0,
                   size_t cap = 0) noexcept
      : ptr_(p), size_(sz), capacity_(cap), grow_(grow) {
    return {};
}

  FMT_CONSTEXPR20 ~buffer() = default;
  buffer(buffer&&) = default;

  /// Sets the buffer data and capacity.
  FMT_CONSTEXPR void set(T* buf_data, size_t buf_capacity) noexcept {
    return {};
}

 public:
  using value_type = T;
  using const_reference = const T&;

  buffer(const buffer&) = delete;
  void operator=(const buffer&) = delete;

  auto begin() noexcept -> T* {
    abort();
}
  auto end() noexcept -> T* {
    abort();
}

  auto begin() const noexcept -> const T* {
    abort();
}
  auto end() const noexcept -> const T* {
    abort();
}

  /// Returns the size of this buffer.
  constexpr auto size() const noexcept -> size_t {
    return {};
}

  /// Returns the capacity of this buffer.
  constexpr auto capacity() const noexcept -> size_t {
    return {};
}

  /// Returns a pointer to the buffer data (not null-terminated).
  FMT_CONSTEXPR auto data() noexcept -> T* {
    return {};
}
  FMT_CONSTEXPR auto data() const noexcept -> const T* {
    return {};
}

  /// Clears this buffer.
  FMT_CONSTEXPR void clear() {
    return {};
}

  // Tries resizing the buffer to contain `count` elements. If T is a POD type
  // the new elements may not be initialized.
  FMT_CONSTEXPR void try_resize(size_t count) {
    return {};
}

  // Tries increasing the buffer capacity to `new_capacity`. It can increase the
  // capacity by a smaller amount than requested but guarantees there is space
  // for at least one additional element either by increasing the capacity or by
  // flushing the buffer if it is full.
  FMT_CONSTEXPR void try_reserve(size_t new_capacity) {
    return {};
}

  FMT_CONSTEXPR void push_back(const T& value) {
    return {};
}

  /// Appends data to the end of the buffer.
  template <typename U>
  FMT_CONSTEXPR20 void append(const U* begin, const U* end) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Idx> FMT_CONSTEXPR auto operator[](Idx index) -> T& {
    return {};
}
  template <typename Idx>
  constexpr auto operator[](Idx index) const -> const T& {
    return {};
}
};

struct buffer_traits {
  constexpr explicit buffer_traits(size_t) {
    return {};
}
  constexpr auto count() const -> size_t {
    return {};
}
  constexpr auto limit(size_t size) const -> size_t {
    return {};
}
};

class fixed_buffer_traits {
 private:
  size_t count_ = 0;
  size_t limit_;

 public:
  constexpr explicit fixed_buffer_traits(size_t limit) : limit_(limit) {
    return {};
}
  constexpr auto count() const -> size_t {
    return {};
}
  FMT_CONSTEXPR auto limit(size_t size) -> size_t {
    return {};
}
};

template <typename OutputIt, typename InputIt, typename = void>
struct has_append : std::false_type {};

template <typename OutputIt, typename InputIt>
struct has_append<OutputIt, InputIt,
                  void_t<decltype(get_container(std::declval<OutputIt>())
                                      .append(std::declval<InputIt>(),
                                              std::declval<InputIt>()))>>
    : std::true_type {};

template <typename OutputIt, typename T, typename = void>
struct has_insert : std::false_type {};

template <typename OutputIt, typename T>
struct has_insert<
    OutputIt, T,
    void_t<decltype(get_container(std::declval<OutputIt>())
                        .insert({}, std::declval<T>(), std::declval<T>()))>>
    : std::true_type {};

// An optimized version of std::copy with the output value type (T).
template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>() &&
                        has_append<OutputIt, InputIt>())>
FMT_CONSTEXPR auto copy(InputIt begin, InputIt end, OutputIt out) -> OutputIt {
    return {};
}

template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>() &&
                        !has_append<OutputIt, InputIt>() &&
                        has_insert<OutputIt, InputIt>())>
FMT_CONSTEXPR auto copy(InputIt begin, InputIt end, OutputIt out) -> OutputIt {
    return {};
}

template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(!is_back_insert_iterator<OutputIt>() ||
                        !(has_append<OutputIt, InputIt>() ||
                          has_insert<OutputIt, InputIt>()))>
FMT_CONSTEXPR auto copy(InputIt begin, InputIt end, OutputIt out) -> OutputIt {
    return {};
}

// A buffer that writes to an output iterator when flushed.
template <typename OutputIt, typename T, typename Traits = buffer_traits>
class iterator_buffer : public Traits, public buffer<T> {
 private:
  OutputIt out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    return {};
}

  void flush() {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  explicit iterator_buffer(OutputIt out, size_t n = buffer_size)
      : Traits(n), buffer<T>(grow, data_, 0, buffer_size), out_(out) {
    throw std::runtime_error("STUB: not implemented");
}
  iterator_buffer(iterator_buffer&& other) noexcept
      : Traits(other),
        buffer<T>(grow, data_, 0, buffer_size),
        out_(other.out_) {}
  ~iterator_buffer() {
    // Don't crash if flush fails during unwinding.
    FMT_TRY { flush(); }
    FMT_CATCH(...) {}
  }

  auto out() -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}
  auto count() const -> size_t {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename T>
class iterator_buffer<T*, T, fixed_buffer_traits> : public fixed_buffer_traits,
                                                    public buffer<T> {
 private:
  T* out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    return {};
}

  void flush() {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  explicit iterator_buffer(T* out, size_t n = buffer_size)
      : fixed_buffer_traits(n), buffer<T>(grow, out, 0, n), out_(out) {
    throw std::runtime_error("STUB: not implemented");
}
  iterator_buffer(iterator_buffer&& other) noexcept
      : fixed_buffer_traits(other),
        buffer<T>(static_cast<iterator_buffer&&>(other)),
        out_(other.out_) {
    if (this->data() != out_) {
      this->set(data_, buffer_size);
      this->clear();
    }
  }
  ~iterator_buffer() { flush(); }

  auto out() -> T* {
    throw std::runtime_error("STUB: not implemented");
}
  auto count() const -> size_t {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename T> class iterator_buffer<T*, T> : public buffer<T> {
 public:
  explicit iterator_buffer(T* out, size_t = 0)
      : buffer<T>([](buffer<T>&, size_t) {}, out, 0, ~size_t()) {}

  auto out() -> T* { return &*this->end(); }
};

template <typename Container>
class container_buffer : public buffer<typename Container::value_type> {
 private:
  using value_type = typename Container::value_type;

  static FMT_CONSTEXPR void grow(buffer<value_type>& buf, size_t capacity) {
    return {};
}

 public:
  Container& container;

  explicit container_buffer(Container& c)
      : buffer<value_type>(grow, c.size()), container(c) {
    throw std::runtime_error("STUB: not implemented");
}
};

// A buffer that writes to a container with the contiguous storage.
template <typename OutputIt>
class iterator_buffer<
    OutputIt,
    enable_if_t<is_back_insert_iterator<OutputIt>::value &&
                    is_contiguous<typename OutputIt::container_type>::value,
                typename OutputIt::container_type::value_type>>
    : public container_buffer<typename OutputIt::container_type> {
 private:
  using base = container_buffer<typename OutputIt::container_type>;

 public:
  explicit iterator_buffer(typename OutputIt::container_type& c) : base(c) {}
  explicit iterator_buffer(OutputIt out, size_t = 0)
      : base(get_container(out)) {}

  auto out() -> OutputIt { return OutputIt(this->container); }
};

// A buffer that counts the number of code units written discarding the output.
template <typename T = char> class counting_buffer : public buffer<T> {
 private:
  enum { buffer_size = 256 };
  T data_[buffer_size];
  size_t count_ = 0;

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    return {};
}

 public:
  constexpr counting_buffer() : buffer<T>(grow, data_, 0, buffer_size) {
    return {};
}

  constexpr auto count() const noexcept -> size_t {
    return {};
}
};

template <typename T>
struct is_back_insert_iterator<basic_appender<T>> : std::true_type {};

template <typename It, typename Enable = std::true_type>
struct is_buffer_appender : std::false_type {};
template <typename It>
struct is_buffer_appender<
    It, bool_constant<
            is_back_insert_iterator<It>::value &&
            std::is_base_of<buffer<typename It::container_type::value_type>,
                            typename It::container_type>::value>>
    : std::true_type {};

// Maps an output iterator to a buffer.
template <typename T, typename OutputIt,
          FMT_ENABLE_IF(!is_buffer_appender<OutputIt>::value)>
auto get_buffer(OutputIt out) -> iterator_buffer<OutputIt, T> {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, typename OutputIt,
          FMT_ENABLE_IF(is_buffer_appender<OutputIt>::value)>
auto get_buffer(OutputIt out) -> buffer<T>& {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Buf, typename OutputIt>
auto get_iterator(Buf& buf, OutputIt) -> decltype(buf.out()) {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, typename OutputIt>
auto get_iterator(buffer<T>&, OutputIt out) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

// This type is intentionally undefined, only used for errors.
template <typename T, typename Char> struct type_is_unformattable_for;

template <typename Char> struct string_value {
  const Char* data;
  size_t size;
  auto str() const -> basic_string_view<Char> {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename Context> struct custom_value {
  using char_type = typename Context::char_type;
  void* value;
  void (*format)(void* arg, parse_context<char_type>& parse_ctx, Context& ctx);
};

template <typename Char> struct named_arg_value {
  const named_arg_info<Char>* data;
  size_t size;
};

struct custom_tag {};

#if !FMT_BUILTIN_TYPES
#  define FMT_BUILTIN , monostate
#else
#  define FMT_BUILTIN
#endif

// A formatting argument value.
template <typename Context> class value {
 public:
  using char_type = typename Context::char_type;

  union {
    monostate no_value;
    int int_value;
    unsigned uint_value;
    long long long_long_value;
    ullong ulong_long_value;
    native_int128 int128_value;
    native_uint128 uint128_value;
    bool bool_value;
    char_type char_value;
    float float_value;
    double double_value;
    long double long_double_value;
    const void* pointer;
    string_value<char_type> string;
    custom_value<Context> custom;
    named_arg_value<char_type> named_args;
  };

  constexpr FMT_INLINE value() : no_value() {
    return {};
}
  constexpr FMT_INLINE value(signed char x) : int_value(x) {
    return {};
}
  constexpr FMT_INLINE value(unsigned char x FMT_BUILTIN) : uint_value(x) {
    return {};
}
  constexpr FMT_INLINE value(signed short x) : int_value(x) {
    return {};
}
  constexpr FMT_INLINE value(unsigned short x FMT_BUILTIN) : uint_value(x) {
    return {};
}
  constexpr FMT_INLINE value(int x) : int_value(x) {
    return {};
}
  constexpr FMT_INLINE value(unsigned x FMT_BUILTIN) : uint_value(x) {
    return {};
}
  constexpr FMT_INLINE value(long x FMT_BUILTIN) : value(long_type(x)) {
    return {};
}
  constexpr FMT_INLINE value(unsigned long x FMT_BUILTIN)
      : value(ulong_type(x)) {
    return {};
}
  constexpr FMT_INLINE value(long long x FMT_BUILTIN) : long_long_value(x) {
    return {};
}
  constexpr FMT_INLINE value(ullong x FMT_BUILTIN) : ulong_long_value(x) {
    return {};
}
  FMT_INLINE value(native_int128 x FMT_BUILTIN) : int128_value(x) {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_INLINE value(native_uint128 x FMT_BUILTIN) : uint128_value(x) {
    throw std::runtime_error("STUB: not implemented");
}
  constexpr FMT_INLINE value(bool x FMT_BUILTIN) : bool_value(x) {
    return {};
}

  template <typename T, FMT_ENABLE_IF(is_code_unit<T>::value)>
  constexpr FMT_INLINE value(T x FMT_BUILTIN) : char_value(x) {
    return {};
}

  constexpr FMT_INLINE value(float x FMT_BUILTIN) : float_value(x) {
    return {};
}
  constexpr FMT_INLINE value(double x FMT_BUILTIN) : double_value(x) {
    return {};
}
  FMT_INLINE value(long double x FMT_BUILTIN) : long_double_value(x) {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_CONSTEXPR FMT_INLINE value(char_type* x FMT_BUILTIN) {
    return {};
}
  FMT_CONSTEXPR FMT_INLINE value(const char_type* x FMT_BUILTIN) {
    return {};
}
  template <typename T, typename C = char_t<T>,
            FMT_ENABLE_IF(!std::is_pointer<T>::value)>
  FMT_CONSTEXPR value(const T& x FMT_BUILTIN) {
    return {};
}
  constexpr FMT_INLINE value(void* x FMT_BUILTIN) : pointer(x) {
    return {};
}
  constexpr FMT_INLINE value(const void* x FMT_BUILTIN) : pointer(x) {
    return {};
}
  constexpr FMT_INLINE value(volatile void* x FMT_BUILTIN)
      : pointer(const_cast<const void*>(x)) {
    return {};
}
  constexpr FMT_INLINE value(const volatile void* x FMT_BUILTIN)
      : pointer(const_cast<const void*>(x)) {
    return {};
}
  constexpr FMT_INLINE value(nullptr_t) : pointer(nullptr) {
    return {};
}

  template <typename T,
            FMT_ENABLE_IF(
                (std::is_pointer<T>::value ||
                 std::is_member_pointer<T>::value) &&
                !std::is_void<typename std::remove_pointer<T>::type>::value)>
  constexpr value(const T&) {
    return {};
}

  template <typename T, FMT_ENABLE_IF(use_format_as<T>::value)>
  constexpr value(const T& x) : value(format_as(x)) {
    return {};
}
  template <typename T, FMT_ENABLE_IF(use_format_as_member<T>::value)>
  constexpr value(const T& x) : value(formatter<T>::format_as(x)) {
    return {};
}

  template <typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
  constexpr value(const T& named_arg) : value(named_arg.value) {
    return {};
}

  template <typename T,
            FMT_ENABLE_IF(use_formatter<T>::value || !FMT_BUILTIN_TYPES)>
  FMT_CONSTEXPR FMT_INLINE value(T& x) : value(x, custom_tag()) {
    return {};
}

  FMT_ALWAYS_INLINE value(const named_arg_info<char_type>* args, size_t size)
      : named_args{args, size} {
    throw std::runtime_error("STUB: not implemented");
}

 private:
  template <typename T, FMT_ENABLE_IF(has_formatter<T, char_type>())>
  FMT_CONSTEXPR value(T& x, custom_tag) {
    return {};
}

  template <typename T, FMT_ENABLE_IF(!has_formatter<T, char_type>())>
  FMT_CONSTEXPR value(const T&, custom_tag) {
    return {};
}

  // Formats an argument of a custom type, such as a user-defined class.
  template <typename T>
  static void format_custom(void* arg, parse_context<char_type>& parse_ctx,
                            Context& ctx) {
    throw std::runtime_error("STUB: not implemented");
}
};

enum { packed_arg_bits = 4 };
// Maximum number of arguments with packed types.
enum { max_packed_args = 62 / packed_arg_bits };
enum : ullong { is_unpacked_bit = 1ULL << 63 };
enum : ullong { has_named_args_bit = 1ULL << 62 };

template <typename It, typename T, typename Enable = void>
struct is_output_iterator : std::false_type {};

template <> struct is_output_iterator<appender, char> : std::true_type {};

template <typename It, typename T>
struct is_output_iterator<
    It, T,
    enable_if_t<std::is_assignable<decltype(*std::declval<decay_t<It>&>()++),
                                   T>::value>> : std::true_type {};

template <typename> constexpr auto encode_types() -> ullong {
    return {};
}

template <typename Context, typename First, typename... T>
constexpr auto encode_types() -> ullong {
    return {};
}

template <typename Context, typename... T, size_t NUM_ARGS = sizeof...(T)>
constexpr auto make_descriptor() -> ullong {
    return {};
}

template <typename Context, int NUM_ARGS>
using arg_t = conditional_t<NUM_ARGS <= max_packed_args, value<Context>,
                            basic_format_arg<Context>>;

template <typename Context, int NUM_ARGS, int NUM_NAMED_ARGS, ullong DESC>
struct named_arg_store {
  // args_[0].named_args points to named_args to avoid bloating format_args.
  arg_t<Context, NUM_ARGS> args[NUM_ARGS + 1u];
  named_arg_info<typename Context::char_type> named_args[NUM_NAMED_ARGS + 0u];

  template <typename... T>
  FMT_CONSTEXPR FMT_ALWAYS_INLINE named_arg_store(T&... values)
      : args{{named_args, NUM_NAMED_ARGS}, values...} {
    return {};
}

  named_arg_store(named_arg_store&& rhs) {
    args[0] = {named_args, NUM_NAMED_ARGS};
    for (size_t i = 1; i < sizeof(args) / sizeof(*args); ++i)
      args[i] = rhs.args[i];
    for (size_t i = 0; i < NUM_NAMED_ARGS; ++i)
      named_args[i] = rhs.named_args[i];
  }

  named_arg_store(const named_arg_store& rhs) = delete;
  auto operator=(const named_arg_store& rhs) -> named_arg_store& = delete;
  auto operator=(named_arg_store&& rhs) -> named_arg_store& = delete;
  operator const arg_t<Context, NUM_ARGS>*() const { return args + 1; }
};

// An array of references to arguments. It can be implicitly converted to
// `basic_format_args` for passing into type-erased formatting functions
// such as `vformat`. It is a plain struct to reduce binary size in debug mode.
template <typename Context, int NUM_ARGS, int NUM_NAMED_ARGS, ullong DESC>
struct format_arg_store {
  // +1 to workaround a bug in gcc 7.5 that causes duplicated-branches warning.
  using type =
      conditional_t<NUM_NAMED_ARGS == 0,
                    arg_t<Context, NUM_ARGS>[max_of<size_t>(1, NUM_ARGS)],
                    named_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC>>;
  type args;
};

// TYPE can be different from type_constant<T>, e.g. for __float128.
template <typename T, typename Char, type TYPE> struct native_formatter {
 private:
  dynamic_format_specs<Char> specs_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return {};
}

  template <type U = TYPE,
            FMT_ENABLE_IF(U == type::string_type || U == type::cstring_type ||
                          U == type::char_type)>
  FMT_CONSTEXPR void set_debug_format(bool set = true) {
    return {};
}

  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const T& val, FormatContext& ctx) const
      -> decltype(ctx.out());
};

template <bool B> constexpr bool enforce_compile_checks() {
    return {};
}

template <typename T = int> constexpr auto is_locking() -> bool {
    return {};
}
template <typename T1, typename T2, typename... Tail>
constexpr auto is_locking() -> bool {
    return {};
}

FMT_API void vformat_to(buffer<char>& buf, string_view fmt, format_args args,
                        locale_ref loc = {});

#if FMT_WIN32
FMT_API void vprint_mojibake(FILE*, string_view, format_args, bool);
#else  // format_args is passed by reference since it is defined later.
inline void vprint_mojibake(FILE*, string_view, const format_args&, bool) {
    throw std::runtime_error("STUB: not implemented");
}
#endif
}  // namespace detail

// The main public API.

template <typename T, typename Char = char>
using named_arg = detail::named_arg<T, Char>;

template <typename Char>
FMT_CONSTEXPR void parse_context<Char>::do_check_arg_id(int arg_id) {
    return {};
}

template <typename Char>
FMT_CONSTEXPR void parse_context<Char>::check_dynamic_spec(int arg_id) {
    return {};
}

FMT_BEGIN_EXPORT

// An output iterator that appends to a buffer. It is used instead of
// back_insert_iterator to reduce symbol sizes and avoid <iterator> dependency.
template <typename T> class basic_appender {
 protected:
  detail::buffer<T>* container;

 public:
  using container_type = detail::buffer<T>;

  constexpr basic_appender(detail::buffer<T>& buf) : container(&buf) {
    return {};
}

  FMT_CONSTEXPR auto operator=(T c) -> basic_appender& {
    return {};
}
  FMT_CONSTEXPR auto operator*() -> basic_appender& {
    return {};
}
  FMT_CONSTEXPR auto operator++() -> basic_appender& {
    return {};
}
  FMT_CONSTEXPR auto operator++(int) -> basic_appender {
    return {};
}
};

// A formatting argument. Context is a template parameter for the compiled API
// where output can be unbuffered.
template <typename Context> class basic_format_arg {
 private:
  detail::value<Context> value_;
  detail::type type_;

  friend class basic_format_args<Context>;

  using char_type = typename Context::char_type;

 public:
  class handle {
   private:
    detail::custom_value<Context> custom_;

   public:
    explicit handle(detail::custom_value<Context> custom) : custom_(custom) {
    throw std::runtime_error("STUB: not implemented");
}

    void format(parse_context<char_type>& parse_ctx, Context& ctx) const {
    throw std::runtime_error("STUB: not implemented");
}
  };

  constexpr basic_format_arg() : type_(detail::type::none_type) {
    return {};
}
  basic_format_arg(const detail::named_arg_info<char_type>* args, size_t size)
      : value_(args, size) {
    throw std::runtime_error("STUB: not implemented");
}
  template <typename T>
  basic_format_arg(T&& val)
      : value_(val), type_(detail::stored_type_constant<T, Context>::value) {
    throw std::runtime_error("STUB: not implemented");
}

  constexpr explicit operator bool() const noexcept {
    return {};
}
  auto type() const -> detail::type {
    throw std::runtime_error("STUB: not implemented");
}

  /**
   * Visits an argument dispatching to the appropriate visit method based on
   * the argument type. For example, if the argument type is `double` then
   * `vis(value)` will be called with the value of type `double`.
   */
  template <typename Visitor>
  FMT_CONSTEXPR FMT_INLINE auto visit(Visitor&& vis) const -> decltype(vis(0)) {
    return {};
}

  auto format_custom(const char_type* parse_begin,
                     parse_context<char_type>& parse_ctx, Context& ctx)
      -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

/**
 * A view of a collection of formatting arguments. To avoid lifetime issues it
 * should only be used as a parameter type in type-erased functions such as
 * `vformat`:
 *
 *     void vlog(fmt::string_view fmt, fmt::format_args args);  // OK
 *     fmt::format_args args = fmt::make_format_args();  // Dangling reference
 */
template <typename Context> class basic_format_args {
 private:
  // A descriptor that contains information about formatting arguments.
  // If the number of arguments is less or equal to max_packed_args then
  // argument types are passed in the descriptor. This reduces binary code size
  // per formatting function call.
  ullong desc_;
  union {
    // If is_packed() returns true then argument values are stored in values_;
    // otherwise they are stored in args_. This is done to improve cache
    // locality and reduce compiled code size since storing larger objects
    // may require more code (at least on x86-64) even if the same amount of
    // data is actually copied to stack. It saves ~10% on the bloat test.
    const detail::value<Context>* values_;
    const basic_format_arg<Context>* args_;
  };

  constexpr auto is_packed() const -> bool {
    return {};
}
  constexpr auto has_named_args() const -> bool {
    return {};
}

  FMT_CONSTEXPR auto type(int index) const -> detail::type {
    return {};
}

  template <int NUM_ARGS, int NUM_NAMED_ARGS, ullong DESC>
  using store =
      detail::format_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC>;

 public:
  using format_arg = basic_format_arg<Context>;

  constexpr basic_format_args() : desc_(0), args_(nullptr) {
    return {};
}

  /// Constructs a `basic_format_args` object from `format_arg_store`.
  template <int NUM_ARGS, int NUM_NAMED_ARGS, ullong DESC,
            FMT_ENABLE_IF(NUM_ARGS <= detail::max_packed_args)>
  constexpr FMT_ALWAYS_INLINE basic_format_args(
      const store<NUM_ARGS, NUM_NAMED_ARGS, DESC>& s)
      : desc_(DESC | (NUM_NAMED_ARGS != 0 ? +detail::has_named_args_bit : 0)),
        values_(s.args) {
    return {};
}

  template <int NUM_ARGS, int NUM_NAMED_ARGS, ullong DESC,
            FMT_ENABLE_IF(NUM_ARGS > detail::max_packed_args)>
  constexpr basic_format_args(const store<NUM_ARGS, NUM_NAMED_ARGS, DESC>& s)
      : desc_(DESC | (NUM_NAMED_ARGS != 0 ? +detail::has_named_args_bit : 0)),
        args_(s.args) {
    return {};
}

  /// Constructs a `basic_format_args` object from a dynamic list of arguments.
  constexpr basic_format_args(const format_arg* args, int count,
                              bool has_named = false)
      : desc_(detail::is_unpacked_bit | detail::to_unsigned(count) |
              (has_named ? +detail::has_named_args_bit : 0)),
        args_(args) {
    return {};
}

  /// Returns the argument with the specified id.
  FMT_CONSTEXPR auto get(int id) const -> format_arg {
    return {};
}

  template <typename Char>
  auto get(basic_string_view<Char> name) const -> format_arg {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Char>
  FMT_CONSTEXPR auto get_id(basic_string_view<Char> name) const -> int {
    return {};
}

  auto max_size() const -> int {
    throw std::runtime_error("STUB: not implemented");
}
};

// A formatting context.
class context {
 private:
  appender out_;
  format_args args_;
  FMT_NO_UNIQUE_ADDRESS locale_ref loc_;

 public:
  using char_type = char;  ///< The character type for the output.
  using iterator = appender;
  using format_arg = basic_format_arg<context>;
  enum { builtin_types = FMT_BUILTIN_TYPES };

  /// Constructs a `context` object. References to the arguments are stored
  /// in the object so make sure they have appropriate lifetimes.
  constexpr context(iterator out, format_args args, locale_ref loc = {})
      : out_(out), args_(args), loc_(loc) {
    return {};
}
  context(context&&) = default;
  context(const context&) = delete;
  void operator=(const context&) = delete;

  FMT_CONSTEXPR auto arg(int id) const -> format_arg {
    return {};
}
  inline auto arg(string_view name) const -> format_arg {
    throw std::runtime_error("STUB: not implemented");
}
  FMT_CONSTEXPR auto arg_id(string_view name) const -> int {
    return {};
}
  auto args() const -> const format_args& {
    throw std::runtime_error("STUB: not implemented");
}

  // Returns an iterator to the beginning of the output range.
  constexpr auto out() const -> iterator {
    return {};
}

  // Advances the begin iterator to `it`.
  FMT_CONSTEXPR void advance_to(iterator) {
    return {};
}

  constexpr auto locale() const -> locale_ref {
    return {};
}
};

template <typename Char = char> struct runtime_format_string {
  basic_string_view<Char> str;
};

/**
 * Creates a runtime format string.
 *
 * **Example**:
 *
 *     // Check format string at runtime instead of compile-time.
 *     fmt::print(fmt::runtime("{:d}"), "I am not a number");
 */
inline auto runtime(string_view s) -> runtime_format_string<> {
    throw std::runtime_error("STUB: not implemented");
}

/// A compile-time format string. Use `format_string` in the public API to
/// prevent type deduction.
template <typename... T> struct fstring {
 private:
  static constexpr int num_static_named_args =
      detail::count_static_named_args<T...>();

  using checker = detail::format_string_checker<
      char, int(sizeof...(T)), num_static_named_args,
      num_static_named_args != detail::count_named_args<T...>()>;

  using arg_pack = detail::arg_pack<T...>;

 public:
  string_view str;
  using t = fstring;

  // Reports a compile-time error if S is not a valid format string for T.
  template <size_t N>
  FMT_CONSTEVAL FMT_ALWAYS_INLINE fstring(const char (&s)[N]) : str(s, N - 1) {
    throw std::runtime_error("STUB: not implemented");
}
  template <typename S,
            FMT_ENABLE_IF(std::is_convertible<const S&, string_view>::value)>
  FMT_CONSTEVAL FMT_ALWAYS_INLINE fstring(const S& s) : str(s) {
    throw std::runtime_error("STUB: not implemented");
}
  template <typename S,
            FMT_ENABLE_IF(std::is_base_of<detail::compile_string, S>::value&&
                              std::is_same<typename S::char_type, char>::value)>
  FMT_ALWAYS_INLINE fstring(const S&) : str(S()) {
    throw std::runtime_error("STUB: not implemented");
}
  fstring(runtime_format_string<> fmt) : str(fmt.str) {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_DEPRECATED operator const string_view&() const {
    throw std::runtime_error("STUB: not implemented");
}
  auto get() const -> string_view {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename... T> using format_string = typename fstring<T...>::t;

template <typename T, typename Char = char>
using is_formattable = bool_constant<!std::is_same<
    detail::mapped_t<conditional_t<std::is_void<T>::value, int*, T>, Char>,
    void>::value>;
#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
template <typename T, typename Char = char>
concept formattable = is_formattable<remove_reference_t<T>, Char>::value;
#endif

// A formatter specialization for natively supported types.
template <typename T, typename Char>
struct formatter<T, Char,
                 enable_if_t<detail::type_constant<T, Char>::value !=
                             detail::type::custom_type>>
    : detail::native_formatter<T, Char, detail::type_constant<T, Char>::value> {
};

/**
 * Constructs an object that stores references to arguments and can be
 * implicitly converted to `format_args`. `Context` can be omitted in which case
 * it defaults to `context`. See `arg` for lifetime considerations.
 */
// Take arguments by lvalue references to avoid some lifetime issues, e.g.
//   auto args = make_format_args(std::string());
template <typename Context = context, typename... T,
          int NUM_ARGS = int(sizeof...(T)),
          int NUM_NAMED_ARGS = detail::count_named_args<T...>(),
          ullong DESC = detail::make_descriptor<Context, T...>()>
constexpr FMT_ALWAYS_INLINE auto make_format_args(T&... args)
    -> detail::format_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC> {
    return {};
}

template <typename... T>
using vargs =
    detail::format_arg_store<context, int(sizeof...(T)),
                             detail::count_named_args<T...>(),
                             detail::make_descriptor<context, T...>()>;

/**
 * Returns a named argument to be used in a formatting function.
 * It should only be used in a call to a formatting function.
 *
 * **Example**:
 *
 *     fmt::print("The answer is {answer}.", fmt::arg("answer", 42));
 *
 * Named arguments passed with `fmt::arg` are not supported
 * in compile-time checks, but `"answer"_a=42` are compile-time checked in
 * sufficiently new compilers. See `operator""_a()`.
 */
template <typename T>
inline auto arg(const char* name, const T& arg) -> named_arg<T> {
    throw std::runtime_error("STUB: not implemented");
}

/// Formats a string and writes the output to `out`.
template <typename OutputIt,
          FMT_ENABLE_IF(detail::is_output_iterator<remove_cvref_t<OutputIt>,
                                                   char>::value)>
// DEPRECATED! Passing out as a forwarding reference.
auto vformat_to(OutputIt&& out, string_view fmt, format_args args)
    -> remove_cvref_t<OutputIt> {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt`, writes the result to
 * the output iterator `out` and returns the iterator past the end of the output
 * range. `format_to` does not append a terminating null character.
 *
 * **Example**:
 *
 *     auto out = std::vector<char>();
 *     fmt::format_to(std::back_inserter(out), "{}", 42);
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<remove_cvref_t<OutputIt>,
                                                   char>::value)>
FMT_INLINE auto format_to(OutputIt&& out, format_string<T...> fmt, T&&... args)
    -> remove_cvref_t<OutputIt> {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt> struct format_to_n_result {
  OutputIt out;  ///< Iterator past the end of the output range.
  size_t size;   ///< Total (not truncated) output size.
};

template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to_n(OutputIt out, size_t n, string_view fmt, format_args args)
    -> format_to_n_result<OutputIt> {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt`, writes up to `n`
 * characters of the result to the output iterator `out` and returns the total
 * (not truncated) output size and the iterator past the end of the output
 * range. `format_to_n` does not append a terminating null character.
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
FMT_INLINE auto format_to_n(OutputIt out, size_t n, format_string<T...> fmt,
                            T&&... args) -> format_to_n_result<OutputIt> {
    throw std::runtime_error("STUB: not implemented");
}

struct format_to_result {
  char* out;       ///< Pointer to just after the last successful write.
  bool truncated;  ///< Specifies if the output was truncated.

  FMT_CONSTEXPR operator char*() const {
    return {};
}
};

template <size_t N>
FMT_DEPRECATED auto vformat_to(char (&out)[N], string_view fmt,
                               format_args args) -> format_to_result {
    throw std::runtime_error("STUB: not implemented");
}

template <size_t N, typename... T>
FMT_INLINE auto format_to(char (&out)[N], format_string<T...> fmt, T&&... args)
    -> format_to_result {
    throw std::runtime_error("STUB: not implemented");
}

/// Returns the number of chars in the output of `format(fmt, args...)`.
template <typename... T>
FMT_NODISCARD FMT_INLINE auto formatted_size(format_string<T...> fmt,
                                             T&&... args) -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

FMT_API void vprint(string_view fmt, format_args args);
FMT_API void vprint(FILE* f, string_view fmt, format_args args);
FMT_API void vprintln(FILE* f, string_view fmt, format_args args);
FMT_API void vprint_buffered(FILE* f, string_view fmt, format_args args);

/**
 * Formats `args` according to specifications in `fmt` and writes the output
 * to `stdout`.
 *
 * **Example**:
 *
 *     fmt::print("The answer is {}.", 42);
 */
template <typename... T>
FMT_INLINE void print(format_string<T...> fmt, T&&... args) {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt` and writes the
 * output to the file `f`.
 *
 * **Example**:
 *
 *     fmt::print(stderr, "Don't {}!", "panic");
 */
template <typename... T>
FMT_INLINE void print(FILE* f, format_string<T...> fmt, T&&... args) {
    throw std::runtime_error("STUB: not implemented");
}

/// Formats `args` according to specifications in `fmt` and writes the output
/// to the file `f` followed by a newline.
template <typename... T>
FMT_INLINE void println(FILE* f, format_string<T...> fmt, T&&... args) {
    throw std::runtime_error("STUB: not implemented");
}

/// Formats `args` according to specifications in `fmt` and writes the output
/// to `stdout` followed by a newline.
template <typename... T>
FMT_INLINE void println(format_string<T...> fmt, T&&... args) {
    throw std::runtime_error("STUB: not implemented");
}

FMT_PRAGMA_GCC(pop_options)
FMT_END_EXPORT
FMT_END_NAMESPACE

#ifdef FMT_HEADER_ONLY
#  include "format.h"
#endif
#endif  // FMT_BASE_H_
