
OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {

	. = 0x0;

	.text : {
		PROVIDE(_text_start = .);
		*(.text.init)
		*(.text .text.*)
		PROVIDE(_text_end = .);
	}

	. = ALIGN(0x10);

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	}

	. = ALIGN(0x10);

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
}
