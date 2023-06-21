#include <sys/tasking.h>

list_t* task_list;
task_t *current_task;
task_t *prev_task;
regs_t saved_context;

tid_t current_tid = 0;

tid_t get_tid(void) {
	return current_task->tid;
}

static void switch_context(regs_t* p_regs, context_t* n_regs) {
    if(prev_task) {
        prev_task->regs.eax = p_regs->eax;
        prev_task->regs.ebx = p_regs->ebx;
        prev_task->regs.ecx = p_regs->ecx;
        prev_task->regs.edx = p_regs->edx;
        prev_task->regs.esi = p_regs->esi;
        prev_task->regs.edi = p_regs->edi;
        prev_task->regs.ebp = p_regs->ebp;
        prev_task->regs.esp = p_regs->useresp;
        prev_task->regs.eflags = p_regs->eflags;
        prev_task->regs.eip = p_regs->eip;
        __asm__ volatile("mov %%cr3, %0" : "=r"(prev_task->regs.cr3));
    }

    if(((page_directory_t*) n_regs->cr3) != NULL) {
        switch_page_directory((page_directory_t*) n_regs->cr3, 1);
    }

    tss.esp0 = current_task->stack;

    prev_task = current_task;
    user_regs_switch(n_regs);
}


void create_task(const char* name, void *loc) {
    task_t* task = kcalloc(sizeof(task_t), 1);

    task->regs.eip = (uint32_t) loc;
    task->regs.eflags = 0x206;
    strcpy(task->name, name);

    task->regs.esp = LOAD_MEMORY_ADDRESS;

    task->page_dir = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(task->page_dir, 0, sizeof(page_directory_t));
    copy_page_directory(task->page_dir, kernel_page_dir);
    allocate_region(task->page_dir, LOAD_MEMORY_ADDRESS - 4 * PAGE_SIZE, LOAD_MEMORY_ADDRESS, 0, 0, 1);

    task->regs.cr3 = (uint32_t) virt2phys(kernel_page_dir, task->page_dir);
    task->state = TASK_STATE_ALIVE;

    if(!current_task)
        current_task = task;
    task->self = list_insert_front(task_list, task);

    kprintf("%s created\n", name);
}

/* for now, just a naive scheduler */
void scheduler(void) {
    task_t* next;
    if(!task_list->length)
        return;

    if (!current_task) {
        current_task = list_peek_front(task_list);
        prev_task = NULL;
        switch_context(NULL, &current_task->regs);
    }

    kprintf("scheduler running\n");

    node_t* nextnode = (current_task->self)->next;
    if(current_task->state == TASK_STATE_ZOMBIE) {
        list_delete(task_list, current_task->self);
        prev_task = NULL;
    }

    if(!nextnode)
        next = list_peek_front(task_list);
    else
        next = nextnode->value;

    current_task = next;
    if(current_task == NULL)
        kpanic("no process left, did you exit all user process ??? Never exit the userspace init process!!!!");

    kprintf("Scheduler chose %s to run at 0x%x\n", current_task->name, current_task->regs.eip);


    /* switch context to that of the next task */
    switch_context(&saved_context, &next->regs);
}

void tasking_init(void) {
    klog(LOG_OK, "Initializing multitasking\n");
    task_list = list_create("task list", 0);
}
