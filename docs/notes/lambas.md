# Captures

While working on the 2D physics, I found that `liquid` only needed to be captures in one

``` cpp
constexpr math::Rect liquid = {
  0,
  settings.width,
  settings.height / 2,
  settings.height,
};

auto update = [&particles,  &liquid](const float dt, const input::State& input) {
	if (liquid.contains(p.position)) {
		const math::Vec2 drag = physics::generate_drag_force(p.velocity, 100.0f);
		
		p.add_force(drag);
	} 
}

};

auto render = [&particles](render::Framebuffer& fb) {
render::draw::rect(fb,
				   {liquid.x_min, liquid.y_min},
				   liquid.x_max - liquid.x_min,
				   liquid.y_max - liquid.y_min,
				   render::color::sky_blue);
};
```

In `render`, only the properties are being used, and since it's `constexpr` is used, the properties `x_min` are evaluated at compile time. 

`update` uses the `.contains` method, which is a member function that takes the implicit `this` pointer when it's called, so an address is needed, and so it needs to be captured. 