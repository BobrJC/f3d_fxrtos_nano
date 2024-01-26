#include <FXRTOS.h>

extern void toggle_blue_led(void);
extern void toggle_red_led(void);

void
task0(void* args)
{
    fx_thread_sleep(1);
    for (;;)
    {
        toggle_blue_led();
        fx_thread_yield();
    }
}

void
task1(void* args)
{
    fx_thread_sleep(1);
    for (;;)
    {
        toggle_red_led();
        fx_thread_yield();
    }
}


void
task2(void* args)
{
    fx_thread_sleep(1);

    for (;;)
    {
        toggle_5_led();
        fx_thread_yield();
    }
}

task3(void* args)
{
    fx_thread_sleep(1);
    for (;;)
    {
        toggle_6_led();
        fx_thread_yield();
        //fx_thread_sleep(300);
    }
}

task4(void* args)
{

    for (;;)
    {
        toggle_7_led();
        fx_thread_yield();
        //fx_thread_sleep(300);
    }
}

void
fx_app_init(void)
{
    static fx_thread_t thread0;
    static fx_thread_t thread1;
    static fx_thread_t thread2;
    static fx_thread_t thread3;
    static fx_thread_t thread4;
    static uint32_t stack0[0x200 / sizeof(uint32_t)];
    static uint32_t stack1[0x200 / sizeof(uint32_t)];
    static uint32_t stack2[0x200 / sizeof(uint32_t)];
    static uint32_t stack3[0x200 / sizeof(uint32_t)];
    static uint32_t stack4[0x200 / sizeof(uint32_t)];

    fx_thread_init(&thread0, task0, NULL, 5, stack0, sizeof(stack0), false);
    fx_thread_init(&thread1, task1, NULL, 5, stack1, sizeof(stack1), false);
    fx_thread_init(&thread2, task2, NULL, 5, stack2, sizeof(stack2), false);
    fx_thread_init(&thread3, task3, NULL, 5, stack3, sizeof(stack3), false);
    fx_thread_init(&thread4, task4, NULL, 5, stack4, sizeof(stack4), false);
}

