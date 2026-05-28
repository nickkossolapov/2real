# Rotation from basis vectors

To rotate so that the world axes land on unit vectors $\mathbf{x}$, $\mathbf{y}$, $\mathbf{z}$, the rotation matrix is just those vectors as columns:

$$
R = \begin{bmatrix}
x_x & y_x & z_x \\
x_y & y_y & z_y \\
x_z & y_z & z_z
\end{bmatrix}
$$

For this, there are certain constraints on $\mathbf{x}$, $\mathbf{y}$, and $\mathbf{z}$, namely:

- Unit length (no scaling)
- Mutually orthogonal (no shearing)
- Right-handed, $Z = X \times Y$ (no reflection)

This can make the rotation derivation feel too "easy". But we chose the vectors for the camera to make this true, i.e. the matrix is orthonormal

![[../attachments/Pasted image 20260528080118.png]]

