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