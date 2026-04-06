#pragma once

#include <SDL3/SDL_timer.h>

class FrameLimiter {

public:
  explicit FrameLimiter(const float target_fps, bool v_sync)
    : target_fps_(target_fps), v_sync_(v_sync) {
  }

  float tick() {
    const uint64_t now = SDL_GetTicks();
    const float dt = static_cast<float>(now - last_time_);

    last_time_ = now;

    if (!v_sync_) {
      cap();
    }

    return dt;
  }

private:
  void cap() const {
    const float target_ms = 1000.0f / target_fps_;

    if (const float elapsed = static_cast<float>(SDL_GetTicks() - last_time_); elapsed < target_ms) {
      SDL_Delay(static_cast<uint32_t>(target_ms - elapsed));
    }
  }

  float target_fps_;
  bool v_sync_;
  Uint64 last_time_ = SDL_GetTicks();
};
