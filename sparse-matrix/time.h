#pragma once
#include "types.h"

template <typename F>
u64 profile( F&& func ) {
    u64 start = clock();
    func();
    return clock() - start;
}