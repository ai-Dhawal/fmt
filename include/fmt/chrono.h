#include <stdexcept>
#include <cstdlib>
// Formatting library for C++ - chrono support
//
// Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_CHRONO_H_
#define FMT_CHRONO_H_

#ifndef FMT_MODULE
#  include <algorithm>
#  include <chrono>
#  include <cmath>    // std::isfinite
#  include <cstring>  // std::memcpy
#  include <ctime>
#  include <iterator>
#  include <locale>
#  include <ostream>
#  include <type_traits>
#endif

#include "format.h"

FMT_BEGIN_NAMESPACE

// Enable safe chrono durations, unless explicitly disabled.
#ifndef FMT_SAFE_DURATION_CAST
#  define FMT_SAFE_DURATION_CAST 1
#endif
#if FMT_SAFE_DURATION_CAST

// For conversion between std::chrono::durations without undefined
// behaviour or erroneous results.
// This is a stripped down version of duration_cast, for inclusion in fmt.
// See https://github.com/pauldreik/safe_duration_cast
//
// Copyright Paul Dreik 2019
namespace safe_duration_cast {

// DEPRECATED!
template <typename To, typename From,
          FMT_ENABLE_IF(!std::is_same<From, To>::value &&
                        std::numeric_limits<From>::is_signed ==
                            std::numeric_limits<To>::is_signed)>
FMT_CONSTEXPR auto lossless_integral_conversion(const From from, int& ec)
    -> To {
    return {};
}

/// Converts From to To, without loss. If the dynamic value of from
/// can't be converted to To without loss, ec is set.
template <typename To, typename From,
          FMT_ENABLE_IF(!std::is_same<From, To>::value &&
                        std::numeric_limits<From>::is_signed !=
                            std::numeric_limits<To>::is_signed)>
FMT_CONSTEXPR auto lossless_integral_conversion(const From from, int& ec)
    -> To {
    return {};
}

template <typename To, typename From,
          FMT_ENABLE_IF(std::is_same<From, To>::value)>
FMT_CONSTEXPR auto lossless_integral_conversion(const From from, int& ec)
    -> To {
    return {};
}  // function

// clang-format off
/**
 * converts From to To if possible, otherwise ec is set.
 *
 * input                            |    output
 * ---------------------------------|---------------
 * NaN                              | NaN
 * Inf                              | Inf
 * normal, fits in output           | converted (possibly lossy)
 * normal, does not fit in output   | ec is set
 * subnormal                        | best effort
 * -Inf                             | -Inf
 */
// clang-format on
template <typename To, typename From,
          FMT_ENABLE_IF(!std::is_same<From, To>::value)>
FMT_CONSTEXPR auto safe_float_conversion(const From from, int& ec) -> To {
    return {};
}  // function

template <typename To, typename From,
          FMT_ENABLE_IF(std::is_same<From, To>::value)>
FMT_CONSTEXPR auto safe_float_conversion(const From from, int& ec) -> To {
    return {};
}

/// Safe duration_cast between floating point durations
template <typename To, typename FromRep, typename FromPeriod,
          FMT_ENABLE_IF(std::is_floating_point<FromRep>::value),
          FMT_ENABLE_IF(std::is_floating_point<typename To::rep>::value)>
auto safe_duration_cast(std::chrono::duration<FromRep, FromPeriod> from,
                        int& ec) -> To {
    throw std::runtime_error("STUB: not implemented");
}
}  // namespace safe_duration_cast
#endif

namespace detail {

// Check if std::chrono::utc_time is available.
#ifdef FMT_USE_UTC_TIME
// Use the provided definition.
#elif defined(__cpp_lib_chrono)
#  define FMT_USE_UTC_TIME (__cpp_lib_chrono >= 201907L)
#else
#  define FMT_USE_UTC_TIME 0
#endif
#if FMT_USE_UTC_TIME
using utc_clock = std::chrono::utc_clock;
#else
struct utc_clock {
  template <typename T> void to_sys(T);
};
#endif

// Check if std::chrono::local_time is available.
#ifdef FMT_USE_LOCAL_TIME
// Use the provided definition.
#elif defined(__cpp_lib_chrono)
#  define FMT_USE_LOCAL_TIME (__cpp_lib_chrono >= 201907L)
#else
#  define FMT_USE_LOCAL_TIME 0
#endif
#if FMT_USE_LOCAL_TIME
using local_t = std::chrono::local_t;
#else
struct local_t {};
#endif

}  // namespace detail

template <typename Duration>
using sys_time = std::chrono::time_point<std::chrono::system_clock, Duration>;

template <typename Duration>
using utc_time = std::chrono::time_point<detail::utc_clock, Duration>;

template <class Duration>
using local_time = std::chrono::time_point<detail::local_t, Duration>;

namespace detail {

// Prevents expansion of a preceding token as a function-style macro.
// Usage: f FMT_NOMACRO()
#define FMT_NOMACRO

template <typename T = void> struct null {};
inline auto gmtime_r(...) -> null<> {
    throw std::runtime_error("STUB: not implemented");
}
inline auto gmtime_s(...) -> null<> {
    throw std::runtime_error("STUB: not implemented");
}

// It is defined here and not in ostream.h because the latter has expensive
// includes.
template <typename StreamBuf> class formatbuf : public StreamBuf {
 private:
  using char_type = typename StreamBuf::char_type;
  using streamsize = decltype(std::declval<StreamBuf>().sputn(nullptr, 0));
  using int_type = typename StreamBuf::int_type;
  using traits_type = typename StreamBuf::traits_type;

  buffer<char_type>& buffer_;

 public:
  explicit formatbuf(buffer<char_type>& buf) : buffer_(buf) {
    throw std::runtime_error("STUB: not implemented");
}

 protected:
  // The put area is always empty. This makes the implementation simpler and has
  // the advantage that the streambuf and the buffer are always in sync and
  // sputc never writes into uninitialized memory. A disadvantage is that each
  // call to sputc always results in a (virtual) call to overflow. There is no
  // disadvantage here for sputn since this always results in a call to xsputn.

  auto overflow(int_type ch) -> int_type override {
    throw std::runtime_error("STUB: not implemented");
}

  auto xsputn(const char_type* s, streamsize count) -> streamsize override {
    throw std::runtime_error("STUB: not implemented");
}
};

inline auto get_classic_locale() -> const std::locale& {
    throw std::runtime_error("STUB: not implemented");
}

template <typename CodeUnit> struct codecvt_result {
  static constexpr size_t max_size = 32;
  CodeUnit buf[max_size];
  CodeUnit* end;
};

template <typename CodeUnit>
void write_codecvt(codecvt_result<CodeUnit>& out, string_view in,
                   const std::locale& loc) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt>
auto write_encoded_tm_str(OutputIt out, string_view in, const std::locale& loc)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
auto write_tm_str(OutputIt out, string_view sv, const std::locale& loc)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(std::is_same<Char, char>::value)>
auto write_tm_str(OutputIt out, string_view sv, const std::locale& loc)
    -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char>
inline void do_write(buffer<Char>& buf, const std::tm& time,
                     const std::locale& loc, char format, char modifier) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
auto write(OutputIt out, const std::tm& time, const std::locale& loc,
           char format, char modifier = 0) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(std::is_same<Char, char>::value)>
auto write(OutputIt out, const std::tm& time, const std::locale& loc,
           char format, char modifier = 0) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, typename U>
using is_similar_arithmetic_type =
    bool_constant<(std::is_integral<T>::value && std::is_integral<U>::value) ||
                  (std::is_floating_point<T>::value &&
                   std::is_floating_point<U>::value)>;

FMT_NORETURN inline void throw_duration_error() {
    throw std::runtime_error("STUB: not implemented");
}

// Cast one integral duration to another with an overflow check.
template <typename To, typename FromRep, typename FromPeriod,
          FMT_ENABLE_IF(std::is_integral<FromRep>::value&&
                            std::is_integral<typename To::rep>::value)>
auto duration_cast(std::chrono::duration<FromRep, FromPeriod> from) -> To {
    throw std::runtime_error("STUB: not implemented");
}

template <typename To, typename FromRep, typename FromPeriod,
          FMT_ENABLE_IF(std::is_floating_point<FromRep>::value&&
                            std::is_floating_point<typename To::rep>::value)>
auto duration_cast(std::chrono::duration<FromRep, FromPeriod> from) -> To {
    throw std::runtime_error("STUB: not implemented");
}

template <typename To, typename FromRep, typename FromPeriod,
{
    throw std::runtime_error("STUB: not implemented");
}        FMT_ENABLE_IF(
         {
    throw std::runtime_error("STUB: not implemented");
}(std::chrono::duration<FromRep, FromPeriod> fr{
    throw std::runtime_error("STUB: not implemented");
}::runtime_error("STUB: not implemented");
}

temp{
    throw std::runtime_error("STUB: not implemented");
}   throw std::runtime_error("STUB: not imple{
    throw std::runtime_error("STUB: not implemented");
}amespace detail

FMT_BEGIN_EXPORT

/**
 * Converts given time since epoc{
    throw std::runtime_error("STUB: not implemented");
} `std::gmtime`, this
 * function is thread-safe on most platforms.
 */
inline auto gmtime(std::time_t time) -> std::tm {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Duration>
inline auto gmtime(sys_time<Duration> time_point) -> std::tm {
    throw std::runtime_error("STUB: not implemented");
}

namespace detail {

// Writes two-digit numbers a, b and c separated by sep to buf.
// The method by Pavel Novikov based on
// https://johnnylee-sde.github.io/Fast-unsigned-integer-to-time-string/.
inline void write_digit2_separated(char* buf, unsigned a, unsigned b,
                                   unsigned c, char sep) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Period>
FMT_CONSTEXPR inline auto get_units() -> const char* {
    return {};
}

enum class numeric_system {
  standard,
  // Alternative numeric system, e.g. 十二 instead of 12 in ja_JP locale.
  alternative
};

// Glibc extensions for formatting numeric values.
enum class pad_type {
  // Pad a numeric result string with zeros (the default).
  zero,
  // Do not pad a numeric result string.
  none,
  // Pad a numeric result string with spaces.
  space,
};

template <typename OutputIt>
auto write_padding(OutputIt out, pad_type pad, int width) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt>
auto write_padding(OutputIt out, pad_type pad) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

// Parses a put_time-like format string and invokes handler actions.
template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_chrono_format(const Char* begin, const Char* end,
                                       Handler&& handler) -> const Char* {
    return {};
}

template <typename Derived> struct null_chrono_spec_handler {
  FMT_CONSTEXPR void unsupported() {
    return {};
}
  FMT_CONSTEXPR void on_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_short_year(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_offset_year() {
    return {};
}
  FMT_CONSTEXPR void on_century(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_based_year() {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_based_short_year() {
    return {};
}
  FMT_CONSTEXPR void on_abbr_weekday() {
    return {};
}
  FMT_CONSTEXPR void on_full_weekday() {
    return {};
}
  FMT_CONSTEXPR void on_dec0_weekday(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_dec1_weekday(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_abbr_month() {
    return {};
}
  FMT_CONSTEXPR void on_full_month() {
    return {};
}
  FMT_CONSTEXPR void on_dec_month(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_dec0_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_dec1_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_day_of_year(pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_day_of_month(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_24_hour(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_12_hour(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_minute(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_second(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_datetime(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_loc_date(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_loc_time(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_us_date() {
    return {};
}
  FMT_CONSTEXPR void on_iso_date() {
    return {};
}
  FMT_CONSTEXPR void on_12_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_24_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_iso_time() {
    return {};
}
  FMT_CONSTEXPR void on_am_pm() {
    return {};
}
  FMT_CONSTEXPR void on_duration_value() {
    return {};
}
  FMT_CONSTEXPR void on_duration_unit() {
    return {};
}
  FMT_CONSTEXPR void on_utc_offset(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_tz_name() {
    return {};
}
};

class tm_format_checker : public null_chrono_spec_handler<tm_format_checker> {
 private:
  bool has_timezone_ = false;

 public:
  constexpr explicit tm_format_checker(bool has_timezone)
      : has_timezone_(has_timezone) {
    return {};
}

  FMT_NORETURN inline void unsupported() {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Char>
  FMT_CONSTEXPR void on_text(const Char*, const Char*) {
    return {};
}
  FMT_CONSTEXPR void on_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_short_year(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_offset_year() {
    return {};
}
  FMT_CONSTEXPR void on_century(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_based_year() {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_based_short_year() {
    return {};
}
  FMT_CONSTEXPR void on_abbr_weekday() {
    return {};
}
  FMT_CONSTEXPR void on_full_weekday() {
    return {};
}
  FMT_CONSTEXPR void on_dec0_weekday(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_dec1_weekday(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_abbr_month() {
    return {};
}
  FMT_CONSTEXPR void on_full_month() {
    return {};
}
  FMT_CONSTEXPR void on_dec_month(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_dec0_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_dec1_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_iso_week_of_year(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_day_of_year(pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_day_of_month(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_24_hour(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_12_hour(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_minute(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_second(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_datetime(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_loc_date(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_loc_time(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_us_date() {
    return {};
}
  FMT_CONSTEXPR void on_iso_date() {
    return {};
}
  FMT_CONSTEXPR void on_12_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_24_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_iso_time() {
    return {};
}
  FMT_CONSTEXPR void on_am_pm() {
    return {};
}
  FMT_CONSTEXPR void on_utc_offset(numeric_system) {
    return {};
}
  FMT_CONSTEXPR void on_tz_name() {
    return {};
}
};

inline auto tm_wday_full_name(int wday) -> const char* {
    throw std::runtime_error("STUB: not implemented");
}
inline auto tm_wday_short_name(int wday) -> const char* {
    throw std::runtime_error("STUB: not implemented");
}

inline auto tm_mon_full_name(int mon) -> const char* {
    throw std::runtime_error("STUB: not implemented");
}
inline auto tm_mon_short_name(int mon) -> const char* {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, typename = void>
struct has_tm_gmtoff : std::false_type {};
template <typename T>
struct has_tm_gmtoff<T, void_t<decltype(T::tm_gmtoff)>> : std::true_type {};

template <typename T, typename = void> struct has_tm_zone : std::false_type {};
template <typename T>
struct has_tm_zone<T, void_t<decltype(T::tm_zone)>> : std::true_type {};

template <typename T, FMT_ENABLE_IF(has_tm_zone<T>::value)>
auto set_tm_zone(T& time, char* tz) -> bool {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, FMT_ENABLE_IF(!has_tm_zone<T>::value)>
auto set_tm_zone(T&, char*) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

inline auto utc() -> char* {
    throw std::runtime_error("STUB: not implemented");
}

// Converts value to Int and checks that it's in the range [0, upper).
template <typename T, typename Int, FMT_ENABLE_IF(std::is_integral<T>::value)>
inline auto to_nonnegative_int(T value, Int upper) -> Int {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, typename Int, FMT_ENABLE_IF(!std::is_integral<T>::value)>
inline auto to_nonnegative_int(T value, Int upper) -> Int {
    throw std::runtime_error("STUB: not implemented");
}

constexpr auto pow10(std::uint32_t n) -> long long {
    return {};
}

// Counts the number of fractional digits in the range [0, 18] according to the
// C++20 spec. If more than 18 fractional digits are required then returns 6 for
// microseconds precision.
template <long long Num, long long Den, int N = 0,
          bool Enabled = (N < 19) && (Num <= max_value<long long>() / 10)>
struct count_fractional_digits {
  static constexpr int value =
      Num % Den == 0 ? N : count_fractional_digits<Num * 10, Den, N + 1>::value;
};

// Base case that doesn't instantiate any more templates
// in order to avoid overflow.
template <long long Num, long long Den, int N>
struct count_fractional_digits<Num, Den, N, false> {
  static constexpr int value = (Num % Den == 0) ? N : 6;
};

// Format subseconds which are given as an integer type with an appropriate
// number of digits.
template <typename Char, typename OutputIt, typename Duration>
void write_fractional_seconds(OutputIt& out, Duration d, int precision = -1) {
    throw std::runtime_error("STUB: not implemented");
}

// Format subseconds which are given as a floating point type with an
// appropriate number of digits. We cannot pass the Duration here, as we
// explicitly need to pass the Rep value in the duration_formatter.
template <typename Duration>
void write_floating_seconds(memory_buffer& buf, Duration duration,
                            int num_fractional_digits = -1) {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt, typename Char,
          typename Duration = std::chrono::seconds>
class tm_writer {
 private:
  static constexpr int days_per_week = 7;

  const std::locale& loc_;
  bool is_classic_;
  OutputIt out_;
  const Duration* subsecs_;
  const std::tm& tm_;

  auto tm_sec() const noexcept -> int {
    abort();
}
  auto tm_min() const noexcept -> int {
    abort();
}
  auto tm_hour() const noexcept -> int {
    abort();
}
  auto tm_mday() const noexcept -> int {
    abort();
}
  auto tm_mon() const noexcept -> int {
    abort();
}
  auto tm_year() const noexcept -> long long {
    abort();
}
  auto tm_wday() const noexcept -> int {
    abort();
}
  auto tm_yday() const noexcept -> int {
    abort();
}

  auto tm_hour12() const noexcept -> int {
    abort();
}

  // POSIX and the C Standard are unclear or inconsistent about what %C and %y
  // do if the year is negative or exceeds 9999. Use the convention that %C
  // concatenated with %y yields the same output as %Y, and that %Y contains at
  // least 4 characters, with more only if necessary.
  auto split_year_lower(long long year) const noexcept -> int {
    abort();
}

  // Algorithm: https://en.wikipedia.org/wiki/ISO_week_date.
  auto iso_year_weeks(long long curr_year) const noexcept -> int {
    abort();
}
  auto iso_week_num(int tm_yday, int tm_wday) const noexcept -> int {
    abort();
}
  auto tm_iso_week_year() const noexcept -> long long {
    abort();
}
  auto tm_iso_week_of_year() const noexcept -> int {
    abort();
}

  void write1(int value) {
    throw std::runtime_error("STUB: not implemented");
}
  void write2(int value) {
    throw std::runtime_error("STUB: not implemented");
}
  void write2(int value, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void write_year_extended(long long year, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void write_year(long long year, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void write_utc_offset(long long offset, numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(has_tm_gmtoff<T>::value)>
  void format_utc_offset(const T& tm, numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  template <typename T, FMT_ENABLE_IF(!has_tm_gmtoff<T>::value)>
  void format_utc_offset(const T&, numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename T, FMT_ENABLE_IF(has_tm_zone<T>::value)>
  void format_tz_name(const T& tm) {
    throw std::runtime_error("STUB: not implemented");
}
  template <typename T, FMT_ENABLE_IF(!has_tm_zone<T>::value)>
  void format_tz_name(const T&) {
    throw std::runtime_error("STUB: not implemented");
}

  void format_localized(char format, char modifier = 0) {
    throw std::runtime_error("STUB: not implemented");
}

 public:
  tm_writer(const std::locale& loc, OutputIt out, const std::tm& tm,
            const Duration* subsecs = nullptr)
      : loc_(loc),
        is_classic_(loc_ == get_classic_locale()),
        out_(out),
        subsecs_(subsecs),
        tm_(tm) {
    throw std::runtime_error("STUB: not implemented");
}

  auto out() const -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

  FMT_CONSTEXPR void on_text(const Char* begin, const Char* end) {
    return {};
}

  void on_abbr_weekday() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_full_weekday() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec0_weekday(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec1_weekday(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_abbr_month() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_full_month() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_datetime(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_loc_date(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_loc_time(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_us_date() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_date() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_utc_offset(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_tz_name() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_year(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_short_year(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_offset_year() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_century(numeric_system ns) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_dec_month(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_dec0_week_of_year(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec1_week_of_year(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_week_of_year(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_iso_week_based_year() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_week_based_short_year() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_day_of_year(pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_day_of_month(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_24_hour(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_12_hour(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_minute(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_second(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_12_hour_time() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_24_hour_time() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_time() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_am_pm() {
    throw std::runtime_error("STUB: not implemented");
}

  // These apply to chrono durations but not tm.
  void on_duration_value() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_duration_unit() {
    throw std::runtime_error("STUB: not implemented");
}
};

struct chrono_format_checker : null_chrono_spec_handler<chrono_format_checker> {
  bool has_precision_integral = false;

  FMT_NORETURN inline void unsupported() {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Char>
  FMT_CONSTEXPR void on_text(const Char*, const Char*) {
    return {};
}
  FMT_CONSTEXPR void on_day_of_year(pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_24_hour(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_12_hour(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_minute(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_second(numeric_system, pad_type) {
    return {};
}
  FMT_CONSTEXPR void on_12_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_24_hour_time() {
    return {};
}
  FMT_CONSTEXPR void on_iso_time() {
    return {};
}
  FMT_CONSTEXPR void on_am_pm() {
    return {};
}
  FMT_CONSTEXPR void on_duration_value() const {
    return {};
}
  FMT_CONSTEXPR void on_duration_unit() {
    return {};
}
};

template <typename T,
          FMT_ENABLE_IF(std::is_integral<T>::value&& has_isfinite<T>::value)>
inline auto isfinite(T) -> bool {
    throw std::runtime_error("STUB: not implemented");
}

template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
inline auto mod(T x, int y) -> T {
    throw std::runtime_error("STUB: not implemented");
}
template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
inline auto mod(T x, int y) -> T {
    throw std::runtime_error("STUB: not implemented");
}

// If T is an integral type, maps T to its unsigned counterpart, otherwise
// leaves it unchanged (unlike std::make_unsigned).
template <typename T, bool INTEGRAL = std::is_integral<T>::value>
struct make_unsigned_or_unchanged {
  using type = T;
};

template <typename T> struct make_unsigned_or_unchanged<T, true> {
  using type = typename std::make_unsigned<T>::type;
};

template <typename Rep, typename Period,
          FMT_ENABLE_IF(std::is_integral<Rep>::value)>
inline auto get_milliseconds(std::chrono::duration<Rep, Period> d)
    -> std::chrono::duration<Rep, std::milli> {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename Rep, typename OutputIt,
          FMT_ENABLE_IF(std::is_integral<Rep>::value)>
auto format_duration_value(OutputIt out, Rep val, int) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename Rep, typename OutputIt,
          FMT_ENABLE_IF(std::is_floating_point<Rep>::value)>
auto format_duration_value(OutputIt out, Rep val, int precision) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename OutputIt>
auto copy_unit(string_view unit, OutputIt out, Char) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename OutputIt>
auto copy_unit(string_view unit, OutputIt out, wchar_t) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

template <typename Char, typename Period, typename OutputIt>
auto format_duration_unit(OutputIt out) -> OutputIt {
    throw std::runtime_error("STUB: not implemented");
}

class get_locale {
 private:
  union {
    std::locale locale_;
  };
  bool has_locale_ = false;

 public:
  inline get_locale(bool localized, locale_ref loc) : has_locale_(localized) {
    throw std::runtime_error("STUB: not implemented");
}
  inline ~get_locale() {
    if (has_locale_) locale_.~locale();
  }
  inline operator const std::locale&() const {
    throw std::runtime_error("STUB: not implemented");
}
};

template <typename Char, typename Rep, typename Period>
struct duration_formatter {
  using iterator = basic_appender<Char>;
  iterator out;
  // rep is unsigned to avoid overflow.
  using rep =
      conditional_t<std::is_integral<Rep>::value && sizeof(Rep) < sizeof(int),
                    unsigned, typename make_unsigned_or_unchanged<Rep>::type>;
  rep val;
  int precision;
  locale_ref locale;
  bool localized = false;
  using seconds = std::chrono::duration<rep>;
  seconds s;
  using milliseconds = std::chrono::duration<rep, std::milli>;
  bool negative;

  using tm_writer_type = tm_writer<iterator, Char>;

  duration_formatter(iterator o, std::chrono::duration<Rep, Period> d,
                     locale_ref loc)
      : out(o), val(static_cast<rep>(d.count())), locale(loc), negative(false) {
    throw std::runtime_error("STUB: not implemented");
}

  // returns true if nan or inf, writes to out.
  auto handle_nan_inf() -> bool {
    throw std::runtime_error("STUB: not implemented");
}

  auto days() const -> Rep {
    throw std::runtime_error("STUB: not implemented");
}
  auto hour() const -> Rep {
    throw std::runtime_error("STUB: not implemented");
}

  auto hour12() const -> Rep {
    throw std::runtime_error("STUB: not implemented");
}

  auto minute() const -> Rep {
    throw std::runtime_error("STUB: not implemented");
}
  auto second() const -> Rep {
    throw std::runtime_error("STUB: not implemented");
}

  auto time() const -> std::tm {
    throw std::runtime_error("STUB: not implemented");
}

  void write_sign() {
    throw std::runtime_error("STUB: not implemented");
}

  void write(Rep value, int width, pad_type pad = pad_type::zero) {
    throw std::runtime_error("STUB: not implemented");
}

  void write_nan() {
    throw std::runtime_error("STUB: not implemented");
}

  template <typename Callback, typename... Args>
  void format_tm(const tm& time, Callback cb, Args... args) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_text(const Char* begin, const Char* end) {
    throw std::runtime_error("STUB: not implemented");
}

  // These are not implemented because durations don't have date information.
  void on_abbr_weekday() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_full_weekday() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec0_weekday(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec1_weekday(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_abbr_month() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_full_month() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_datetime(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_loc_date(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_loc_time(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_us_date() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_date() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_utc_offset(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_tz_name() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_year(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_short_year(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_offset_year() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_century(numeric_system) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_week_based_year() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_week_based_short_year() {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec_month(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec0_week_of_year(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_dec1_week_of_year(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_iso_week_of_year(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}
  void on_day_of_month(numeric_system, pad_type) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_day_of_year(pad_type) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_24_hour(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_12_hour(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_minute(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_second(numeric_system ns, pad_type pad) {
    throw std::runtime_error("STUB: not implemented");
}

  void on_12_hour_time() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_24_hour_time() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_iso_time() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_am_pm() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_duration_value() {
    throw std::runtime_error("STUB: not implemented");
}

  void on_duration_unit() {
    throw std::runtime_error("STUB: not implemented");
}
};

}  // namespace detail

#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907
using weekday = std::chrono::weekday;
using day = std::chrono::day;
using month = std::chrono::month;
using year = std::chrono::year;
using year_month_day = std::chrono::year_month_day;
#else
// A fallback version of weekday.
class weekday {
 private:
  unsigned char value_;

 public:
  weekday() = default;
  constexpr explicit weekday(unsigned wd) noexcept
      : value_(static_cast<unsigned char>(wd != 7 ? wd : 0)) {
    return {};
}
  constexpr auto c_encoding() const noexcept -> unsigned {
    return {};
}
};

class day {
 private:
  unsigned char value_;

 public:
  day() = default;
  constexpr explicit day(unsigned d) noexcept
      : value_(static_cast<unsigned char>(d)) {
    return {};
}
  constexpr explicit operator unsigned() const noexcept {
    return {};
}
};

class month {
 private:
  unsigned char value_;

 public:
  month() = default;
  constexpr explicit month(unsigned m) noexcept
      : value_(static_cast<unsigned char>(m)) {
    return {};
}
  constexpr explicit operator unsigned() const noexcept {
    return {};
}
};

class year {
 private:
  int value_;

 public:
  year() = default;
  constexpr explicit year(int y) noexcept : value_(y) {
    return {};
}
  constexpr explicit operator int() const noexcept {
    return {};
}
};

class year_month_day {
 private:
  fmt::year year_;
  fmt::month month_;
  fmt::day day_;

 public:
  year_month_day() = default;
  constexpr year_month_day(const year& y, const month& m, const day& d) noexcept
      : year_(y), month_(m), day_(d) {
    return {};
}
  constexpr auto year() const noexcept -> fmt::year {
    return {};
}
  constexpr auto month() const noexcept -> fmt::month {
    return {};
}
  constexpr auto day() const noexcept -> fmt::day {
    return {};
}
};
#endif  // __cpp_lib_chrono >= 201907

template <typename Char>
struct formatter<weekday, Char> : private formatter<std::tm, Char> {
 private:
  bool use_tm_formatter_ = false;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && *it == 'L') {
      ++it;
      this->set_localized();
    }
    use_tm_formatter_ = it != end && *it != '}';
    return use_tm_formatter_ ? formatter<std::tm, Char>::parse(ctx) : it;
  }

  template <typename FormatContext>
  auto format(weekday wd, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto time = std::tm();
    time.tm_wday = static_cast<int>(wd.c_encoding());
    if (use_tm_formatter_) return formatter<std::tm, Char>::format(time, ctx);
    detail::get_locale loc(this->localized(), ctx.locale());
    auto w = detail::tm_writer<decltype(ctx.out()), Char>(loc, ctx.out(), time);
    w.on_abbr_weekday();
    return w.out();
  }
};

template <typename Char>
struct formatter<day, Char> : private formatter<std::tm, Char> {
 private:
  bool use_tm_formatter_ = false;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    use_tm_formatter_ = it != end && *it != '}';
    return use_tm_formatter_ ? formatter<std::tm, Char>::parse(ctx) : it;
  }

  template <typename FormatContext>
  auto format(day d, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto time = std::tm();
    time.tm_mday = static_cast<int>(static_cast<unsigned>(d));
    if (use_tm_formatter_) return formatter<std::tm, Char>::format(time, ctx);
    detail::get_locale loc(false, ctx.locale());
    auto w = detail::tm_writer<decltype(ctx.out()), Char>(loc, ctx.out(), time);
    w.on_day_of_month(detail::numeric_system::standard, detail::pad_type::zero);
    return w.out();
  }
};

template <typename Char>
struct formatter<month, Char> : private formatter<std::tm, Char> {
 private:
  bool use_tm_formatter_ = false;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && *it == 'L') {
      ++it;
      this->set_localized();
    }
    use_tm_formatter_ = it != end && *it != '}';
    return use_tm_formatter_ ? formatter<std::tm, Char>::parse(ctx) : it;
  }

  template <typename FormatContext>
  auto format(month m, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto time = std::tm();
    time.tm_mon = static_cast<int>(static_cast<unsigned>(m)) - 1;
    if (use_tm_formatter_) return formatter<std::tm, Char>::format(time, ctx);
    detail::get_locale loc(this->localized(), ctx.locale());
    auto w = detail::tm_writer<decltype(ctx.out()), Char>(loc, ctx.out(), time);
    w.on_abbr_month();
    return w.out();
  }
};

template <typename Char>
struct formatter<year, Char> : private formatter<std::tm, Char> {
 private:
  bool use_tm_formatter_ = false;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    use_tm_formatter_ = it != end && *it != '}';
    return use_tm_formatter_ ? formatter<std::tm, Char>::parse(ctx) : it;
  }

  template <typename FormatContext>
  auto format(year y, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto time = std::tm();
    time.tm_year = static_cast<int>(y) - 1900;
    if (use_tm_formatter_) return formatter<std::tm, Char>::format(time, ctx);
    detail::get_locale loc(false, ctx.locale());
    auto w = detail::tm_writer<decltype(ctx.out()), Char>(loc, ctx.out(), time);
    w.on_year(detail::numeric_system::standard, detail::pad_type::zero);
    return w.out();
  }
};

template <typename Char>
struct formatter<year_month_day, Char> : private formatter<std::tm, Char> {
 private:
  bool use_tm_formatter_ = false;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    use_tm_formatter_ = it != end && *it != '}';
    return use_tm_formatter_ ? formatter<std::tm, Char>::parse(ctx) : it;
  }

  template <typename FormatContext>
  auto format(year_month_day val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto time = std::tm();
    time.tm_year = static_cast<int>(val.year()) - 1900;
    time.tm_mon = static_cast<int>(static_cast<unsigned>(val.month())) - 1;
    time.tm_mday = static_cast<int>(static_cast<unsigned>(val.day()));
    if (use_tm_formatter_) return formatter<std::tm, Char>::format(time, ctx);
    detail::get_locale loc(true, ctx.locale());
    auto w = detail::tm_writer<decltype(ctx.out()), Char>(loc, ctx.out(), time);
    w.on_iso_date();
    return w.out();
  }
};

template <typename Rep, typename Period, typename Char>
struct formatter<std::chrono::duration<Rep, Period>, Char> {
 private:
  format_specs specs_;
  detail::arg_ref<Char> width_ref_;
  detail::arg_ref<Char> precision_ref_;
  basic_string_view<Char> fmt_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    if (it == end || *it == '}') return it;

    it = detail::parse_align(it, end, specs_);
    if (it == end) return it;

    Char c = *it;
    if ((c >= '0' && c <= '9') || c == '{') {
      it = detail::parse_width(it, end, specs_, width_ref_, ctx);
      if (it == end) return it;
    }

    auto checker = detail::chrono_format_checker();
    if (*it == '.') {
      checker.has_precision_integral = !std::is_floating_point<Rep>::value;
      it = detail::parse_precision(it, end, specs_, precision_ref_, ctx);
    }
    if (it != end && *it == 'L') {
      specs_.set_localized();
      ++it;
    }
    end = detail::parse_chrono_format(it, end, checker);
    fmt_ = {it, detail::to_unsigned(end - it)};
    return end;
  }

  template <typename FormatContext>
  auto format(std::chrono::duration<Rep, Period> d, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto specs = specs_;
    auto precision = specs.precision;
    specs.precision = -1;
    auto begin = fmt_.begin(), end = fmt_.end();
    // As a possible future optimization, we could avoid extra copying if width
    // is not specified.
    auto buf = basic_memory_buffer<Char>();
    auto out = basic_appender<Char>(buf);
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width, width_ref_,
                                ctx);
    detail::handle_dynamic_spec(specs.dynamic_precision(), precision,
                                precision_ref_, ctx);
    if (begin == end || *begin == '}') {
      out = detail::format_duration_value<Char>(out, d.count(), precision);
      detail::format_duration_unit<Char, Period>(out);
    } else {
      auto f =
          detail::duration_formatter<Char, Rep, Period>(out, d, ctx.locale());
      f.precision = precision;
      f.localized = specs_.localized();
      detail::parse_chrono_format(begin, end, f);
    }
    return detail::write(
        ctx.out(), basic_string_view<Char>(buf.data(), buf.size()), specs);
  }
};

template <typename Char> struct formatter<std::tm, Char> {
 private:
  format_specs specs_;
  detail::arg_ref<Char> width_ref_;
  basic_string_view<Char> fmt_ =
      detail::string_literal<Char, '%', 'F', ' ', '%', 'T'>();

 protected:
  auto localized() const -> bool { return specs_.localized(); }
  FMT_CONSTEXPR void set_localized() { specs_.set_localized(); }

  FMT_CONSTEXPR auto do_parse(parse_context<Char>& ctx, bool has_timezone)
      -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    if (it == end || *it == '}') return it;

    it = detail::parse_align(it, end, specs_);
    if (it == end) return it;

    Char c = *it;
    if ((c >= '0' && c <= '9') || c == '{') {
      it = detail::parse_width(it, end, specs_, width_ref_, ctx);
      if (it == end) return it;
    }

    if (*it == 'L') {
      specs_.set_localized();
      ++it;
    }

    end = detail::parse_chrono_format(it, end,
                                      detail::tm_format_checker(has_timezone));
    // Replace the default format string only if the new spec is not empty.
    if (end != it) fmt_ = {it, detail::to_unsigned(end - it)};
    return end;
  }

  template <typename Duration, typename FormatContext>
  auto do_format(const std::tm& tm, FormatContext& ctx,
                 const Duration* subsecs) const -> decltype(ctx.out()) {
    auto specs = specs_;
    auto buf = basic_memory_buffer<Char>();
    auto out = basic_appender<Char>(buf);
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width, width_ref_,
                                ctx);

    auto loc_ref = specs.localized() ? ctx.locale() : locale_ref();
    detail::get_locale loc(static_cast<bool>(loc_ref), loc_ref);
    auto w = detail::tm_writer<basic_appender<Char>, Char, Duration>(
        loc, out, tm, subsecs);
    detail::parse_chrono_format(fmt_.begin(), fmt_.end(), w);
    return detail::write(
        ctx.out(), basic_string_view<Char>(buf.data(), buf.size()), specs);
  }

 public:
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return do_parse(ctx, detail::has_tm_gmtoff<std::tm>::value);
  }

  template <typename FormatContext>
  auto format(const std::tm& tm, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return do_format<std::chrono::seconds>(tm, ctx, nullptr);
  }
};

// DEPRECATED! Reversed order of template parameters.
template <typename Char, typename Duration>
struct formatter<sys_time<Duration>, Char> : private formatter<std::tm, Char> {
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return this->do_parse(ctx, true);
  }

  template <typename FormatContext>
  auto format(sys_time<Duration> val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    std::tm tm = gmtime(val);
    using period = typename Duration::period;
    if FMT_CONSTEXPR20 (period::num == 1 && period::den == 1 &&
                        !std::is_floating_point<
                            typename Duration::rep>::value) {
      detail::set_tm_zone(tm, detail::utc());
      return formatter<std::tm, Char>::format(tm, ctx);
    }
    Duration epoch = val.time_since_epoch();
    Duration subsecs = detail::duration_cast<Duration>(
        epoch - detail::duration_cast<std::chrono::seconds>(epoch));
    if (subsecs.count() < 0) {
      auto second = detail::duration_cast<Duration>(std::chrono::seconds(1));
      if (tm.tm_sec != 0) {
        --tm.tm_sec;
      } else {
        tm = gmtime(val - second);
        detail::set_tm_zone(tm, detail::utc());
      }
      subsecs += second;
    }
    return formatter<std::tm, Char>::do_format(tm, ctx, &subsecs);
  }
};

template <typename Duration, typename Char>
struct formatter<utc_time<Duration>, Char>
    : formatter<sys_time<Duration>, Char> {
  template <typename FormatContext>
  auto format(utc_time<Duration> val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return formatter<sys_time<Duration>, Char>::format(
        detail::utc_clock::to_sys(val), ctx);
  }
};

template <typename Duration, typename Char>
struct formatter<local_time<Duration>, Char>
    : private formatter<std::tm, Char> {
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return this->do_parse(ctx, false);
  }

  template <typename FormatContext>
  auto format(local_time<Duration> val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto time_since_epoch = val.time_since_epoch();
    auto seconds_since_epoch =
        detail::duration_cast<std::chrono::seconds>(time_since_epoch);
    // Use gmtime to prevent time zone conversion since local_time has an
    // unspecified time zone.
    std::tm t = gmtime(seconds_since_epoch.count());
    using period = typename Duration::period;
    if (period::num == 1 && period::den == 1 &&
        !std::is_floating_point<typename Duration::rep>::value) {
      return formatter<std::tm, Char>::format(t, ctx);
    }
    auto subsecs =
        detail::duration_cast<Duration>(time_since_epoch - seconds_since_epoch);
    return formatter<std::tm, Char>::do_format(t, ctx, &subsecs);
  }
};

FMT_END_EXPORT
FMT_END_NAMESPACE

#endif  // FMT_CHRONO_H_
