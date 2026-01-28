#pragma once

// DLL Export/Import macros for cross-platform shared library support

#if defined(KOO_SIM_STATIC)
    // Static library - no import/export needed
    #define KOO_API
#elif defined(_WIN32) || defined(_WIN64)
    #ifdef KOO_SIM_EXPORTS
        #define KOO_API __declspec(dllexport)
    #else
        #define KOO_API __declspec(dllimport)
    #endif
#else
    // GCC/Clang visibility
    #define KOO_API __attribute__((visibility("default")))
#endif

// Suppress unused parameter warnings
#define KOO_UNUSED(x) (void)(x)
