C++ programs are single threaded by default. Pretty easy to create new threads

``` cpp
#include <thread>
 
void worker() {
  // This runs on a separate thread
}
 
int main() {
  std::thread t(worker); // Explicitly spawn a new thread
  t.join(); // Wait for it to finish

  return 0;
}
```

## `static` variables

`thread_local` can be used to have one copy per thread, helping prevent race conditions with static variables (storage duration).