#include <FXRTOS.h>
#include "main.h"
#include <stdbool.h>
const unsigned gpio_intr = EXTI15_10_IRQn;
static fx_dpc_t gpio_dpc;

void init_gpio()
{
    GPIO_InitTypeDef init_struct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    init_struct.Mode = GPIO_MODE_IT_FALLING;
    init_struct.Pull = GPIO_NOPULL;
    init_struct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &init_struct);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void
fx_intr_handler(void)
{
    //
    // Check whether the vector is ours
    //
    if (hal_intr_get_current_vect() == EXTI15_10_IRQn)
    {
        timer_restart();
        fx_dpc_request(&gpio_dpc, gpio_dpc_handler, (void*)0x0);
    }
}
void ISR_task(void* p)
{
    while (1)
    {
        
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }    
}
void vTask1Code(void)
{
    for (int i = 0; i < 2; i++)
    {
        i--;
    }
}

void fx_app_init()
{
    static fx_thread_t thread0;
    static uint32_t stack0[0x200 / sizeof(uint32_t)];
     fx_thread_init(
        &thread0, 
        vTask1Code, 
        (void*) 1, 
        5,
        stack0, 
        sizeof(stack0), 
        false
    );
}