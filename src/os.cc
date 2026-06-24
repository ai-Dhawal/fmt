#include <stdexcept>
#include <cstdlib>
// Formatting library for C++ - optional OS-specific functionality
//
// Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors
// All rights reserved.
//
// For the license information refer to format.h.

// Disable bogus MSVC warnings.
#if !defined(_CRT_SECURE_NO_WARNINGS) && defined(_MSC_VER)
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include "fmt/os.h"

#ifndef FMT_MODULE

#  if FMT_USE_FCNTL
#    include <sys/stat.h>
#    include <sys/types.h>

#    ifdef _WRS_KERNEL    // VxWorks7 kernel
#      include <ioLib.h>  // getpagesize
#    endif

#    ifndef _WIN32
#      include <unistd.h>
#    else
#      ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#      endif
#      include <io.h>
#    endif  // _WIN32
#  endif    // FMT_USE_FCNTL

#  ifdef _WIN32
#    include <windows.h>

#    include <climits>  // CHAR_BIT
#  endif
#endif

#ifdef _WIN32
#  ifndef S_IRUSR
#    define S_IRUSR _S_IREAD
#  endif
#  ifndef S_IWUSR
#    define S_IWUSR _S_IWRITE
#  endif
#  ifndef S_IRGRP
#    define S_IRGRP 0
#  endif
#  ifndef S_IWGRP
#    define S_IWGRP 0
#  endif
#  ifndef S_IROTH
#    define S_IROTH 0
#  endif
#  ifndef S_IWOTH
#    define S_IWOTH 0
#  endif
#endif

namespace {
#ifdef _WIN32

// Return type of read and write functions.
using rwresult = int;

// On Windows the count argument to read and write is unsigned, so convert
// it from size_t preventing integer overflow.
inline unsigned convert_rwcount(size_t count) {
  return count <= UINT_MAX ? static_cast<unsigned>(count) : UINT_MAX;
}

class system_message {
  system_message(const system_message&) = delete;
  void operator=(const system_message&) = delete;

  unsigned long result_;
  wchar_t* message_;

  static bool is_whitespace(wchar_t c) noexcept {
    return c == L' ' || c == L'\n' || c == L'\r' || c == L'\t' || c == L'\0';
  }

 public:
  explicit system_message(unsigned long error_code)
      : result_(0), message_(nullptr) {
    result_ = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<wchar_t*>(&message_), 0, nullptr);
    if (result_ != 0) {
      while (result_ != 0 && is_whitespace(message_[result_ - 1])) {
        --result_;
      }
    }
  }
  ~system_message() { LocalFree(message_); }
  explicit operator bool() const noexcept { return result_ != 0; }
  operator fmt::basic_string_view<wchar_t>() const noexcept {
    return fmt::basic_string_view<wchar_t>(message_, result_);
  }
};

class utf8_system_category final : public std::error_category {
 public:
  const char* name() const noexcept override { return "system"; }
  std::string message(int error_code) const override {
    auto&& msg = system_message(error_code);
    if (msg) {
      auto utf8_message = fmt::detail::to_utf8<wchar_t>();
      if (utf8_message.convert(msg)) {
        return utf8_message.str();
      }
    }
    return "unknown error";
  }
};

#elif FMT_USE_FCNTL

// Return type of read and write functions.
using rwresult = ssize_t;

inline auto convert_rwcount(size_t count) -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

#endif
}  // namespace

FMT_BEGIN_NAMESPACE

#ifdef _WIN32

FMT_API const std::error_category& system_category() noexcept {
  static const utf8_system_category category;
  return category;
}

std::system_error vwindows_error(int err_code, string_view format_str,
                                 format_args args) {
  auto ec = std::error_code(err_code, system_category());
  return std::system_error(ec, vformat(format_str, args));
}

void detail::format_windows_error(detail::buffer<char>& out, int error_code,
                                  const char* message) noexcept {
  FMT_TRY {
    auto&& msg = system_message(error_code);
    if (msg) {
      auto utf8_message = to_utf8<wchar_t>();
      if (utf8_message.convert(msg)) {
        fmt::format_to(appender(out), FMT_STRING("{}: {}"), message,
                       string_view(utf8_message));
        return;
      }
    }
  }
  FMT_CATCH(...) {}
  format_error_code(out, error_code, message);
}

void report_windows_error(int error_code, const char* message) noexcept {
  do_report_error(detail::format_windows_error, error_code, message);
}

#endif  // _WIN32

buffered_file::~buffered_file() noexcept {
  if (file_ && FMT_SYSTEM(fclose(file_)) != 0)
    report_system_error(errno, "cannot close file");
}

buffered_file::buffered_file(cstring_view filename, cstring_view mode) {
    throw std::runtime_error("STUB: not implemented");
}

void buffered_file::close() {
    throw std::runtime_error("STUB: not implemented");
}

auto buffered_file::descriptor() const -> int {
    throw std::runtime_error("STUB: not implemented");
}

#if FMT_USE_FCNTL
#  ifdef _WIN32
using mode_t = int;
#  endif

constexpr mode_t default_open_mode =
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

file::file(cstring_view path, int oflag) {
    throw std::runtime_error("STUB: not implemented");
}

file::~file() noexcept {
  // Don't retry close in case of EINTR!
  // See http://linux.derkeiler.com/Mailing-Lists/Kernel/2005-09/3000.html
  if (fd_ != -1 && FMT_POSIX_CALL(close(fd_)) != 0)
    report_system_error(errno, "cannot close file");
}

void file::close() {
    throw std::runtime_error("STUB: not implemented");
}

auto file::size() const -> long long {
    throw std::runtime_error("STUB: not implemented");
}

auto file::read(void* buffer, size_t count) -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

auto file::write(const void* buffer, size_t count) -> size_t {
    throw std::runtime_error("STUB: not implemented");
}

auto file::dup(int fd) -> file {
    throw std::runtime_error("STUB: not implemented");
}

void file::dup2(int fd) {
    throw std::runtime_error("STUB: not implemented");
}

void file::dup2(int fd, std::error_code& ec) noexcept {
    abort();
}

auto file::fdopen(const char* mode) -> buffered_file {
    throw std::runtime_error("STUB: not implemented");
}

#  if defined(_WIN32) && !defined(__MINGW32__)
file file::open_windows_file(wcstring_view path, int oflag) {
  int fd = -1;
  auto err = _wsopen_s(&fd, path.c_str(), oflag, _SH_DENYNO, default_open_mode);
  if (fd == -1) {
    FMT_THROW(system_error(err, FMT_STRING("cannot open file {}"),
                           detail::to_utf8<wchar_t>(path.c_str()).c_str()));
  }
  return file(fd);
}
#  endif

pipe::pipe() {
    throw std::runtime_error("STUB: not implemented");
}

#  if !defined(__MSDOS__)
auto getpagesize() -> long {
    throw std::runtime_error("STUB: not implemented");
}
#  endif

void ostream::grow(buffer<char>& buf, size_t) {
    throw std::runtime_error("STUB: not implemented");
}

ostream::ostream(cstring_view path, const detail::ostream_params& params)
    : buffer<char>(grow), file_(path, params.oflag) {
    throw std::runtime_error("STUB: not implemented");
}

ostream::ostream(ostream&& other) noexcept
    : buffer<char>(grow, other.data(), other.size(), other.capacity()),
      file_(std::move(other.file_)) {
  other.clear();
  other.set(nullptr, 0);
}

ostream::~ostream() {
  flush();
  delete[] data();
}
#endif  // FMT_USE_FCNTL
FMT_END_NAMESPACE
