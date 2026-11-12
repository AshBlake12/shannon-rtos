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
#include "kernel/semaphore.h"

//i cleaned ts thank me later
//
static uint32_t stack_bench[512];
static tcb_t tcb_bench;

static sem_t g_sem_hw;

volatile uint32_t g_isr_give_cycles = 0;
volatile uint32_t g_task_recv_cycles = 0;

static void print_banner(void)
{
    kprintf("\r\n");
    kprintf(" ████ █   █  ███  █   █ █   █  ███  █   █    ████  █████  ███   ████ \r\n");
    kprintf("█     █   █ █   █ ██  █ ██  █ █   █ ██  █    █   █   █   █   █ █     \r\n");
    kprintf(" ███  █████ █████ █ █ █ █ █ █ █   █ █ █ █    ████    █   █   █  ███  \r\n");
    kprintf("    █ █   █ █   █ █  ██ █  ██ █   █ █  ██    █  █    █   █   █     █ \r\n");
    kprintf("████  █   █ █   █ █   █ █   █  ███  █   █    █   █   █    ███  ████  \r\n");
    kprintf("                   Shashank [github.com/AshBlake12]                  \r\n");
    kprintf("\r\n");
}

void TIM6_DAC_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_UIF) 
    {
        TIM6->SR &= ~TIM_SR_UIF;

        // timestamp cycle count right before waking the task
        g_isr_give_cycles = DWT->CYCCNT;
        semaphore_give(&g_sem_hw);
    }
}

static void task_benchmark(void *arg)
{
    (void)arg;

    // start timer ONLY now that the task is alive and ready to block
    extern void tim6_init(uint16_t psc, uint16_t arr);
    tim6_init(23999, 99);

    for (int i = 0; i < 10; i++) 
    {
        // blocks until tim6 gives the semaphore
        semaphore_take(&g_sem_hw, K_FOREVER);
        g_task_recv_cycles = DWT->CYCCNT;

        uint32_t latency = g_task_recv_cycles - g_isr_give_cycles;
        kprintf("[bench] sample %d: uncle to baccha latency = %u cycles\n", i, latency);
    }

    // stop timer so it stops spamming interrupts
    TIM6->CR1 &= ~TIM_CR1_CEN;
    kprintf("Succesful sacrifice done\n");

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
    fpu_init(true);

    sched_init();
    time_init();
    
    print_banner();
    kprintf("ISR Benchmark w Semaphores\n");

    // binary semaphore starts empty
    semaphore_init(&g_sem_hw, 0, 1);

    // priority 8 runs higher than idle
    task_create(&tcb_bench, stack_bench, sizeof(stack_bench), task_benchmark, NULL, "bench", 8);


    kernel_start();

    return 0;
}



