# Depth buffer

Perspective matrix in code:

``` cpp
const float a = aspect_ratio;  
const float f = 1.0f / std::tan(fov / 2.0f);  
const float l = z_far / (z_far - z_near);  
  
m.m[0][0] = a * f;  
m.m[1][1] = f;  
m.m[2][2] = l;  
m.m[2][3] = -z_near * l;  
m.m[3][2] = 1.0f;
```

Multiplying  by a standard position vector:

$$\begin{bmatrix} a \cdot f & 0 & 0 & 0 \\ 0 & f & 0 & 0 \\ 0 & 0 & l & -z_{near} \cdot l \\ 0 & 0 & 1 & 0 \end{bmatrix} \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix} = \begin{bmatrix} x \cdot a \cdot f \\ y \cdot f \\ z \cdot l - z_{near} \cdot l \\ z \end{bmatrix}$$

After the matrix multiplication, values are in **Clip Space**. The coordinates are not bounded between 0 and 1 yet. Instead, they are scaled relative to that new $W$ component, dividing by $W$ takes them to **Normalized Device Coordinates (NDC)**. That's the $[0, 1]$ space. 

## Z in Clip Space/NDC

- If the original $z = z_{near}$, the top equation becomes $z_{near} \cdot l - z_{near} \cdot l = 0$. After dividing by $W$ ($z_{near}$), the final $Z$ is **$0$**.
*   If the original $z = z_{far}$, the top equation becomes $z_{far} \cdot l - z_{near} \cdot l = l(z_{far} - z_{near})$. Substituting $l$:  $$\frac{z_{far}}{z_{far} - z_{near}} \cdot (z_{far} - z_{near}) = z_{far}$$After dividing by $W$ ($z_{far}$), the final $Z$ is **$1$**.

## Non-linearity

After the perspective matrix and divide, z ends up in $[0, 1]$. But this mapping is non-linear, most of the precision is packed near the near plane:

| World z | z_ndc |
|---------|-------|
| 0.1     | 0.0   |
| 1.0     | 0.9   |
| 5.0     | 0.98  |
| 50.0    | 0.998 |
| 100.0   | 1.0   |

This causes z-fighting for distant objects.

## Reversed z-buffer

Since most of the precision is on close objects, far away objects can have conflicts. If two large walls are 80 meters away and spaced 5 centimeters apart, they will likely map to the exact same float value in the depth buffer. To avoid this, use **Reversed-Z**. 

Instead of mapping the near plane to `0.0` and the far plane to `1.0`, the matrix math is flipped so that:
* $z_{near} \rightarrow 1.0$
* $z_{far} \rightarrow 0.0$

Because floating-point numbers naturally have more precision near 0, this will nullify the floating point errors for distant objects. 


```cpp
const float a = aspect_ratio;
const float f = 1.0f / std::tan(fov / 2.0f);

// FLIPPED: l now maps near to 1 and far to 0
const float l = z_near / (z_near - z_far); 

m.m[0][0] = a * f;
m.m[1][1] = f;
m.m[2][2] = l;
m.m[2][3] = -z_far * l; // Uses z_far instead of z_near
m.m[3][2] = 1.0f;
```

## Z vs W buffer

W-buffer will produce the opposite effect of the Z-buffer. It'll resolve further triangles better, and triangles close to the camera will fight.
