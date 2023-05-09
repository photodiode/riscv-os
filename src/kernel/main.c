
#include <types.h>

#include "print.h"


void main(u64 hart_id) {

	printf("Hello, I'm hart %d\n", hart_id);

	while (1);
}
