
OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {

	. = 0x80200000;
	PROVIDE(_memory_start = .);

	.text : ALIGN(8) {
		PROVIDE(_text_start = .);
		*(.text);
		*(.text.*);
		PROVIDE(_text_end = .);
	}

	.rodata : ALIGN(8) {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	}

	.data : ALIGN(0x1000) {
		PROVIDE(_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);

	}

	.bss : ALIGN(8) {
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		PROVIDE(_bss_end = .);

	}

	.k_stack : ALIGN(0x1000) {
		PROVIDE(_k_stack_start = .);
	}

	PROVIDE(_memory_end = _memory_start + 256M);
}
