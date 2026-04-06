Where to use `#define` preprocessor in C++

1. Include guards (though `#pragma once` is simpler)
	``` cpp
	#ifndef MY_HEADER_H 
	#define MY_HEADER_H`
	// ...
	#endif
	```
2. Conditional compilation
	``` cpp
	#define DEBUG
	#ifdef DEBUG
	// debug-only code
	#endif
	```
3. Stringification (const can't do this)
	``` cpp
	#define STRINGIFY(x) #x
	STRINGIFY(hello) // → "hello"
	```
4. Token pasting / macros with code generation
	``` cpp
	#define MAKE_GETTER(field) int get_##field() { return field; }
	```
 