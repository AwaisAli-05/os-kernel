 void __attribute__((section(".text.kernel_main"))) kernel_main();
extern void timer_irq();

#define MAX_TASKS 5
#define QUEUES 3
#define SLICE_TICKS 5
#define AGING_LIMIT 6
#define TIMER_DIVIDER 20

typedef struct {
    char name;
    int queue;
    int slice_left;
    int waiting_slices;
    int active;
} task_t;

task_t tasks[MAX_TASKS] = {
    {'A', 0, SLICE_TICKS, 0, 1},
    {'B', 0, SLICE_TICKS, 0, 1},
    {'C', 0, SLICE_TICKS, 0, 1},
    {'D', 0, SLICE_TICKS, 0, 0},
    {'E', 0, SLICE_TICKS, 0, 0}
};

int current = 0;
int ticks = 0;
int total_slices = 0;
int tick_divider = 0;

struct idt_entry {
    unsigned short lo, sel;
    unsigned char zero, flags;
    unsigned short hi;
} __attribute__((packed)) idt[256];

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idtp;

void outb(unsigned short p, unsigned char v) {
    __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));
}

void print(int r, int c, char ch, char col) {
    char* v = (char*)0xb8000;
    v[(r*80+c)*2] = ch;
    v[(r*80+c)*2+1] = col;
}

void clear() {
    for(int i=0;i<2000;i++)
        print(i/80, i%80, ' ', 0x07);
}

int pick_next() {
    int start = (current + 1) % MAX_TASKS;

    for(int q=0; q<QUEUES; q++) {
        for(int i=0; i<MAX_TASKS; i++) {
            int idx = (start + i) % MAX_TASKS;
            if(tasks[idx].active && tasks[idx].queue == q)
                return idx;
        }
    }
    return current;
}

void apply_aging() {
    for(int i=0;i<MAX_TASKS;i++) {
        if(i==current || !tasks[i].active) continue;

        if(tasks[i].queue > 0) {
            tasks[i].waiting_slices++;
            if(tasks[i].waiting_slices >= AGING_LIMIT) {
                tasks[i].queue--;
                tasks[i].waiting_slices = 0;
            }
        }
    }
}

void irq0_handler() {
    tick_divider++;

    if(tick_divider < TIMER_DIVIDER) {
        outb(0x20,0x20);
        return;
    }
    tick_divider = 0;
    ticks++;

    if(ticks % SLICE_TICKS == 0) {
        total_slices++;

        if(tasks[current].queue < QUEUES-1)
            tasks[current].queue++;

        tasks[current].waiting_slices = 0;

        if(total_slices == 6) {
            tasks[3].active = 1;
            tasks[4].active = 1;
        }

        apply_aging();
        current = pick_next();
    }

    print(0,36,'M',0x0F);
    print(0,38,'L',0x0F);
    print(0,40,'F',0x0F);
    print(0,42,'Q',0x0F);

    print(0,70,'T',0x0F);
    print(0,71,':',0x0F);
    print(0,72,tasks[current].name,0x0E);
    print(0,74,'Q',0x0F);
    print(0,75,':',0x0F);
    print(0,76,'0'+tasks[current].queue,0x0E);

    print(1,1,'P',0x0F);
    print(1,3,'Q',0x0F);

    for(int i=0;i<MAX_TASKS;i++) {
        if(!tasks[i].active) continue;

        if(i == current) {
            print(3+i,0,'>',0x0E);
            print(3+i,1,tasks[i].name,0x0E);
            print(3+i,3,'0'+tasks[i].queue,0x0E);
        } else {
            print(3+i,0,' ',0x07);
            print(3+i,1,tasks[i].name,0x0F);
            print(3+i,3,'0'+tasks[i].queue,0x0A);
        }
    }

    outb(0x20,0x20);
}

void init_idt() {
    outb(0x20,0x11); outb(0xA0,0x11);
    outb(0x21,0x20); outb(0xA1,0x28);
    outb(0x21,0x04); outb(0xA1,0x02);
    outb(0x21,0x01); outb(0xA1,0x01);
    outb(0x21,0);    outb(0xA1,0);

    unsigned int a = (unsigned int)timer_irq;
    idt[32] = (struct idt_entry){a & 0xffff, 0x08, 0, 0x8E, a >> 16};

    idtp.limit = sizeof(idt)-1;
    idtp.base  = (unsigned int)idt;
    __asm__ volatile("lidt (%0)"::"r"(&idtp));
}

void kernel_main() {
    clear();
    init_idt();
    __asm__ volatile("sti");

    while(1)
        __asm__ volatile("hlt");
}