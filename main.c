#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "uart.h"
#include "kprintf.h"
#include "timing.h"
#include "fault.h"
#include "port.h"
#include "kernel/list.h"
#include "kernel/task.h"
#include "kernel/sched.h"
#include "kernel/time.h"
#include "kernel/kernel.h"
#include "stm32h7a3xxq.h"
#include "arch/fpu.h"

static uint32_t stack_high[512];
static uint32_t stack_mid1[512];
static uint32_t stack_mid2[512];

static tcb_t tcb_high;
static tcb_t tcb_mid1;
static tcb_t tcb_mid2;

static volatile uint32_t g_high_runs = 0;
static volatile uint32_t g_mid1_runs = 0;
static volatile uint32_t g_mid2_runs = 0;

//My eyes hurt

static uint32_t stack_hog1[512];
static uint32_t stack_hog2[512];
static uint32_t stack_worker[512];

static tcb_t tcb_hog1;
static tcb_t tcb_hog2;
static tcb_t tcb_worker;

static volatile uint32_t g_hog1_count = 0;
static volatile uint32_t g_hog2_count = 0;
static volatile uint32_t g_worker_count = 0;

static volatile uint32_t g_worker_cycles = 0;
static volatile uint32_t g_blinker_wakes = 0;

//they still hurt
volatile uint32_t g_systick_entry_cycle = 0;
volatile uint32_t g_task_woken_cycle = 0;


//they continue to hurt ill clean them i promise
static uint32_t stack_fp1[512];
static uint32_t stack_fp2[512];
static uint32_t stack_nofp[512];

static tcb_t tcb_fp1;
static tcb_t tcb_fp2;
static tcb_t tcb_nofp;

static volatile uint32_t g_fp1_err = 0;
static volatile uint32_t g_fp2_err = 0;
static volatile uint32_t g_nofp_runs = 0;

// static void task_blinker(void *arg)
// {
//     (void)arg;
//     for (;;) 
//     {
//         task_sleep(500);
//
//         g_blinker_wakes++;
//         board_led_toggle();
//
//         kprintf("[Winker Blinker] wake #%u at tick: %u (worker cycles: %u)\n", g_blinker_wakes, g_tick, g_worker_cycles);
//
//         if (g_blinker_wakes >= 5) 
//         {
//           kprintf("slave ran in background without starvation.\n");
//         }
//     }
// }



static inline void resume_task(tcb_t *tcb)
{
    uint32_t status = enter_critical();
    tcb->state = TASK_READY;
    sched_ready(tcb);
    exit_critical(status);
}

static void task_worker(void *arg)
{
    (void)arg;
    while (1) 
    {
        g_worker_count++;
    }
}


static void task_high(void *arg)
{
    (void)arg;
    for (;;) {
        g_high_runs++;

        kprintf("[MY lord Task is on the highest prority] Run %u\n", g_high_runs);

        if (g_high_runs == 3) 
        {
            kprintf("[Task is stoned] Mids should run now...\n");
            task_suspend(NULL);
        }

        task_yield();
    }
}


static void task_mid1(void *arg)
{
    (void)arg;
    
    for (;;) 
    {
        g_mid1_runs++;
        kprintf("  [My Lord, Mid1 tasks running now priority 10] run %u\n", g_mid1_runs);

        if (g_mid1_runs >= 5) {
            task_suspend(NULL);
        }
        
        task_yield();
    }
}


static void task_mid2(void *arg)
{
    (void)arg;

    for (;;) 
    {
        g_mid2_runs++;
        kprintf("  [My LOrd Mid2 Tasks running now, at 10] Run %u\n", g_mid2_runs);

        if (g_mid1_runs >= 4 && g_mid2_runs >= 4) 
        {

            kprintf("No Wayy Priority Pick worked ggs\n");
            break;
        }

          task_yield();
    }
    //SOme kind of tick-wrapping Test [Paneer Roll]
    
    uint32_t status = enter_critical();
    g_tick = 0xFFFFFFE0UL; // 32 ticks before wrap 
    exit_critical(status);
    
    kprintf("[Paneer Roll] before nini tick: 0x%x\n", g_tick);
    task_sleep(50); // This should cross the wrap to 0x00000012 
    kprintf("[Paneer Roll] after nini tick: 0x%x\n", g_tick);

    // Measure Preemption Latency 
    for (int i = 0; i < 3; i++) 
    {
        task_sleep(10);
        uint32_t latency = (uint32_t)(g_task_woken_cycle - g_systick_entry_cycle); 
        kprintf("preempt latency: %u cycles\n", latency);
    }

    // Check if worker was crying in the background
    kprintf("[Slave Check] worker cooked %u loops while we slept\n", g_worker_count);

    // watermarks
    kprintf("[Watermarks] Free real estate:\n");

    kprintf("  high:   %u bytes\n", (uint32_t)task_stack_watermark(&tcb_high));
    kprintf("  mid1:   %u bytes\n", (uint32_t)task_stack_watermark(&tcb_mid1));
    kprintf("  mid2:   %u bytes\n", (uint32_t)task_stack_watermark(&tcb_mid2));
    kprintf("  hog1:   %u bytes\n", (uint32_t)task_stack_watermark(&tcb_hog1));
    kprintf("  hog2:   %u bytes\n", (uint32_t)task_stack_watermark(&tcb_hog2));
    kprintf("  worker: %u bytes\n", (uint32_t)task_stack_watermark(&tcb_worker));

    // Wake up the pigs to test equal prio slicing without yields
    kprintf("[Pigs unleashed] Hog1 and Hog2 boutta fight at prio 15...\n");
    resume_task(&tcb_hog1);
    resume_task(&tcb_hog2);

    task_sleep(100); // let them slice each other's throats for 100 ticks
    kprintf("Hog1 ate %u chunks, Hog2 ate %u chunks\n", g_hog1_count, g_hog2_count);

    kprintf("FPU gonna take my virginity.\n");
        for (;;) 
    {
        board_led_toggle();
        for (volatile int i = 0; i < 2000000; i++) __NOP();
    }
}

static void task_hog1(void *arg)
{
    (void)arg;
    while (1) 
    {
        g_hog1_count++;
        for (volatile int i = 0; i < 1000; i++) __NOP();
    }
    //task_suspend(NULL);
}

static void task_hog2(void *arg)
{
    (void)arg;
    while (1) 
    {
        g_hog2_count++;
        for (volatile int i = 0; i < 1000; i++) __NOP();
    }
}

static inline void set_s16(float val) {
    __asm volatile ("vmov s16, %0" :: "r"(*(uint32_t *)&val));
}

static inline float get_s16(void) {
    uint32_t res;
    __asm volatile ("vmov %0, s16" : "=r"(res));
    return *(float *)&res;
}

static void task_fp1(void *arg)
{
    (void)arg;
    float seed = 123.456f;
    for (int i = 0; i < 500; i++) {
        set_s16(seed);
        task_yield();
        if (get_s16() != seed) {
            g_fp1_err++;
        }
        seed += 1.0f;
    }
    kprintf("FP1 %u errors\n", g_fp1_err);
    task_suspend(NULL);
}

static void task_fp2(void *arg)
{
    (void)arg;
    float seed = 987.654f;
    for (int i = 0; i < 500; i++) {
        set_s16(seed);
        task_yield();
        if (get_s16() != seed) {
            g_fp2_err++;
        }
        seed += 2.0f;
    }
    kprintf("FP2 %u errors\n", g_fp2_err);
    task_suspend(NULL);
}

static void task_nofp(void *arg)
{
    (void)arg;
    for (int i = 0; i < 500; i++) {
        g_nofp_runs++;
        task_yield();
    }
    kprintf("%u without touching FPU inappropriately\n", g_nofp_runs);
    task_suspend(NULL);
}


int main(void)
{
    board_init();
    uart_init();
    timing_init();
    timing_dwt_init();
    fault_init();
    port_init();

    fpu_init(false);

    sched_init();
    time_init();

    kprintf("[Shannon-RTOS] FPU Test\n");

    /*task_create(&tcb_hog1, stack_hog1, sizeof(stack_hog1), task_hog1, NULL, "hog1", 10);
    task_create(&tcb_hog2, stack_hog2, sizeof(stack_hog2), task_hog2, NULL, "hog2", 10);*/

    task_create(&tcb_fp1,  stack_fp1,  sizeof(stack_fp1),  task_fp1,  NULL, "fp1",  10);
    task_create(&tcb_fp2,  stack_fp2,  sizeof(stack_fp2),  task_fp2,  NULL, "fp2",  10);
    task_create(&tcb_nofp, stack_nofp, sizeof(stack_nofp), task_nofp, NULL, "nofp", 10);

    kernel_start();

    return 0;
}




