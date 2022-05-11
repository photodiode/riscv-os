
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
	PROVIDE(_stack_start = .);
	PROVIDE(_stack_size  = 0x1000);

	PROVIDE(_memory_end = _memory_start + 256M);
}

