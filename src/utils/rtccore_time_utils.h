#ifndef RTCCORE_TIME_UTILS_H_
#define RTCCORE_TIME_UTILS_H_

#include <stdint.h>
#include <stdio.h>
class TimeUtils{
public:
    static uint64_t gettickcount();  // ms
};
#endif /* TIME_UTILS_H_ */
