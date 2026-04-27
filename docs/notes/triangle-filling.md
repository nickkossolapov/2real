# Midpoint
In my original flat-top/flat-bottom code:

```cpp
void filled_flat_bottom(Context& context, const std::array<math::Vec2, 3>& vertices, const uint32_t color) {  
  auto [top, mid1, mid2] = vertices;  
  
  const float m1 = (mid1.x - top.x) / (mid1.y - top.y);  
  const float m2 = (mid2.x - top.x) / (mid2.y - top.y);  
  
  const int y_start = static_cast<int>(std::ceil(top.y));  
  const int y_end = static_cast<int>(std::floor(mid1.y));  
  
  for (int y = y_start; y <= y_end; y++) {  
    const float dy = static_cast<float>(y) - top.y;  
    const float x_start = top.x + m1 * dy;  
    const float x_end = top.x + m2 * dy;  
  
    flat_line(context, y, x_start, x_end, color);  
  }  
}

void filled_triangle(Context& context, std::array<math::Vec2, 3> v, const uint32_t color) {  
  // sorting and other code
  // ...
  
    const math::Vec2 v_mid = {.x = v[0].x + (v[2].x - v[0].x) * (v[1].y - v[0].y) / (v[2].y - v[0].y), .y = v[1].y};  
  
    filled_flat_bottom(context, {v[0], v[1], v_mid}, color);  
    filled_flat_top(context, {v[2], v[1], v_mid}, color);  
  }  
}
```

There actually isn't any need to calculate the midpoint. The only place it's used is in is either 
```cpp
const float m1 = (mid1.x - top.x) / (mid1.y - top.y);  
const float m2 = (mid2.x - top.x) / (mid2.y - top.y);  
```

But the slope (or rather inverse) is the same along the entire triangle edge, so it's not actually needed. 