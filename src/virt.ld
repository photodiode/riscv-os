
OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {

	. = 0x80000000;
	PROVIDE(_memory_start = .);

	.text : {
		PROVIDE(_text_start = .);
		*(.text.init)
		*(.text .text.*)
		PROVIDE(_text_end = .);
	}

	. = ALIGN(0x1000);

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	}

	. = ALIGN(0x1000);

	.data : {
		PROVIDE(_data_start = .);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);

	}

	.bss :{
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		PROVIDE(_bss_end = .);

	}

	. = ALIGN(0x1000);

	PROVIDE(_k_stack_start = .);
	PROVIDE(_k_stack_size  = 0x2000);

	PROVIDE(_memory_end = _memory_start + 256M);
}
