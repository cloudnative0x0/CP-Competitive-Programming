#ifndef CP_COMPETITIVE_PROGRAMMING_PROFILER_HPP
#define CP_COMPETITIVE_PROGRAMMING_PROFILER_HPP

#pragma once

namespace pkg {
    inline constexpr bool kDebugEnabled = true;

    void StartProfiling();
    void StopProfiling();

    void Debug(const char* format, ...)
    #if defined(__GNUC__) || defined(__clang__)
        __attribute__((format(printf, 1, 2)))
    #endif
        ;

    void SetTrace(bool on);
    bool Trace();

}

#endif //CP_COMPETITIVE_PROGRAMMING_PROFILER_HPP