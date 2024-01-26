#include <FXRTOS.h>
#include "main.h"
#include <stdbool.h>
#define SET_ROUND_ROBIN_TIMESLICE   0
bool pin_state = 0;
void vTask1Code( void * pvParameters)
{
    fx_thread_sleep(1);
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    	fx_thread_yield();
    }
}

void vTask2Code( void * pvParameters)
{
    fx_thread_sleep(1);
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    	fx_thread_yield();
    }
}
void vTask3Code( void * pvParameters)
{
    fx_thread_sleep(1);
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    	fx_thread_yield();
    }
}
void vTask4Code( void * pvParameters)
{
    fx_thread_sleep(1);
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    	fx_thread_yield();
    }
}
void vTask5Code( void * pvParameters)
{
    fx_thread_sleep(1);
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    	fx_thread_yield();
    }
}
void fx_app_init()
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
    fx_thread_init(
        &thread0, 
        vTask1Code, 
        (void*) 1, 
        5,
        stack0, 
        sizeof(stack0), 
        false
    );
    fx_thread_init(
        &thread1, 
        vTask2Code, 
        (void*) 1, 
        5,
        stack1, 
        sizeof(stack1),  
        false
    );
    fx_thread_init(
        &thread2, 
        vTask3Code, 
        (void*) 1, 
        5,
        stack2, 
        sizeof(stack2), 
        false
    );
    fx_thread_init(
        &thread3, 
        vTask4Code, 
        (void*) 1, 
        5,
        stack3, 
        sizeof(stack3), 
        false
    );
    fx_thread_init(
        &thread4, 
        vTask5Code, 
        (void*) 1, 
        5,
        stack4, 
        sizeof(stack4), 
        false
    );
    
}
