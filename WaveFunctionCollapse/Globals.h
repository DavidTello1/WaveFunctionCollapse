#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

// Disable STL exceptions
#ifndef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0
#endif
#define _STATIC_CPPLIB

/* CHARS AT END INDICATES THE TYPE OF LOG TO ENABLE FILTERS
* Characters must be written as string type %s
* v - verbose
* g - geometry
* d - debug
* e - error
* w - warning //TODO
* Ex: LOG("My log string: %s, my number: %d","My string", 23,'v')
*/
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)
#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long int u64;
typedef unsigned long long UID;


// Align 16, use if you have math elements in your class like float4x4 or AABB
#define ALIGN_CLASS_TO_16 \
	void* operator new(size_t i) { return _aligned_malloc(i,16); }\
    void operator delete(void* p) { _aligned_free(p); }

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }
