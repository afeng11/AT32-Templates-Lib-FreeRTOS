#include "main.h"

void led_init()
{
    gpio_init_type gpio_init_struct;
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_1;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_2;
    gpio_init(GPIOA, &gpio_init_struct);
}

void at32_led_toggle(gpio_type *gpio_x, uint16_t pins)
{
        gpio_x->odt ^= pins;
}

void led2_task_function(void *pvParameters)
{
    while (1)
    {
        at32_led_toggle(GPIOA, GPIO_PINS_1);
        vTaskDelay(250);
    }
}

/* led3 task function */
void led3_task_function(void *pvParameters)
{
    while (1)
    {
        at32_led_toggle(GPIOA, GPIO_PINS_2);
        vTaskDelay(125);
    }
}

TaskHandle_t led2_handler;
TaskHandle_t led3_handler;
int main()
{
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

    system_clock_config();

    delay_init();

    led_init();


    {
        taskENTER_CRITICAL();

        xTaskCreate((TaskFunction_t)led2_task_function,
                    (const char *)"LED2_task",
                    (uint16_t)512,
                    (void *)NULL,
                    (UBaseType_t)2,
                    (TaskHandle_t *)&led2_handler);
        xTaskCreate((TaskFunction_t)led3_task_function,
                    (const char *)"LED3_task",
                    (uint16_t)512,
                    (void *)NULL,
                    (UBaseType_t)2,
                    (TaskHandle_t *)&led3_handler);

        taskEXIT_CRITICAL();

        vTaskStartScheduler();
    }

    while (1)
    {
        gpio_bits_reset(GPIOA, GPIO_PINS_1);
        gpio_bits_set(GPIOA, GPIO_PINS_2);
        delay_ms(500);
        gpio_bits_set(GPIOA, GPIO_PINS_1);
        gpio_bits_reset(GPIOA, GPIO_PINS_2);
        delay_ms(500);
    }
}