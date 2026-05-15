# vertice textures

[OBJ spec](https://paulbourke.net/dataformats/obj/obj_spec.pdf)

It doesn't seem to mention any ranges for `vt`, just that the default is 0. Surprsingly hard to find good info on this behaviour. Some YouTube videos mention it does tiling (so `u = u % 1`), but some also mention just plain clipping ([like these Blender docs?](https://wiki.blender.jp/Doc:2.6/Manual/Textures/Mapping/UV/Unwrapping))

> Any UVs that lie outside the 0 to 1 range will be clipped to that range by being moved to the UV space border it is closest to.

[OpenGL has this configurable](https://learnopengl.com/Getting-started/Textures), with the default being tiling (just ignoring integer part)

> Texture coordinates usually range from `(0,0)` to `(1,1)` but what happens if we specify coordinates outside this range? The default behavior of OpenGL is to repeat the texture images (we basically ignore the integer part of the floating point texture coordinate), but there are more options OpenGL offers:
>
> - `GL_REPEAT`: The default behavior for textures. Repeats the texture image.
> - `GL_MIRRORED_REPEAT`: Same as `GL_REPEAT` but mirrors the image with each repeat.
> - `GL_CLAMP_TO_EDGE`: Clamps the coordinates between `0` and `1`. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
> - `GL_CLAMP_TO_BORDER`: Coordinates outside the range are now given a user-specified border color.

I think for now I'll go with the OpenGL behaviour