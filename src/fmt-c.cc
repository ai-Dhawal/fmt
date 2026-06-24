#include <stdexcept>
#include <cstdlib>
// Formatting library for C++ - the C API
//
// Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors
// All rights reserved.
//
// For the license information refer to format.h.

#include "fmt/fmt-c.h"

#include <fmt/base.h>

constexpr size_t max_c_format_args = 16;

static int convert_c_format_args(
    fmt::basic_format_arg<fmt::format_context>* format_args,
    const fmt_arg* args, size_t num_args) {
    throw std::runtime_error("STUB: not implemented");
}

extern "C" int fmt_vformat(char* buffer, size_t size, const char* fmt,
                           const fmt_arg* args, size_t num_args) {
    throw std::runtime_error("STUB: not implemented");
}

extern "C" int fmt_vprint(FILE* stream, const char* fmt, const fmt_arg* args,
                          size_t num_args) {
    throw std::runtime_error("STUB: not implemented");
}
