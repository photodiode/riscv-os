
#ifndef memory_h
#define memory_h

#include <types.h>


#define PAGE_SIZE_0     0x1000
#define PAGE_SIZE_1   0x200000
#define PAGE_SIZE_2 0x40000000


// memory constants
extern const u64 _k_text_start;
extern const u64 _k_text_end;
#define K_TEXT_START ((const u64)&_k_text_start)
#define K_TEXT_END   ((const u64)&_k_text_end)

extern const u64 _k_rodata_start;
extern const u64 _k_rodata_end;
#define K_RODATA_START ((const u64)&_k_rodata_start)
#define K_RODATA_END   ((const u64)&_k_rodata_end)

extern const u64 _k_data_start;
extern const u64 _k_data_end;
#define K_DATA_START ((const u64)&_k_data_start)
#define K_DATA_END   ((const u64)&_k_data_end)

extern const u64 _k_bss_start;
extern const u64 _k_bss_end;
#define K_BSS_START ((const u64)&_k_bss_start)
#define K_BSS_END   ((const u64)&_k_bss_end)

extern const u64 _k_stack_start;
extern const u64 _k_stack_size;
#define K_STACK_START ((const u64)&_k_stack_start)
#define K_STACK_SIZE  ((const u64)_k_stack_size)
// ----


typedef struct {
	u64 ra, sp, gp, tp;
	u64 t0, t1, t2;
	u64 s0, s1;
	u64 a0, a1, a2, a3, a4, a5, a6, a7;
	u64 s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	u64 t3, t4, t5, t6;

	//u64 f[32]; // floating point registers
	u64 satp; // 248

	u64 kernel_satp; // 256
	u64 kernel_sp;   // 264
	u64 kernel_trap; // 272

	u64 pagetable_address; //280

	u64 epc; //288
} trap_frame; // hart state


// kernel page allocator
void  alloc_init(u64 hart_count, const u64 memory_end);

void* alloc(u64 page_count); // allocate 4KiB page
void  _free(void** ptr);
#define free(ptr) _free((void**)&ptr) // this lets us set the pointer to NULL
// ----


#endif // memory_h
