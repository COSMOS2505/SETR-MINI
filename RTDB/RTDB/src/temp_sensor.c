#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>~
#include "temp_sensor.h"
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(temp_sensor, LOG_LEVEL_INF); // Prints para Logging da UART

// configurações do ADC
// 1 - Nó do ADC no Device Tree
#define ADC_NODE  DT_INST(0 , nordic_nrf_saadc)

// 2 - Parametros do sensor de temperatura MCP9700 GND-VOUT-VDD
// VOUT = 500mV + (10mV/°C * T)
#define SENSOR_OFFSET_MV 500   // Offset em mV
#define SENSOR_MV_PER_DEGREE 10 // mV por grau Celsius

//  3 - Configuração do canal ADC
#define ADC_CHANNEL_ID 1
#define ADC_INPUT_PIN  ADC_INPUT_POSITIVE_ANALOG_IN1 // Pino AIN1

// 4 - Resolução do ADC
#define ADC_RESOLUTION 12 // 12 bits
// Levando em consideração que a tensão varia de 0.5V a 1.5V 
// E que a referência interna é 0.6V (600mV), precisamos ajustar a escala
// para mapear corretamente a faixa de tensão do sensor, com um ganho de 1/6
#define ADC_GAIN ADC_GAIN_1_6
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT

// Variaveis estaticas do driver ADC
static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE); // Busca o device ADC
static int16_t sample_buffer[5];                     // Buffer para apenas um amostra


//função de inicialização do ADC
void temp_sensor_init(void){
    if (!device_is_ready(adc_dev)){
        LOG_ERR("O ADC não está disponível ")
        return;
    }

    //configurações do canal ADC
    channel_cfg.channel_id = ADC_CHANNEL_ID;
    channel_cfg
}