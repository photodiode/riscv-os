
#ifndef types_h
#define types_h

// unsigned integers
typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

#define U8_MAX  (0xFF)
#define U16_MAX (0xFFFF)
#define U32_MAX (0xFFFFFFFFU)
#define U64_MAX (0xFFFFFFFFFFFFFFFFUL)
// ----

// signed integers
typedef char   i8;
typedef short i16;
typedef int   i32;
typedef long  i64;

#define I8_MAX  (0x7F)
#define I16_MAX (0x7FFF)
#define I32_MAX (0x7FFFFFFF)
#define I64_MAX (0x7FFFFFFFFFFFFFFFL)

#define I8_MIN  (-I8_MAX-1)
#define I16_MIN (-I16_MAX-1)
#define I32_MIN (-I32_MAX-1)
#define I64_MIN (-I64_MAX-1)
// ----

// floating point
/*//typedef ???    f8;
typedef __fp16 f16;
typedef float  f32;
typedef double f64;

#define F16_MIN 0x1.0p-14f
#define F32_MIN 0x1.0p-126f
#define F64_MIN 0x1.0p-1022

#define F16_MAX 0x1.FFCp15f
#define F32_MAX 0x1.FFFFFEp127f
#define F64_MAX 0x1.FFFFFFFFFFFFFp1023

#define INFINITY __builtin_inff()
#define NAN      __builtin_nanf("")*/
// ----

// booleans
typedef _Bool bool;
#define true  1
#define false 0
// ----

// pointers
#define NULL ((void*)0)
// ----

// extras
//#define packed __attribute__((__packed__))
//#define aligned(BYTES) __attribute__((aligned(BYTES)))
// ----

#endif // types_h
