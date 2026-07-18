#pragma once
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <cstdarg>
#include <cstdint>

namespace engine {

struct SparseLogger {
  void log(SDL_PRINTF_FORMAT_STRING const char* fmt, ...) SDL_PRINTF_VARARG_FUNC(2) {
    const uint64_t now = SDL_GetTicks();

    if (now < next_log_time_) {
      return;
    }

    next_log_time_ = now + log_interval_ms_;

    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
    va_end(args);
  }

private:
  static constexpr uint64_t log_interval_ms_ = 3000;

  uint64_t next_log_time_ = SDL_GetTicks();
};

} // namespace engine
