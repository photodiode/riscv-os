
#ifndef memory_h
#define memory_h

#include <types.h>


#define PAGE_SIZE 4096


// memory constants
extern const u64 _text_start;
extern const u64 _text_end;
#define K_TEXT_START ((const u64)&_text_start)
#define K_TEXT_END   ((const u64)&_text_end)

extern const u64 _rodata_start;
extern const u64 _rodata_end;
#define K_RODATA_START ((const u64)&_rodata_start)
#define K_RODATA_END   ((const u64)&_rodata_end)

extern const u64 _data_start;
#define K_DATA_START ((const u64)&_data_start)

extern const u64 _k_stack_start;
extern const u64 _k_stack_size;
#define K_STACK_START ((const u64)&_k_stack_start)
#define K_STACK_SIZE  ((const u64)&_k_stack_size)

extern const u64 _memory_end;
#define MEMORY_END ((const u64)&_memory_end)
// ----


typedef struct {
	u64 x[32]; // general purpose registers
	//u64 f[32]; // floating point registers
	u64 satp;

	u64 kernel_satp;
	u64 kernel_sp;

	u64 hart_id;
	u64 pagetable_address;

	u64 pc;
} trap_frame; // hart state


// kernel page allocator
void  memory_init(u64 cpu_count);

void* alloc(u64 page_count); // allocate 4KiB page
void  _free(void** ptr);
#define free(ptr) _free((void**)&ptr) // this lets us set the pointer to NULL
// ----


#endif // memory_h
