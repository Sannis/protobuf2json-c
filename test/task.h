/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef TASK_H_
#define TASK_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
# include "stdint-msvc2008.h"
#else
# include <stdint.h>
#endif

#if !defined(_WIN32)
# include <sys/time.h>
# include <sys/resource.h>  /* setrlimit() */
#endif

#ifdef _WIN32
# include <io.h>
# ifndef S_IRUSR
#  define S_IRUSR _S_IREAD
# endif
# ifndef S_IWUSR
#  define S_IWUSR _S_IWRITE
# endif
#endif

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# elif defined(_MAX_PATH)
#  define MAXPATHLEN _MAX_PATH
# elif defined(CCHMAXPATH)
#  define MAXPATHLEN CCHMAXPATH
# else
#  define MAXPATHLEN 1024
# endif
#endif

#define TEST_JSON_FLAGS (JSON_INDENT(2) | JSON_PRESERVE_ORDER)

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

/* Log to stderr. */
#define LOG(...)                        \
  do {                                  \
    fprintf(stderr, "%s", __VA_ARGS__); \
    fflush(stderr);                     \
  } while (0)

#define LOGF(...)                       \
  do {                                  \
    fprintf(stderr, __VA_ARGS__);       \
    fflush(stderr);                     \
  } while (0)

/* Die with fatal error. */
#define FATAL(msg)                                        \
  do {                                                    \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  } while (0)

/* Have our own assert, so we are sure it does not get optimized away in
 * a release build.
 */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)

#define ASSERT_ZERO(expr)                                 \
 do {                                                     \
  if (expr != 0) {                                        \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: "         \
            "%s is not zero, but %d\n",                   \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr, expr);                                 \
    abort();                                              \
  }                                                       \
 } while (0)

#define ASSERT_EQUALS(actual, expected)                   \
 do {                                                     \
  if (actual != expected) {                               \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: "         \
            "%s is not %d, but %d\n",                     \
            __FILE__,                                     \
            __LINE__,                                     \
            #actual, expected, actual);                   \
    abort();                                              \
  }                                                       \
 } while (0)

/* Assert that string are equal */
#define ASSERT_STRCMP(actual, expected)                                                         \
 do {                                                                                           \
  if (strcmp(actual, expected)) {                                                               \
    fprintf(stderr,                                                                             \
            "Assertion failed in %s on line %d:\n%s\n --- not equal to expected --- \n%s\n",    \
            __FILE__,                                                                           \
            __LINE__,                                                                           \
            actual,                                                                             \
            expected);                                                                          \
    abort();                                                                                    \
  }                                                                                             \
 } while (0)

/* Just sugar for wrapping the main() for a task or helper. */
#define TEST_IMPL(name)                                                       \
  int run_test_##name(void);                                                  \
  int run_test_##name(void)

#define BENCHMARK_IMPL(name)                                                  \
  int run_benchmark_##name(void);                                             \
  int run_benchmark_##name(void)

#define HELPER_IMPL(name)                                                     \
  int run_helper_##name(void);                                                \
  int run_helper_##name(void)

/* Pause the calling thread for a number of milliseconds. */
void runner_sleep(int msec);

/* Reserved test exit codes. */
enum test_status {
  TEST_OK = 0,
  TEST_TODO,
  TEST_SKIP
};

#define RETURN_OK()                                                           \
  do {                                                                        \
    return TEST_OK;                                                           \
  } while (0)

#define RETURN_TODO(explanation)                                              \
  do {                                                                        \
    LOGF("%s\n", explanation);                                                \
    return TEST_TODO;                                                         \
  } while (0)

#define RETURN_SKIP(explanation)                                              \
  do {                                                                        \
    LOGF("%s\n", explanation);                                                \
    return TEST_SKIP;                                                         \
  } while (0)

#if defined _WIN32 && ! defined __GNUC__

#include <stdarg.h>

/* Emulate snprintf() on Windows, _snprintf() doesn't zero-terminate the buffer
 * on overflow...
 */
static int snprintf(char* buf, size_t len, const char* fmt, ...) {
  va_list ap;
  int n;

  va_start(ap, fmt);
  n = _vsprintf_p(buf, len, fmt, ap);
  va_end(ap);

  /* It's a sad fact of life that no one ever checks the return value of
   * snprintf(). Zero-terminating the buffer hopefully reduces the risk
   * of gaping security holes.
   */
  if (n < 0)
    if (len > 0)
      buf[0] = '\0';

  return n;
}

#endif

#endif /* TASK_H_ */
