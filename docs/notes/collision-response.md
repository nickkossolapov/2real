## Penalty method

Model contact as a spring (+ damper). When bodies overlap by penetration depth
`d`, apply a restoring force $F = k d$ (minus damping) pushing them apart.
Contact becomes just another force fed into the normal integrator.

- More physically accurate, and arguable easier to reason about since it's just another force.
- Penetration exists, so softness is inherent. Stiff `k` (to look rigid) makes the ODE stiff, forcing tiny timesteps or it explodes. Tuning `k`/damping is finicky and scale/mass-dependent. Stacking is spongy and jittery. Restitution is hard to control precisely.
- Could maybe work better for modelling soft-bodies since it's more physically accurate

## Impulse method

Treat contact as instantaneous. Solve for the impulse `j` (a jump in momentum, not a force) that makes the relative normal velocity satisfy the constraint $v_{rel} \cdot n \ge 0$ (and $= -e\, v_{approach}$ for restitution). Apply $\Delta v = j\, n / m$ directly to velocities.

- Used alongside projection method (moving bodies so that no penetration is occurring)
- Bodies are truly rigid (no springiness), stable at large timesteps, restitution/friction map cleanly into the math, and it's the industry standard. Iterating the impulse solve over all contact resolves stacks well.
- Needs a lot more machinery than penalty method.

I should also look into constraint-based solvers.