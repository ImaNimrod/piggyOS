#include <sys/task.h>

task_t* current_task;
list_t* task_list;

static volatile tid_t next_tid = 0;

static uint32_t prev_ticks;
static uint32_t curr_ticks;

extern page_directory_t* current_page_dir;

task_t* task_create(uintptr_t func) {
    task_t* t = (task_t*) kcalloc(sizeof(task_t), 1);

    t->tid = next_tid++;
    t->state = TASK_READY;
    t->exit_code = 0;

    t->ifr = NULL;

    t->page_dir = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    copy_page_directory(t->page_dir, current_page_dir);

    t->kstack_mem = kmalloc(KSTACK_SIZE);
    if (!t->kstack_mem) {
        kfree(t->kstack_mem);
        return NULL;
    }
    
    t->kstack = (uint8_t*) t->kstack_mem + KSTACK_SIZE - sizeof(struct context);

    if (current_task->ifr) {
        regs_t* ifr2 = ((regs_t*) t->kstack) - 1; 

        t->kstack = (uint32_t*) ifr2;
        *ifr2 = *current_task->ifr;
        ifr2->eax = 0;
    }

    struct context* ctx = (struct context*) t->kstack;

    ctx->ebp = (uint32_t) t->kstack;
    ctx->eip = func;
    ctx->eflags = 0x202; // enable interrupts

    t->self = list_insert_front(task_list, t);

    schedule();

    return t;
}

void task_destroy(task_t* t) {
    list_remove_node(task_list, t->self);

    kfree(t->kstack_mem);
    kfree(t->page_dir);
    kfree(t);
}

void task_exit(int code) {
    task_t* t = current_task;

    t->exit_code = code;
    t->state = TASK_ZOMBIE;

    schedule();
}

void schedule(void) {
    if (!task_list)
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
        task_destroy(next);
        goto _pick_next;
    }

    if (!current_task) {
        kpanic("no tasks left. this is really bad\n");
    }

    /* update time used by current_task */
    curr_ticks = get_ticks();
    current_task->usage += curr_ticks - prev_ticks;
    prev_ticks = curr_ticks;

    tss_set_stack(0x10, (uint32_t) next->kstack);
    switch_page_directory(next->page_dir);
    context_switch(next);
}

void multitasking_init(uintptr_t func) {
    klog(LOG_OK, "Initializing multitasking\n");

    task_list = list_create();
    
    /* initial kernel idle task */
    task_create(func);
}
