# Perspective matrix
Responsible for:
- Aspect ratio: $a = h / w$ 
- FOV factor: $f = 1 / tan( \theta / 2)$ 
- Normalisation: x, y, z to \[-1, 1\]: $\lambda = \frac{z_{far}}{z_{far} - z_{near}}$

World space to screen space: $$\begin{bmatrix}x \\ y \\ z\end{bmatrix} \implies \begin{bmatrix}a f x \\ f y \\ \lambda z - \lambda z_{near} \end{bmatrix} \implies \text{perspective divide (x/z, y/z)} => \text{screen space}$$
Projection matrix: 
$$
\begin{bmatrix}
a \cdot f & 0 & 0 & 0 \\
0 & f & 0 & 0 \\
0 & 0 & \lambda & -\lambda \cdot z_{near} \\
0 & 0 & 1 & 0
\end{bmatrix}
\begin{bmatrix}x \\ y \\ z \\ 1\end{bmatrix}
$$
`m[3][2] = 1` is to store the origin `z` to `w` for perspective divide.
# Orthographic projection

Parallel projection, same size regardless of z distance

# Perspective projections

![[../attachments/Pasted image 20260405115413.png|550]]

The triangles are similar, so

$$\frac{P'_x}{P_x} = \frac{1}{P_z} \implies P'_x = \frac{P_x}{P_z}$$

If the screen is kept a constant distance, the FOV won't actually matter, `draw_point`  is clamped by the screen size. So FOV is actually determined by screen size in that case?

Known as **perspective divide**.

# Textures

![[../attachments/Pasted image 20260428202741.png]]

It happens because there is a mismatch between screen space and true space. Because of the projection, a step in the screen x-y will not be constant in the real x-y space because of the z-divide. 

Naive barycentric interpolation assumes the attribute being interpolated (u, v, world-z, etc.) varies linearly in screen space, which it doesn't under perspective.

Look at the edges in the above picture on the left, all the coloured squares on the edge have the same length. In perspective correction, further away squares have smaller lengths. 

This incorrect texture mapping is called "affine texture mapping". The textures maintain parallel lines => affine


