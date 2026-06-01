Dot product properties:

- Commutative: $\mathbf{a} \cdot \mathbf{b} = \mathbf{b} \cdot \mathbf{a}$ 
- Distributive over addition: $\quad \mathbf{n} \cdot (\mathbf{a} + \mathbf{b}) = \mathbf{n} \cdot \mathbf{a} + \mathbf{n} \cdot \mathbf{b}$
- Bilinearity: $\quad (\alpha\mathbf{a} + \beta\mathbf{b}) \cdot \mathbf{c} = \alpha(\mathbf{a}\cdot \mathbf{c}) + \beta(\mathbf{b} \cdot \mathbf{c})\quad \mathbf{c} \cdot (\alpha\mathbf{a} + \beta\mathbf{b}) = \alpha(\mathbf{c} \cdot \mathbf{a}) + \beta(\mathbf{c} \cdot \mathbf{b})$

The second and third property can be used to determine which point along a line intersects a plane

Per Kenneth I. Joy, Clipping (UC Davis CG Notes):

In this case, $Q_1$ lies on the "in" side of the plane, and $Q_2$ lies on the "out" side of the plane (the case shown in the illustration). Calculate the intersection $I$ of the line and the plane by first noting that $I$ is a point on the line $\overline{Q_1 Q_2}$, and can be represented in the form

$$
I = Q_1 + t \langle Q_2 - Q_1 \rangle
$$
To determine $t$, we first subtract the identity

$$
P = P + t \langle P - P \rangle
$$

from both sides of the equation for $I$, giving

$$
\langle I - P \rangle = \langle Q_1 - P \rangle + t \bigl( \langle Q_2 - P \rangle - \langle Q_1 - P \rangle \bigr)
$$

Since $I$ and $P$ are both in the plane, $\langle I - P \rangle$ is a vector perpendicular to $\mathbf{n}$. So dotting both sides with the normal vector $\mathbf{n}$ gives

$$
\begin{aligned}
0 &= \langle I - P \rangle \cdot \mathbf{n} \\
  &= \langle Q_1 - P \rangle \cdot \mathbf{n} + t \bigl( \langle Q_2 - P \rangle \cdot \mathbf{n} - \langle Q_1 - P \rangle \cdot \mathbf{n} \bigr) \\
  &= d_1 + t (d_2 - d_1)
\end{aligned}
$$

and solving for $t$, we see that

$$
t = \frac{d_1}{d_1 - d_2}
$$