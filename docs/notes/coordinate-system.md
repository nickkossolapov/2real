# Internal Coordinate system

## Handedness

Using a left-handed coordinate system:

- `+x` - Right
- `+y` - Up
- `+z` - Forward (into the screen)

This matches DirectX, Unreal Engine, and Unity conventions.

## Rotation Convention

Positive angles rotate **clockwise** when looking down the positive axis (left-handed rule).

### Rotation Matrices

Rotation about `x` (pitch):

$$
R_x(\theta) = \begin{pmatrix}
1 & 0 & 0 & 0 \\
0 & \cos\theta & \sin\theta & 0 \\
0 & -\sin\theta & \cos\theta & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}
$$

Rotation about `y` (yaw):

$$
R_y(\theta) = \begin{pmatrix}
\cos\theta & 0 & -\sin\theta & 0 \\
0 & 1 & 0 & 0 \\
\sin\theta & 0 & \cos\theta & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}
$$

Rotation about `z` (roll):

$$
R_z(\theta) = \begin{pmatrix}
\cos\theta & \sin\theta & 0 & 0 \\
-\sin\theta & \cos\theta & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}
$$

## Euler Angle Order

Rotations are applied as `r_x * r_y * r_z` (pitch, yaw, then roll).

# SDL

SDL is actually y-down - so I need to invert the pixels, so `y_screen = height - y_engine`