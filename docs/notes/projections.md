# Orthographic projection

Parallel projection, same size regardless of z distance

# Perspective projections

![[../attachments/Pasted image 20260405115413.png|550]]

The triangles are similar, so

$$\frac{P'_x}{P_x} = \frac{1}{P_z} \implies P'_x = \frac{P_x}{P_z}$$

If the screen is kept a constant distance, the FOV won't actually matter, `draw_point`  is clamped by the screen size.
So FOV is actually determined by screen size in that case.

Known as **perspective divide**.



