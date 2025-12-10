#include "rtdb.h"

// Variáveis privadas do RTDB
static bool system_on = false;
static double current_temp = 22.0;
static double target_temp = 25.0;

// Criação de mutex para proteção das variáveis por método estático
// Havia também a forma de criação dinêmica onde é declarado e depois inicializado.
K_MUTEX_DEFINE(rtdb_mutex);

void rtdb_init(void)
{
    // Inicialização DO mutex (já feito pela macro K_MUTEX_DEFINE) com memória reservada estaticamente 
}

void rtdb_set_system_on(bool on)        // Função para ligar ou desligar o sistema com base na bool on
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    system_on = on;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_toggle_system_on(void)        // Função para alternar o estado do sistema (toggle)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    system_on = !system_on;
    k_mutex_unlock(&rtdb_mutex);
}
void rtdb_increment_target_temp(void)   // Função para incrementar a temperatura alvo em 1 grau
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    target_temp += 1.0;
    k_mutex_unlock(&rtdb_mutex);
}
void rtdb_decrement_target_temp(void)   // Função para decrementar a temperatura alvo em 1 grau
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    target_temp -= 1.0;
    k_mutex_unlock(&rtdb_mutex);
}
void rtdb_set_current_temp(double temp) // Função para atualizar a temperatura atual
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    current_temp = temp;
    k_mutex_unlock(&rtdb_mutex);
}
// Funções GET seguras

bool rtdb_get_system_on(void)       // Função para obter o estado atual do sistema (ligado/desligado)
{
    bool on;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    on = system_on;
    k_mutex_unlock(&rtdb_mutex);
    return on;
}
double rtdb_get_target_temp(void)   // Função para obter a temperatura alvo atual
{
    double temp;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    temp = target_temp;
    k_mutex_unlock(&rtdb_mutex);
    return temp;
}
double rtdb_get_current_temp(void)  // Função para obter a temperatura atual
{
    double temp;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    temp = current_temp;
    k_mutex_unlock(&rtdb_mutex);
    return temp;
}

