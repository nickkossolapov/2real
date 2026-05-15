# Single file external dependencies in C++

For header only files, the below will allow the src code to resolve `#include "stb_image.h"` will  `third_party/stb_image.h`.

```
target_include_directories(2real-engine PUBLIC src third_party)
```

If they have their own cmake files etc, would probably need something like this

```
add_subdirectory(third_party/some_library)
target_link_libraries(2real-engine PUBLIC some_library
```