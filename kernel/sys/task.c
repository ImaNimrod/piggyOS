#include <sys/task.h>

static task_t ktask;
task_t* current;
static list_t* task_list;
static pid_t next_pid = 1;

int task_init(task_t* t, task_entry_t entry) {
    char* ti;
    uint32_t* sp;

    t->pid = next_pid++;

    if (t == &ktask) {
        t->page_dir = kernel_page_dir;
        t->context = NULL;
    }

    // t->page_dir = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    // if (!t->page_dir)
    //     return -1;
    // memset(t->page_dir, 0, sizeof(page_directory_t));
    // copy_page_directory(t->page_dir, kernel_page_dir);

    ti = (char*) kmalloc(4096);
    if (!ti)
        return -1;

    sp = (uint32_t*) ALIGN_DOWN((uintptr_t) ti + 4096, sizeof(uint32_t));

    // if (current->isr_frame) {
    //     regs_t* isr_frame2 = ((regs_t*) sp) - 1;
    //
    //     sp = (uint32_t*) isr_frame2;
    //
    //     *isr_frame2 = *current->isr_frame;
    //     isr_frame2->eax = 0;
    // }

    t->context = ((context_t*) sp) - 1;
    t->context->ebp = (uint32_t) sp;
    t->context->eip = (uint32_t) entry;
    t->context->ebx = 0;
    t->context->edi = 0;
    t->context->esi = 0;

    return 0;
}

task_t* task_create(task_entry_t entry) {
    task_t* t = (task_t*) kcalloc(sizeof(task_t), 1);
    if (task_init(t, entry) < 0) {
        kfree(t);
        klog(LOG_ERR, "Failed to create task...\n");
        return NULL;
    }

    t->self = list_insert_front(task_list, t);
    schedule();
    
    return t;
}

void task_delete(task_t* t) {
    kfree((void*) ALIGN_DOWN((uint32_t) t->context, 4096));
    kfree(t);
}

void schedule(void) {
    if(!list_size(task_list))
        return;

    task_t* last;
    task_t* next;

    last = current;
    list_node_t* nextnode = (current->self)->next;

    if (current->state == TASK_ZOMBIE)
        list_remove_node(task_list, current->self);

    if (!nextnode)
        next = list_peek_front(task_list);
    else
        next = nextnode->value;

    if (next == current) {
        ktask.state = TASK_RUNNING;
        next = &ktask;
    }

    current = next;

    // perform context switch 
    kprintf("scheduler chose to run task: %d\n", next->pid);
    tss.esp0 = ALIGN_UP((uint32_t) next->context, 4096);
    // switch_page_directory((page_directory_t*) next->page_dir, 1);

    context_switch(&last->context, next->context);
}

void tasking_init(void) {
    klog(LOG_OK, "Initializing multitasking\n");
    task_list = list_create();
}
