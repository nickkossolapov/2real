
```cpp
// Using std::transform 
std::transform(nums.begin(), nums.end(), std::back_inserter(results), square); 

// Using std::views::transform 
auto squared_view = nums | std::views::transform(square);
```

`std::views::transform`  is lazy, and can also take Functors, e.g. 

```cpp
struct Multiplier {
    int factor;
    int operator()(int n) const { return n * factor; }
};

Multiplier timesThree{3};
auto tripled = nums | std::views::transform(timesThree);
```

If just iterating, has some benefits like being able to chain operations, no allocation, and more cache friendly since the CPU doesn't have to pull in a second array. Limitations: single threaded, and lazy, so frequent accessing will involve recomputing.

Putting it back into an array:

``` cpp
| std::ranges::to<std::vector>(); // C++23
// or 
std::vector<int> evaluated_vec(view.begin(), view.end());
```