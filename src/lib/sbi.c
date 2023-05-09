
#include <types.h>
#include <riscv.h>

#include "sbi.h"


typedef enum {
	SBI_EID_BASE   =       0x10,
	SBI_EID_HART   =   0x48534D,
	SBI_EID_IPI    =   0x735049,
	SBI_EID_RFENCE = 0x52464E43,
	SBI_EID_RESET  = 0x53525354,
	SBI_EID_TIME   = 0x54494D45,
} sbi_eid;


static inline sbi_ret _sbi_ecall(register u64 a0, register u64 a1, register u64 a2, register u64 a3, register u64 a4, register u64 a5, register u64 fid, register u64 eid) {

	(void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)fid; (void)eid;

	asm("ecall");

	return (sbi_ret) {
		.error = regr(a0),
		.value = regr(a1),
	};
}

static inline sbi_ret sbi_ecall(u64 eid, u64 fid, u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 a5) {
	return _sbi_ecall(a0, a1, a2, a3, a4, a5, fid, eid);
}


// legacy
sbi_ret sbi_console_putchar(u8 byte) {
	return sbi_ecall(0x01, 0, byte, 0, 0, 0, 0, 0);
}
// ----


// base extension
sbi_ret sbi_get_spec_version(void) {
	return sbi_ecall(SBI_EID_BASE, 0, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_get_impl_id(void) {
	return sbi_ecall(SBI_EID_BASE, 1, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_get_impl_version(void) {
	return sbi_ecall(SBI_EID_BASE, 2, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_probe_extension(i64 extension_id) {
	return sbi_ecall(SBI_EID_BASE, 3, extension_id, 0, 0, 0, 0, 0);
}

sbi_ret sbi_get_machine_vendor_id(void) {
	return sbi_ecall(SBI_EID_BASE, 4, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_get_machine_arch_id(void) {
	return sbi_ecall(SBI_EID_BASE, 5, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_get_machine_impl_id(void) {
	return sbi_ecall(SBI_EID_BASE, 6, 0, 0, 0, 0, 0, 0);
}
// ----


// hart state management extension
sbi_ret sbi_hart_start(u64 hart_id, u64 start_addr, u64 opaque) {
	return sbi_ecall(SBI_EID_HART, 0, hart_id, start_addr, opaque, 0, 0, 0);
}

sbi_ret sbi_hart_stop(void) {
	return sbi_ecall(SBI_EID_HART, 1, 0, 0, 0, 0, 0, 0);
}

sbi_ret sbi_hart_get_status(u64 hart_id) {
	return sbi_ecall(SBI_EID_HART, 2, hart_id, 0, 0, 0, 0, 0);
}

sbi_ret sbi_hart_suspend(u32 type, u64 resume_addr, u64 opaque) {
	return sbi_ecall(SBI_EID_HART, 3, type, resume_addr, opaque, 0, 0, 0);
}
// ----


// inter processor interrupt extension
sbi_ret sbi_send_ipi(u64 hart_mask, u64 hart_mask_base) {
	return sbi_ecall(SBI_EID_IPI, 0, hart_mask, hart_mask_base, 0, 0, 0, 0);
}
// ----


// system reset extension
sbi_ret sbi_system_reset(u32 type, u32 reason) {
	return sbi_ecall(SBI_EID_RESET, 0, type, reason, 0, 0, 0, 0);
}
// ----


// timer extension
sbi_ret sbi_set_timer(u64 time) {
	return sbi_ecall(SBI_EID_TIME, 0, time, 0, 0, 0, 0, 0);
}
// ----
