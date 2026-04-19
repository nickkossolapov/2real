#pragma once

#include <SDL3/SDL_timer.h>

namespace engine {

class FrameLimiter {

public:
  explicit FrameLimiter(const float target_fps, bool v_sync)
    : target_fps_(target_fps), v_sync_(v_sync) {
  }

  float tick() {
    const uint64_t now = SDL_GetTicks();

    if (!v_sync_) {
      cap(now);
    }

    const uint64_t after = SDL_GetTicks();
    const float dt = static_cast<float>(after - last_time_);
    last_time_ = after;

    return dt;
  }

private:
  void cap(const uint64_t frame_start) const {
    const float target_ms = 1000.0f / target_fps_;

    if (const float elapsed = static_cast<float>(SDL_GetTicks() - frame_start); elapsed < target_ms) {
      SDL_Delay(static_cast<uint32_t>(target_ms - elapsed));
    }
  }

  float target_fps_;
  bool v_sync_;
  Uint64 last_time_ = SDL_GetTicks();
};

} // namespace engine
