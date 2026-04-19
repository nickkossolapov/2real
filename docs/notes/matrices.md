# Order
Matrices are applied right to left, so in `A * B * C * v`, `C` is first applied to `v`, then `B`, then `A`

So when creating world matrix:

```cpp
translation * rotation * scale * vertex
//  3rd         2nd       1st
```

# Translation
Translation is not a linear transformation. A linear transformation must map the origin to the origin, and translation moves it. 

That's why we use a 3x3 matrix can't be used for translation. By adding the extra dimension (w), translation becomes linear in 4D. The fourth dimension is just a mathematical trick to embed 3D space into 4D where translation is just another matrix multiply, then project back down (`to_vec3`).

This is an **affine transformation**: a linear transformation + translation. Lines still remain parallel.

The transformation matrix is a linear transformation in 4D though. Like sheering in 4D. 2D case with 3D sheering:

![[../attachments/Pasted image 20260418213338.png|542]]

And then we can then ignore the 3D component, to project it back down to 2D.

See [transformation matrix](https://en.wikipedia.org/wiki/Transformation_matrix).

# Column Major vs Row Major

There are two conventions for how vectors and matrices interact. They produce the same results but affect the code and how matrices are laid out in memory. 

The two forms are transpositions of each other.

2real uses column vectors, and most maths and physics resources use column vectors.

## Column Vectors (Column Major)

Translation lives in the **last column**:

$$
\begin{bmatrix}
m_{00} & m_{01} & m_{02} & t_x \\
m_{10} & m_{11} & m_{12} & t_y \\
m_{20} & m_{21} & m_{22} & t_z \\
0 & 0 & 0 & 1
\end{bmatrix}
\begin{bmatrix} x \\ y \\ z \\ w \end{bmatrix}
$$

- Code: `M * v`
- Post-multiplication
- Transforms compose left-to-right: `T * R * S * v` (scale first, then rotate, then translate)
- Used by: **OpenGL, GLM, Unity, 2real**

## Row Vectors (Row Major)

Translation lives in the **last row**:

$$
\begin{bmatrix} x & y & z & w \end{bmatrix}
\begin{bmatrix}
m_{00} & m_{10} & m_{20} & 0 \\
m_{01} & m_{11} & m_{21} & 0 \\
m_{02} & m_{12} & m_{22} & 0 \\
t_x & t_y & t_z & 1
\end{bmatrix}
$$

- Code: `v * M`
- Pre-muplication
- Transforms compose right-to-left: `v * S * R * T` (scale first, then rotate, then translate)
- Used by: **DirectX, Unreal Engine, HLSL**

## Transpose of a Product

The two conventions are related by the transpose identity:

$$(M \cdot N)^T = N^T \cdot M^T$$

This extends to any chain:

$$(A \cdot B \cdot C)^T = C^T \cdot B^T \cdot A^T$$