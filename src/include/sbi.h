
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
} sbi_error;


typedef struct {
	i64 error;
	i64 value;
} sbi_ret;


sbi_ret sbi_console_putchar(u8 byte);

sbi_ret sbi_get_spec_version(void);
sbi_ret sbi_get_impl_id(void);
sbi_ret sbi_get_impl_version(void);
sbi_ret sbi_probe_extension(i64 extension_id);
sbi_ret sbi_get_machine_vendor_id(void);
sbi_ret sbi_get_machine_arch_id(void);
sbi_ret sbi_get_machine_impl_id(void);

sbi_ret sbi_hart_start(u64 hart_id, u64 start_addr, u64 opaque);
sbi_ret sbi_hart_stop(void);
sbi_ret sbi_hart_get_status(u64 hart_id);
sbi_ret sbi_hart_suspend(u32 type, u64 resume_addr, u64 opaque);

sbi_ret sbi_send_ipi(u64 hart_mask, u64 hart_mask_base);

sbi_ret sbi_system_reset(u32 type, u32 reason);

sbi_ret sbi_set_timer(u64 time);


#endif // sbi_h
