
#include <types.h>

#include "print.h"


typedef struct __attribute__((packed)) {
	const u32 magic;
	const u32 version;
	// 0x0008
	const u32 type;
	const u32 vendor;
	const u32 features;
	u32       feature_select;
	// 0x0018
	const u32 pad_1[2];
	// 0x0020
	u32       driver_features;
	u32       driver_feature_select;
	// 0x0028
	const u32 pad_2[2];
	// 0x0030
	u32       queue_select;
	const u32 queue_count_max;
	u32       queue_count;
	// 0x003c
	const u32 pad_3[2];
	// 0x0044
	u32       queue_ready;
	// 0x0048
	const u32 pad_4[2];
	// 0x0050
	u32       queue_notify;
	// 0x0054
	const u32 pad_5[3];
	// 0x0060
	const u32 interrupt_status;
	u32       interrupt_ack;
	// 0x0068
	const u32 pad_6[2];
	// 0x0070
	u32       status;
	// 0x0074
	const u32 pad_7[35];
	// 0x0100
	u32 config[0x3C0];
	// 0x1000
} vio_device;


static const u8 virtio_device_count = 8;
static volatile vio_device* const virtio = (void*)0x10001000;


void virtio_init() {

	for (u8 i = 0; i < virtio_device_count; i++) {

		if (virtio[i].magic != 0x74726976) continue;
		if (virtio[i].type == 0) continue;

		switch (virtio[i].type) {
			case  2: {
				printf("%d: virtio block device\n", i);
				break;
			}
			case 16: {
				printf("%d: virtio gpu device\n", i);
				break;
			}
			case 18: {
				printf("%d: virtio input device\n", i);
				break;
			}
			default: break;
		}
	}
}
