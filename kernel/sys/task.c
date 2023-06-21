#include <sys/task.h>

task_t* current_task;
list_t* task_list;

static volatile tid_t next_tid = 0;
static uint32_t prev_ticks;
extern page_directory_t* kernel_page_dir;

void task_create(char* name, uintptr_t func) {
    task_t* t = (task_t*) kcalloc(sizeof(task_t), 1);

    strncpy(t->name, name, 16);
    t->tid = next_tid++;
    t->state = TASK_READY;

    /* kernel task can just use the existing kernel_page_dir */
    if (t->tid == 0) 
        t->page_dir = kernel_page_dir;

    t->kstack_mem = kmalloc(KSTACK_SIZE);
    if (!t->kstack_mem) {
        kfree(t->kstack_mem);
        return;
    }
    
    t->kstack = (uint8_t*) t->kstack_mem + KSTACK_SIZE - sizeof(struct context);

    struct context* ctx = (struct context*) t->kstack;

    ctx->ebp = (uint32_t) t->kstack;
    ctx->eip = func;
    ctx->eflags = 0x202; // enable interrupts

    t->self = list_insert_front(task_list, t);

    schedule();
}

void task_exit(void) {
    task_t* t = current_task;

    t->state = TASK_ZOMBIE;

    schedule();
}

void schedule(void) {
    if (!task_list && !current_task)
        return;

    /* this only happens for the first task */
    if (!current_task) {
        task_t* next = list_peek_front(task_list);
        context_switch(next);
    }

    list_node_t* next_node = (current_task->self)->next;
    task_t* next;

_pick_next:

    if (!next_node)
        next = list_peek_front(task_list); // we have reached the end of the list
    else
        next = next_node->value; // get the next task in the list
    
    /* if we picked a dead task, destroy it and try again */
    if (next->state == TASK_ZOMBIE) {
        list_remove_node(task_list, next->self);

        kfree(next->kstack_mem);
        kfree(next);

        goto _pick_next;
    }

    if (!current_task) {
        kpanic("no tasks left. this is really bad\n");
    }

    /* update time used by current_task */
    current_task->usage += (get_ticks() - prev_ticks);
    prev_ticks = get_ticks();

    tss_set_stack(0x10, (uint32_t) next->kstack);
    context_switch(next);
}

void multitasking_init(char* name, uintptr_t func) {
    klog(LOG_OK, "Initializing multitasking\n");

    task_list = list_create();
    
    /* initial kernel idle task */
    task_create(name, func);
}
