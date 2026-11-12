#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "uart.h"
#include "kprintf.h"
#include "timing.h"
#include "fault.h"
#include "port.h"
#include "kernel/task.h"
#include "kernel/sched.h"
#include "kernel/time.h"
#include "kernel/kernel.h"
#include "arch/fpu.h"

//basic demo, put ur own tasks here
static uint32_t stack_blink[512];
static uint32_t stack_hello[512];

static tcb_t tcb_blink;
static tcb_t tcb_hello;

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

// blinks led every 500ms, if this stops kernel is ded
static void task_blink(void *arg)
{
    (void)arg;
    while (1)
    {
        board_led_toggle();
        task_sleep(500);
    }
}

static void task_hello(void *arg)
{
    (void)arg;
    uint32_t n = 0;
    while (1)
    {
        kprintf("[hello] tick %u\n", n++);
        task_sleep(1000);
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
    fpu_init(true);

    sched_init();
    time_init();

    kprintf_init_lock();

    print_banner();

    // lower num = higher prio
    task_create(&tcb_blink, stack_blink, sizeof(stack_blink), task_blink, NULL, "blink", 10);
    task_create(&tcb_hello, stack_hello, sizeof(stack_hello), task_hello, NULL, "hello", 10);

    kernel_start(); //never comes back
    return 0;
}
