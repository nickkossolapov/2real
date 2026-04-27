# Overview

Instead of linearly interpolation a bunch, you can use Barycentric coordinates to determine a point in a triangle
$$\mathbf{r} = \alpha \mathbf{r}_1 + \beta \mathbf{r}_2 + \gamma \mathbf{r}_3$$

 All the weights should equal 1: $\alpha + \beta + \gamma = 1$, and each point is $P = (\alpha , \beta , \gamma)$

 The weights refer to the ratios of the areas formed by the lines from the point to the vertices.
 
 ![[../attachments/Pasted image 20260422114652.png|367]]
(This image is counter-clockwise winding. 2real uses clockwise winding)

So $$\alpha = \frac{area_{PBC}}{area_{ABC}}$$and a neat thing is the length of $AB \times AC$ is the area of the parallelogram, so 2x area of triangle, therefore 
$$\alpha = \frac{||AP \times AC||}{||AB \times AC||}$$
# Computing for each pixel

Since barycentric coordinates are linear functions of (x, y), if you step one pixel right (x + 1), each weight changes by a fixed amount:
 $$\alpha (x+1, y) = \alpha (x, y) + d \alpha /dx$$

You precompute $d \alpha /dx$, $d \beta /dx$ (and dy variants for moving to the next row) once per triangle from the edge equations. Then rasterization is just:

``` cpp
for each row:
     α = α_row_start
     β = β_row_start
     for each pixel in row:
         γ = 1 - α - β
         if α >= 0 and β >= 0 and γ >= 0:
             // pixel is inside, interpolate and shade
         α += dα_dx
         β += dβ_dx
     α_row_start += dα_dy
     β_row_start += dβ_dy
```




