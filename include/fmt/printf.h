#include <stdexcept>
#include <cstdlib>
// Formatting library for C++ - legacy printf implementation
//
// Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_PRINTF_H_
#define FMT_PRINTF_H_

#ifndef FMT_MODULE
#  include <algorithm>  // std::find
#  include <limits>     // std::numeric_limits
#endif

#include "format.h"

FMT_BEGIN_NAMESPACE
FMT_BEGIN_EXPORT

template <typename Char> class basic_printf_context {
 private:
  basic_appender<Char> out_;
  basic_format_args<basic_printf_context> args_;

  static_assert(std::is_same<Char, char>::value ||
                    std::is_same<Char, wchar_t>::value,
                "Unsupported code unit type.");

 public:
  using char_type = Char;
  enum { builtin_types = 1 };

  /// Constructs a `printf_context` object. References to the arguments are
  /// stored in the context object so make sure they have appropriate lifetimes.
  basic_printf_context(basic_appender<Char> out,
                       basic_format_args<basic_printf_context> args)
      : out_(out), args_(args) {
    throw std::runtime_error("STUB: not implemented");
}

  auto out() -> basic_appender<Char> {
    throw std::runtime_error("STUB: not implemented");
}
  void advance_to(basic_appender<Char>) {
    throw std::runtime_error("STUB: not implemented");
}

  auto locale() -> locale_ref {
    throw std::runtime_error("STUB: not implemented");
}

  auto arg(int id) const -> basic_format_arg<basic_printf_context> {
    throw std::runtime_error("STUB: not implemented");
}
};

namespace detail {

// Return the result via the out param to workaround gcc bug 77539.
template <bool IS_CONSTEXPR, typename T, typename Ptr = const T*>
FMT_CONSTEXPR auto find(Ptr first, Ptr last, T value, Ptr& out) -> bool {
    return {};
}

template <>
inline auto find<false, char>(const char* first, const char* last, char value,
                              const char*& out) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

// Checks if a value fits in int - used to avoid warnings about comparing
// signed and unsigned integers.
template <bool IS_SIGNED> struct int_checker {
  template <typename T> static auto fits_in_int(T value) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  inline static auto fits_in_int(bool) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

template <> struct int_checker<true> {
  template <typename T> static auto fits_in_int(T value) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
  inline static auto fits_in_int(int) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

struct printf_precision_handler {
  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  auto operator()(T value) -> int {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  auto operator()(T) -> int {
    throw std::runtime_error("STUB: not implemented");
}
};

// An argument visitor that returns true iff arg is a zero integer.
struct is_zero_int {
  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  auto operator()(T value) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  auto operator()(T) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename T> struct make_unsigned_or_bool : std::make_unsigned<T> {};

template <> struct make_unsigned_or_bool<bool> {
  using type = bool;
};

template <typename T, typename Context> class arg_converter {
 private:
  using char_type = typename Context::char_type;

  basic_format_arg<Context>& arg_;
  char_type type_;

 public:
  arg_converter(basic_format_arg<Context>& arg, char_type type)
      : arg_(arg), type_(type) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(bool value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename U, FMT_ENABLE_IF(std::is_integral<U>::value)>
  void operator()(U value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename U, FMT_ENABLE_IF(!std::is_integral<U>::value)>
  void operator()(U) {
    throw std::runtime_error("STUB: not implemented");
}  // No conversion needed for non-integral types.
};

// Converts an integer argument to T for printf, if T is an integral type.
// If T is void, the argument is converted to corresponding signed or unsigned
// type depending on the type specifier: 'd' and 'i' - signed, other -
// unsigned).
template <typename T, typename Context, typename Char>
void convert_arg(basic_format_arg<Context>& arg, Char type) {
    throw std::runtime_error("STUB: not implemented");
}

// Converts an integer argument to char for printf.
template <typename Context> class char_converter {
 private:
  basic_format_arg<Context>& arg_;

 public:
  explicit char_converter(basic_format_arg<Context>& arg) : arg_(arg) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  void operator()(T value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  void operator()(T) {
    throw std::runtime_error("STUB: not implemented");
}  // No conversion needed for non-integral types.
};

// An argument visitor that return a pointer to a C string if argument is a
// string or null otherwise.
template <typename Char> struct get_cstring {
  template <typename T> auto operator()(T) -> const Char* {
    throw std::runtime_error("STUB: not implemented");
}
  auto operator()(const Char* s) -> const Char* {
    throw std::runtime_error("STUB: not implemented");
}
};

// Checks if an argument is a valid printf width specifier and sets
// left alignment if it is negative.
class printf_width_handler {
 private:
  format_specs& specs_;

 public:
  inline explicit printf_width_handler(format_specs& specs) : specs_(specs) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  auto operator()(T value) -> unsigned {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
  auto operator()(T) -> unsigned {
    throw std::runtime_error("STUB: not implemented");
}
};

// Workaround for a bug with the XL compiler when initializing
// printf_arg_formatter's base class.
template <typename Char>
auto make_arg_formatter(basic_appender<Char> iter, format_specs& s)
    -> arg_formatter<Char> {
    throw std::runtime_error("STUB: not implemented");
}

// The `printf` argument formatter.
template <typename Char>
class printf_arg_formatter : public arg_formatter<Char> {
 private:
  using base = arg_formatter<Char>;
  using context_type = basic_printf_context<Char>;

  context_type& context_;

  void write_null_pointer(bool is_string = false) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T> void write(T value) {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  printf_arg_formatter(basic_appender<Char> iter, format_specs& s,
                       context_type& ctx)
      : base(make_arg_formatter(iter, s)), context_(ctx) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(monostate value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(detail::is_integral<T>::value)>
  void operator()(T value) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
  void operator()(T value) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(const char* value) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(const wchar_t* value) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(basic_string_view<Char> value) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(const void* value) {
    throw std::runtime_error("STUB: not implemented");
}

  void operator()(typename basic_format_arg<context_type>::handle handle) {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename Char>
void parse_flags(format_specs& specs, const Char*& it, const Char* end) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename GetArg>
auto parse_header(const Char*& it, const Char* end, format_specs& specs,
                  GetArg get_arg) -> int {
    throw std::runtime_error("STUB: not implemented");
}

inline auto parse_printf_presentation_type(char c, type t, bool& upper)
    -> presentation_type {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename Context>
void vprintf(buffer<Char>& buf, basic_string_view<Char> format,
             basic_format_args<Context> args) {
    throw std::runtime_error("STUB: not implemented");
}
}  // namespace detail

using printf_context = basic_printf_context<char>;
using wprintf_context = basic_printf_context<wchar_t>;

using printf_args = basic_format_args<printf_context>;
using wprintf_args = basic_format_args<wprintf_context>;

/// Constructs an `format_arg_store` object that contains references to
/// arguments and can be implicitly converted to `printf_args`.
template <typename Char = char, typename... T>
inline auto make_printf_args(T&... args)
    -> decltype(fmt::make_format_args<basic_printf_context<Char>>(args...)) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char> struct vprintf_args {
  using type = basic_format_args<basic_printf_context<Char>>;
};

template <typename Char>
inline auto vsprintf(basic_string_view<Char> fmt,
                     typename vprintf_args<Char>::type args)
    -> std::basic_string<Char> {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt` and returns the result
 * as string.
 *
 * **Example**:
 *
 *     std::string message = fmt::sprintf("The answer is %d", 42);
 */
template <typename... T>
inline auto sprintf(string_view fmt, const T&... args) -> std::string {
    throw std::runtime_error("STUB: not implemented");
}
template <typename... T>
FMT_DEPRECATED auto sprintf(basic_string_view<wchar_t> fmt, const T&... args)
    -> std::wstring {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char>
auto vfprintf(std::FILE* f, basic_string_view<Char> fmt,
              typename vprintf_args<Char>::type args) -> int {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt` and writes the output
 * to `f`.
 *
 * **Example**:
 *
 *     fmt::fprintf(stderr, "Don't %s!", "panic");
 */
template <typename... T>
inline auto fprintf(std::FILE* f, string_view fmt, const T&... args) -> int {
    throw std::runtime_error("STUB: not implemented");
}
template <typename... T>
FMT_DEPRECATED auto fprintf(std::FILE* f, basic_string_view<wchar_t> fmt,
                            const T&... args) -> int {
    throw std::runtime_error("STUB: not implemented");
}

/**
 * Formats `args` according to specifications in `fmt` and writes the output
 * to `stdout`.
 *
 * **Example**:
 *
 *   fmt::printf("Elapsed time: %.2f seconds", 1.23);
 */
template <typename... T>
inline auto printf(string_view fmt, const T&... args) -> int {
    throw std::runtime_error("STUB: not implemented");
}

FMT_END_EXPORT
FMT_END_NAMESPACE

#endif  // FMT_PRINTF_H_
