
#ifndef sbi_h
#define sbi_h


#include <types.h>


typedef enum {
	SBI_SUCCESS               =  0,
	SBI_ERR_FAILED            = -1,
	SBI_ERR_NOT_SUPPORTED     = -2,
	SBI_ERR_INVALID_PARAM     = -3,
	SBI_ERR_DENIED            = -4,
	SBI_ERR_INVALID_ADDRESS   = -5,
	SBI_ERR_ALREADY_AVAILABLE = -6,
	SBI_ERR_ALREADY_STARTED   = -7,
	SBI_ERR_ALREADY_STOPPED   = -8,
} sbi_error_types;


// SBI implementation IDs
static const char* sbi_impl_id_strings[] = {
	"Berkeley Boot Loader (BBL)",
	"OpenSBI",
	"Xvisor",
	"KVM",
	"RustSBI",
	"Diosix",
	"Coffer"
};


extern i64 sbi_error;


i64 sbi_console_putchar(u8 byte);

i64 sbi_get_spec_version(void);
i64 sbi_get_impl_id(void);
i64 sbi_get_impl_version(void);
i64 sbi_probe_extension(i64 extension_id);
i64 sbi_get_machine_vendor_id(void);
i64 sbi_get_machine_arch_id(void);
i64 sbi_get_machine_impl_id(void);

i64 sbi_hart_start(u64 hart_id, u64 start_addr, u64 opaque);
i64 sbi_hart_stop(void);
i64 sbi_hart_get_status(u64 hart_id);
i64 sbi_hart_suspend(u32 type, u64 resume_addr, u64 opaque);

i64 sbi_send_ipi(u64 hart_mask, u64 hart_mask_base);

i64 sbi_system_reset(u32 type, u32 reason);

i64 sbi_set_timer(u64 time);


#endif // sbi_h
