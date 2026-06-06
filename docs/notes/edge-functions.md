# Edge functions

The triangle rasterizer is Pineda style. For each edge `a -> b` there's an edge function evaluated at a point `p`:

```cpp
int32_t edge(const Vec2i& a, const Vec2i& b, const Vec2i& p) {
  return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}
```

It's just the z component of `(b - a) x (p - a)`. The sign tells you which side of the edge `p` is on. A pixel is inside the triangle when all three edge functions are positive (with the top-left rule deciding ties on the boundary). 

The barycentric coordinates are linear in parallel lines across the triangle:

![[../attachments/Pasted image 20260606165815.png]]

So long as you cut those lines with a straight line, the change in that line will be linear as well. So 

$$\begin{aligned}
 \Delta w_x &= a_y - b_y = \text{const} \\
 \Delta w_y &= b_x - a_x = \text{const}
 \end{aligned}$$

You can just computer the $p_{0}$ (make sure it's the center subpixel of the pixel), and then just add per row and per column for every pixel.

# Regular fixed point vs exact

Using regular fixed pixels can result in the odd empty pixel along shared edges. A shared edge is evaluated in one direction by triangle A and the reverse direction by its neighbour B, so mathematically the two values are exact negatives. The top-left rule relies on that: exactly one of them owns the boundary pixel.

The cracks came from doing this in fixed point.

Regular fixed point keeps everything at a constant scale `S`. Two numbers at scale `S` multiply to scale `S^2`, so every multiply divides back down by `S` to stay at scale `S`:

```cpp
intermediate = value * f.value;   // scale S^2
return from_raw(intermediate / scale);  // <- back to scale S, lossy
```

That `/ scale` throws away the low bits, and it rounds. An edge and its reverse round in *opposite* directions (the operands are negated), so the two values stop being exact negatives. Near the edge, where the value is ~0, both can round to "outside" and neither draws the pixel.

The fix is to not rescale at all. Snap vertices to a sub-pixel grid (4 bits, scale 16) and keep the edge function as plain integers:

- `b.x - a.x` is Q.4, `p.y - a.y` is Q.4
- product is Q.8
- leave it at Q.8, never shift back to Q.4

There's no division, so no rounding, so the result is exact. The neighbour's reverse edge is the exact negative, every time. This works because the two things used for the edge value for are both scale independent:

- **coverage** only needs the *sign*, and `S^2` is a positive constant so it doesn't change the sign
- **barycentric weights** are `edge / area`, both at scale `S^2`, so the scale cancels. This division is done once in float, *after* coverage is decided, so its rounding only affects the shade of an already-owned pixel, never whether it's drawn

So it's the same expression as a fixed point multiply minus the `>> 8`. The 4 sub-pixel bits aren't about result precision, only about how finely vertices snap.
