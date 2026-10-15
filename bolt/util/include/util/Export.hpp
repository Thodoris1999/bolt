#pragma once

// For data symbols of bolt libraries built as DLLs on Windows. Functions are
// exported by WINDOWS_EXPORT_ALL_SYMBOLS.
#ifdef _WIN32
#  define BOLT_EXPORT __declspec(dllexport)
#  define BOLT_IMPORT __declspec(dllimport)
#else
#  define BOLT_EXPORT
#  define BOLT_IMPORT
#endif
