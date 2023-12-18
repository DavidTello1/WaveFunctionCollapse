#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

// Disable STL exceptions
#ifndef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0
#endif
#define _STATIC_CPPLIB

// Log to console
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);
void log(const char file[], int line, const char* format, ...);

// Utils
#define CAP(n) ((n <= 0.0f) ? n = 0.0f : (n >= 1.0f) ? n = 1.0f : n = n)
#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define MAX(a,b) ((a)>(b)) ? (a) : (b)

// Typedefs
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

//  NULL just in case ----------------------
#ifdef NULL
#undef NULL
#endif
#define NULL  0

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

// --- CONFIGURATION ---
#define TITLE "Wave Function Collapse"
#define ORGANIZATION "David Tello"
#define VERSION "0.1-alpha"

#define FOLDER_ASSETS "Assets/"
#define FOLDER_SETTINGS "Settings/"
#define FOLDER_LIBRARY "Library/"

#define FOLDER_LIBRARY_SCENES "Library/Scenes/"
#define FOLDER_LIBRARY_TEXTURES "Library/Textures/"
#define FOLDER_LIBRARY_SHADERS "Library/Shaders/"
