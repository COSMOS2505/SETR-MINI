/* src/main.c */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"
#include "ui.h"

/* --- CONFIGURAÇÃO DA THREAD DE UI --- */
#define UI_STACK_SIZE 1024
#define UI_PRIORITY 4 // Prioridade média-baixa (Display/LEDs)

/* Definir a memória (Stack) para a thread da UI */
K_THREAD_STACK_DEFINE(ui_stack_area, UI_STACK_SIZE);
struct k_thread ui_thread_data;


int main(void)
{
    printk("--- INICIO DO TESTE DE UI (LEDS + BOTOES) ---\n");

    /* 1. Inicializar a Base de Dados (RTDB) */
    /* Isto é obrigatório antes de qualquer outra coisa [cite: 76] */
    rtdb_init();

    /* TRUQUE DE TESTE: 
     * Como não temos sensor, fingimos que a sala está a 25.0 graus.
     * O Alvo inicial no rtdb.c também é 25.0.
     * Logo, ao ligar, deve acender o LED 2 (Normal).
     */
    rtdb_set_current_temp(25.0);

    /* 2. Inicializar o Hardware de Interface (GPIOs/Ints) */
    ui_init();

    /* 3. Arrancar a Thread que controla os LEDs */
    /* Esta thread vai correr em paralelo para sempre */
    k_thread_create(&ui_thread_data, ui_stack_area,
                    K_THREAD_STACK_SIZEOF(ui_stack_area),
                    (k_thread_entry_t)ui_led_thread_entry,
                    NULL, NULL, NULL,
                    UI_PRIORITY, 0, K_NO_WAIT);

    printk("Sistema pronto! Use os botoes para testar.\n");

    /* 4. Loop de Monitorização (Opcional, só para veres no PC) */
    while (1) {
        bool sistema_on = rtdb_get_system_on();
        double alvo = rtdb_get_target_temp();
        double atual = rtdb_get_current_temp(); // Vai ser sempre 25.0 neste teste

        /* Imprime estado a cada segundo para confirmares o que os LEDs mostram */
        printk("[MAIN] Sys: %s | Atual: %.1f | Alvo: %.1f | Diff: %.1f\n",
               sistema_on ? "ON " : "OFF",
               atual,
               alvo,
               atual - alvo);

        k_msleep(1000);
    }
    
    return 0;
}