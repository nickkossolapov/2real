#pragma once

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <array>

namespace engine {

class FpsLogger {

public:
  void tick() {
    const uint64_t now = SDL_GetTicks();
    const float frame_ms = static_cast<float>(now - last_frame_time_);
    last_frame_time_ = now;

    sum_ -= samples_[index_];
    sum_ += frame_ms;
    samples_[index_] = frame_ms;
    index_ = (index_ + 1) % sample_count;
    if (filled_ < sample_count)
      filled_++;

    if (now - last_log_time_ >= log_interval_ms) {
      const float avg_ms = sum_ / static_cast<float>(filled_);
      SDL_Log("FPS: %.1f (%.2f ms)", 1000.0f / avg_ms, avg_ms);
      last_log_time_ = now;
    }
  }

private:
  static constexpr int sample_count = 60;
  static constexpr uint64_t log_interval_ms = 3000;

  std::array<float, sample_count> samples_ = {};
  float sum_ = 0.0f;
  int index_ = 0;
  int filled_ = 0;
  uint64_t last_frame_time_ = SDL_GetTicks();
  uint64_t last_log_time_ = SDL_GetTicks();
};

} // namespace engine
