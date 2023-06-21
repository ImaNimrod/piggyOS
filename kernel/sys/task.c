#include <sys/task.h>

task_t* current_task;
list_t* task_list;
static volatile pid_t next_pid = 1;

void task_create(char* name, uintptr_t func) {
    context_t* context;

    task_t* t = (task_t*) kmalloc(sizeof(task_t));

    t->kstack_mem = kmalloc(0x1000); 
    if (!t->kstack_mem) {
        kfree(t);
        return;
    }

    strncpy(t->name, name, 16);
    t->pid = next_pid++;
    t->state = TASK_READY;

    /* create an address space for the new task by essentially cloning the kernel's page directory */
    t->page_dir = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(t->page_dir, 0, sizeof(page_directory_t));

    copy_page_directory(t->page_dir, kernel_page_dir);
    t->page_dir = (page_directory_t*) virt2phys(kernel_page_dir, t->page_dir);

    t->kstack = t->kstack_mem + 0x1000 - sizeof(context_t);
    context = (context_t*) t->kstack;

    context->eip = func;
    context->eflags = 0x202; // enable interrupts

    t->self = list_insert_front(task_list, t);

    schedule();
}

void task_exit(int code) {
    task_t* t = current_task;

    t->state = TASK_DEAD;

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
    if (next->state == TASK_DEAD) {
        list_remove_node(task_list, next->self);

        kfree(next->kstack_mem);
        kfree(next);

        goto _pick_next;
    }

    if (!current_task) {
        kpanic("no tasks left. this is really bad\n");
    }
   
    tss_set_stack(0x10, (uint32_t) &next->kstack);
    switch_page_directory(next->page_dir, 1);
    context_switch(next);
}

void multitasking_init(char* name, uintptr_t func) {
    klog(LOG_OK, "Initializing multitasking\n");

    task_list = list_create();
    
    /* initial kernel idle task */
    task_create(name, func);
}
