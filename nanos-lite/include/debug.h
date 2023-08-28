#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <common.h>

/* clang-format off */
#ifndef Log
#define Log(format, ...) \
  printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#endif

#ifndef panic
#define panic(format, ...) \
  do { \
    Log("\33[1;31msystem panic: " format, ## __VA_ARGS__); \
    halt(1); \
  } while (0)
#endif

#ifndef assert
#define assert(cond) \
  do { \
    if (!(cond)) { \
      panic("Assertion failed: %s", #cond); \
    } \
  } while (0)
#endif

#ifndef Assert
#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      Log(format, ## __VA_ARGS__); \
      assert(cond); \
    } \
  } while (0)
#endif
/* clang-format on */

#define TODO() panic("please implement me")

#endif