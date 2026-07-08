![[../attachments/Pasted image 20260707165138.png]]

A naive way to get the contact information from SAT it just to find the point with the most depth. However, this probably won't be very stable for stacking cubes since there's only a single contact point there will be wobble. 

Could use clipping (Sutherland-Hogman from my good old software renderer) to get a contact manifold instead, that will give a contact area which would be more stable.

## AB vs BA

When checking separation, both directions needs to be checked. I originally had this

``` cpp
const auto ab_separation = find_min_separation(a_points, b_points);

if (ab_separation.distance >= 0) {
return {};
}

const auto ba_separation = find_min_separation(b_points, a_points);

// Return contact info

```

This resulted in detection happening even when the boxes weren't contacting

![[../attachments/Pasted image 20260708171348.png]]

In this case AB didn't have a separating axis parallel to its edges, while BA did. Since it wasn't checked, it still reported the contact. 