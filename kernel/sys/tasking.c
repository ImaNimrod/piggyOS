#include <sys/tasking.h>

context_t* saved_context;

void tasking_init(void) {
    klog(LOG_OK, "Multitasking initialized\n");
}
