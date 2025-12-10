#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "UI.h"
#include "rtdb.h"

//modelo de tempo real para o thread dos LEDs ( semaforos )
K_SEM_DEFINE(ui_sem, 0, 1);


// Definições dos botões (botões físicos do kit)
#define BTN0_NODE DT_ALIAS(sw0)
#define BTN1_NODE DT_ALIAS(sw1)
#define BTN3_NODE DT_ALIAS(sw3)

// Definições dos LEDs
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

// Estruturas dos GPIOs para botões e LEDs
static const struct gpio_dt_spec btn0 = GPIO_DT_SPEC_GET_OR(BTN0_NODE, gpios, {0});
static const struct gpio_dt_spec btn1 = GPIO_DT_SPEC_GET_OR(BTN1_NODE, gpios, {0});
static const struct gpio_dt_spec btn3 = GPIO_DT_SPEC_GET_OR(BTN3_NODE, gpios, {0});

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0});
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET_OR(LED2_NODE, gpios, {0});
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET_OR(LED3_NODE, gpios, {0});

// Callbacks dos botões ( interrupções )
static struct gpio_callback btn0_cb_data;
static struct gpio_callback btn1_cb_data;
static struct gpio_callback btn3_cb_data;

// Timer para acordar o thread dos LEDs periodicamente
void ui_timer_handler(struct k_timer *dummy)
{
    k_sem_give(&ui_sem);
}

K_TIMER_DEFINE(ui_timer, ui_timer_handler, NULL);

// BOTÃO INTERRUPTIONS HANDLERS

void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // Botão 0: ON/OFF
    rtdb_toggle_system_on();
}
void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // Botão 1: Aumentar temperatura alvo
    if(!rtdb_get_system_on()){
        // Se o sistema estiver OFF, não faz nada
        return;
    }
    rtdb_increment_target_temp();
}
void button3_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    // Botão 3: Diminuir temperatura alvo~
    if(!rtdb_get_system_on()){
        // Se o sistema estiver OFF, não faz nada
        return;
    }
    rtdb_decrement_target_temp();
}

// Funções de inicialização e thread dos LEDs

void ui_init(void)
{
    
    if (!gpio_is_ready_dt(&btn0) || !gpio_is_ready_dt(&btn1) || !gpio_is_ready_dt(&btn3) ||
        !gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1) || !gpio_is_ready_dt(&led2) || !gpio_is_ready_dt(&led3)) {
        printk("Erro: GPIO device not ready.\n");
        return;
    }

    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE); 
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);


    // -----------------------------------------------------------------------
    // Botao 1    
    if(gpio_pin_configure_dt(&btn0, GPIO_INPUT | GPIO_PULL_UP) < 0) {
        printk("Erro: Configuração do Botão 0 falhou.\n");
        return;
    }
    gpio_pin_interrupt_configure_dt(&btn0, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&btn0_cb_data, button0_pressed, BIT(btn0.pin));
    gpio_add_callback(btn0.port, &btn0_cb_data);
    
    // -----------------------------------------------------------------------
    // Botao 2
    if(gpio_pin_configure_dt(&btn1, GPIO_INPUT | GPIO_PULL_UP) < 0) {
        printk("Erro: Configuração do Botão 1 falhou.\n");
        return;
    }
    gpio_pin_interrupt_configure_dt(&btn1, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&btn1_cb_data, button1_pressed, BIT(btn1.pin));
    gpio_add_callback(btn1.port, &btn1_cb_data);

    // -----------------------------------------------------------------------
    // Botao 4
    if(gpio_pin_configure_dt(&btn3, GPIO_INPUT | GPIO_PULL_UP) < 0) {
    printk("Erro: Configuração do Botão 3 falhou.\n");
    return;
    }
    gpio_pin_interrupt_configure_dt(&btn3, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&btn3_cb_data, button3_pressed, BIT(btn3.pin));
    gpio_add_callback(btn3.port, &btn3_cb_data);

}

// tAREFA periodica para atualizar os LEDs baseado o estado do sistema
void ui_led_thread_entry(void *p1, void *p2, void *p3)
{
   k_timer_start(&ui_timer, K_MSEC(100), K_MSEC(100)); // 100ms period

   while (1)
   {
    k_sem_take(&ui_sem, K_FOREVER);
    // printk("UI Thread woke up to update LEDs.\n");

    // Ler valores da RTDB
    double target_temp = rtdb_get_target_temp();
    double current_temp = rtdb_get_current_temp();
    double temp_diff = current_temp - target_temp;
    bool system_on = rtdb_get_system_on();

    // Saída nos Leds
    // Led 1
    gpio_pin_set_dt(&led0, system_on ? 1 : 0);
    if(!system_on) {
        // Sistema desligado, apagar todos os LEDs
        gpio_pin_set_dt(&led1, 0);
        gpio_pin_set_dt(&led2, 0);
        gpio_pin_set_dt(&led3, 0);
        continue;
    }
    else {
        // Sistema ligado, atualizar LEDs com base na diferença de temperatura
        if (temp_diff < -5.0) {
            // Muito frio
            gpio_pin_set_dt(&led1, 0);
            gpio_pin_set_dt(&led2, 1);
            gpio_pin_set_dt(&led3, 0);
        } else if (temp_diff > 5.0) {
            // Muito quente
            gpio_pin_set_dt(&led1, 0);
            gpio_pin_set_dt(&led2, 0);
            gpio_pin_set_dt(&led3, 1);
        } else {
            // Normal
            gpio_pin_set_dt(&led1, 1);
            gpio_pin_set_dt(&led2, 0);
            gpio_pin_set_dt(&led3, 0);
        }
    }
   }  
}