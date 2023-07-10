#ifndef TIME_UTILS_H_
#define TIME_UTILS_H_

#include <stdint.h>
#include <stdio.h>
namespace TimeUtils {
    uint64_t TimeMillis();  // ms
    int64_t TimeDiff(int64_t later, int64_t earlier);
    int64_t TimeAfter(int64_t elapsed);

    inline int64_t TimeUntil(int64_t later) {
      return later - TimeMillis();
    }
};
#endif /* TIME_UTILS_H_ */
