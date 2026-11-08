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


static uint32_t stack_high[256];
static uint32_t stack_mid1[256];
static uint32_t stack_mid2[256];

static tcb_t tcb_high;
static tcb_t tcb_mid1;
static tcb_t tcb_mid2;

static volatile uint32_t g_high_runs = 0;
static volatile uint32_t g_mid1_runs = 0;
static volatile uint32_t g_mid2_runs = 0;


static uint32_t stack_hog1[256];
static uint32_t stack_hog2[256];

static tcb_t tcb_hog1;
static tcb_t tcb_hog2;

static volatile uint32_t g_count1 = 0;
static volatile uint32_t g_count2 = 0;

//My eyes hurt

static uint32_t stack_blinker[256];
static uint32_t stack_worker[256];

static tcb_t tcb_blinker;
static tcb_t tcb_worker;

static volatile uint32_t g_worker_cycles = 0;
static volatile uint32_t g_blinker_wakes = 0;


static void task_blinker(void *arg)
{
    (void)arg;
    for (;;) 
    {
        task_sleep(500);

        g_blinker_wakes++;
        board_led_toggle();

        kprintf("[Winker Blinker] wake #%u at tick: %u (worker cycles: %u)\n", g_blinker_wakes, g_tick, g_worker_cycles);

        if (g_blinker_wakes >= 5) 
        {
          kprintf("slave ran in background without starvation.\n");
        }
    }
}

static void task_worker(void *arg)
{
    (void)arg;
    for (;;) 
    {
        g_worker_cycles++;
        for (volatile int i = 0; i < 2000; i++) 
        {
            __asm volatile ("nop");
        }
    }
}


static void task_high(void *arg)
{
    (void)arg;
    for (;;) {
        g_high_runs++;
        kprintf("[MY lord Task is on the highest prority] Run %u\n", g_high_runs);

        if (g_high_runs >= 3) {
            kprintf("[Task is stoned] Mids should run now...\n");
            sched_unready(&tcb_high);
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
        
        //Rotate ready list so Mid-2 gets a turn on yield 
        sched_rotate(10);
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

        if (g_mid1_runs >= 5 && g_mid2_runs >= 5) 
        {

            kprintf("No Wayy Priority Pick worked ggs\n");
          
            for (;;) 
            {
                board_led_toggle();
                board_delay_ms(500);
            }

        }
  
        sched_rotate(10);
        task_yield();
    }
             
}

static void task_hog1(void *arg)
{
    (void)arg;
    for (;;) 
    {
        g_count1++;
        //no yield hog
        for (volatile int i = 0; i < 20000; i++) 
        {
            __asm volatile ("nop");
        }
        if ((g_count1 % 50) == 0) 
        {
            kprintf("tick: %u | Hog1: %u | Hog2: %u\n", g_tick, g_count1, g_count2);
            board_led_toggle();
        }
    }
}

static void task_hog2(void *arg)
{
    (void)arg;
    for (;;) 
    {
        g_count2++;
        for (volatile int i = 0; i < 20000; i++) 
        {
            __asm volatile ("nop");
        }
    }
}

int main(void)
{
    board_init();
    uart_init();
    timing_init();
    timing_dwt_init();
    fault_init();
    port_init();
    sched_init();
    time_init();

    kprintf("[Shannon-RTOS] Nini Test\n");


    /*task_create(&tcb_high, stack_high, sizeof(stack_high), task_high, NULL, "high", 5);
    task_create(&tcb_mid1, stack_mid1, sizeof(stack_mid1), task_mid1, NULL, "mid1", 10);
    task_create(&tcb_mid2, stack_mid2, sizeof(stack_mid2), task_mid2, NULL, "mid2", 10);*/

    /*task_create(&tcb_hog1, stack_hog1, sizeof(stack_hog1), task_hog1, NULL, "hog1", 10);
    task_create(&tcb_hog2, stack_hog2, sizeof(stack_hog2), task_hog2, NULL, "hog2", 10);*/

    task_create(&tcb_blinker, stack_blinker, sizeof(stack_blinker), task_blinker, NULL, "blinker", 8);
    task_create(&tcb_worker, stack_worker, sizeof(stack_worker), task_worker, NULL, "worker", 20);
   
   kernel_start();

    for (;;) 
    {
        __asm volatile ("nop");
    }

    return 0;
}




