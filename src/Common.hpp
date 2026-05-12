/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2026, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TESTGPUUNIFIEDMEMORY_COMMON_HPP
#define TESTGPUUNIFIEDMEMORY_COMMON_HPP

#include <string>
#include <sstream>
#include <cstdint>

// Checks whether the entries in the passed pointer are linearly increasing.
bool checkIsArrayLinear(size_t numEntries, const void* ptr, std::string& errorMessage);
inline std::string convertTimeToString(double elapsedTimeNs, uint32_t numElements) {
    double unitNumber;
    std::string unitString;
    if (numElements > 1024 * 1024) {
        unitNumber = elapsedTimeNs * 1e-6;
        unitString = "ms";
    } else {
        unitNumber = elapsedTimeNs * 1e-3;
        unitString = "us";
    }

    std::ostringstream ostr;
    ostr.precision(3);
    ostr << std::fixed << unitNumber;
    return ostr.str() + unitString;
}

inline int getNumRuns(int numCopiesPerRun, uint32_t numElements) {
    if (numElements > 8 * 1024 * 1024) {
        return numCopiesPerRun <= 1 ? 10 : 1;
    }
    if (numElements > 8 * 1024) {
        return 10;
    }
    return 100;
}

#define NUM_CONFIGS 4
constexpr int configsNumCopiesPerRun[4] = { 1, 10, 100, 100 };
constexpr bool configsMeasureUpload[4] = { true, true, true, false };

#endif //TESTGPUUNIFIEDMEMORY_COMMON_HPP
