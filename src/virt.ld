
OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {

	. = 0x80200000;
	PROVIDE(_memory_start = .);

	/* program code */
	.text : {
		PROVIDE(_k_text_start = .);
		*(.text);
		*(.text.*);
		PROVIDE(_k_text_end = .);
	}

	/* read only data */
	.rodata : ALIGN(0x1000) {
		PROVIDE(_k_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_k_rodata_end = .);
	}

	/* initialized data */
	.data : ALIGN(0x1000) {
		PROVIDE(_k_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_k_data_end = .);
	}

	/* uninitialized data */
	.bss : ALIGN(8) {
		PROVIDE(_k_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		PROVIDE(_k_bss_end = .);
	}

	.k_stack : ALIGN(0x1000) {
		PROVIDE(_k_stack_start = .);
	}
}
