#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

#if defined(__APPLE__)
#include <unistd.h>
#elif defined(__ANDROID__)
#include <android/log.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#define LOG()                                                             \
  zomint::LogMessage(                                                     \
      &__FILE__[std::integral_constant<size_t, zomint::GetFileNameOffset( \
                                                   __FILE__)>()],         \
      __FUNCTION__, __LINE__)

namespace zomint {

namespace {
template <typename T, size_t size>
inline constexpr size_t GetFileNameOffset(const T (&file_path)[size],
                                          size_t i = size - 1) {
  static_assert(size > 1, "");
  if (file_path[i] == '/' || file_path[i] == '\\') {
    return i + 1;
  } else if (i == 0) {
    return 0;
  } else {
    return GetFileNameOffset(file_path, i - 1);
  }
}
}  // namespace

class LogMessage : public std::ostringstream {
 public:
  LogMessage(const char* const file, const char* const function,
             const uint32_t line)
      : file_(file) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    *this << std::put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << "."
          << std::setfill('0') << std::setw(3)
          << std::chrono::duration_cast<std::chrono::milliseconds>(
                 now.time_since_epoch())
                     .count() %
                 1000
          << " [" << CurrentProcessId() << "-" << CurrentThreadId() << "] ("
          << file_ << ":" << line << ") " << function << ": ";
  }

  ~LogMessage() {
    *this << "\n";
    std::cout << str();
#if defined(__ANDROID__)
//     __android_log_print(ANDROID_LOG_INFO, file_, "%s", str().c_str());
#endif
  }

 private:
  const char* const file_ = nullptr;

  static uintmax_t CurrentProcessId() {
#if defined(__APPLE__) || defined(__ANDROID__)
    return getpid();
#elif defined(_WIN32)
    return GetCurrentProcessId();
#else
    return 0;
#endif
  }

  static uintmax_t CurrentThreadId() {
#if defined(__APPLE__)
    return pthread_mach_thread_np(pthread_self());
#elif defined(__ANDROID__)
    return gettid();
#elif defined(_WIN32)
    return GetCurrentThreadId();
#else
    return 0;
#endif
  }
};

}  // namespace zomint