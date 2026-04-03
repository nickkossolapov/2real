# Roadmap
## Overall goal
A final scene with a stable stack of 50+ cubes that I can knock over and don't explode or shiver because it's very cold

## Render
- [ ] Get stuff on the screen
- [ ] Understand quaternions
- [ ] Try build math classes - vec3, mat4, and quaternion etc

## Physics
- [ ] Get things moving  
- [ ] Tensors!
- [ ] Numerical stability (object sleeping?)

## "Real" Physics
- [ ] Try do stuff that actually works well on computers
- [ ] GJK & EPA - convex shape collisions 
- [ ] Persistent Contact Manifolds to generate stable points of contact for realistic stacking
- [ ] Grid or AABB Tree so the engine doesn't lag when there are 20+ objects  

# Along the way
- [ ] Explore C++ build tools (and don't use just CLion's built-in ones)
- [ ] Implement more of the math myself, e.g. SIMD (SSE/AVX)
- [ ] Memory management - maybe an arena allocator?
- [ ] Threading - see if I can move the physics solver to its own CPU thread
- [ ] Profiling - build a frame timer to break down physics vs rendering etc.

# Possible resources
A lot of this will be following existing resources, so these are mostly references for ones I want to explore
* Pikuma's 3D Graphics & Physics Courses
* Real-Time Collision Detection by Christer Ericson
* Game Engine Architecture by Jason Gregory