/*
  INSTITUTO FEDERAL DO RIO GRANDE DO NORTE - CAMPUS ZL
  CURSO DE FORMAÇÃO INICIAL E CONTINUADA EM SISTEMAS EMBARCADOS

  DESCRIÇÃO: Projeto de criação de um semáforo para deficientes visuais com estímulo
  sonoro. O código possui a funcionalidade de um semáforo comum, no entanto incrementei
  uma função de interrupção na placa Raspberry Pi Pico W. A função de interrupção que
  implementei garante que independentemente do ponto do semáforo que está acontecendo,
  caso o botão seja acionado, o sistema automaticamente prioriza o pedestre. Ao longo 
  do código, todos esses trechos estão comentados.

  AUTOR: Lucas Tomaz de Moura

*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

//definição dos pinos de saída dos LEDS.
#define LED_VERDE 11
#define LED_AMARELO 12
#define LED_VERMELHO 14
#define PEDESTRE 15

//configuração dos pinos do botão e o buzzer
#define BOTAO_PIN 10
#define BUZZER_PIN 13

//definição da frequência do buzzer.
#define BUZZER_FREQUENCY 1500

//variável volátil usada oara detectar se houve ou não
// o pressionamento do botão dos pedestres.
volatile bool pedestre_acionado = false;

//função de interrupção para determinar se o botão foi acionado, indicando que tem pedestre.
void Pedestre(){
  pedestre_acionado = true;
}

//função para funcionamento do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

int main() {
    stdio_init_all();

    // Inicializa os pinos dos LEDs do semáforo, dos pedestres e do botão
    gpio_init(LED_VERDE);
    gpio_init(LED_AMARELO);
    gpio_init(LED_VERMELHO);
    gpio_init(BOTAO_PIN);
    pwm_init_buzzer(BUZZER_PIN);
    gpio_init(PEDESTRE);

    // Configura todos LEDs como saída
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(PEDESTRE, GPIO_OUT);

    // Configura o botão como entrada e ativa o pull-up
    gpio_set_dir(BOTAO_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_PIN);

    //função para configuração da porta PWm para o buzzer
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // função de interrupção acionada quando o botão dos pedestres é acionado.
    gpio_set_irq_enabled_with_callback(BOTAO_PIN, GPIO_IRQ_EDGE_FALL, true, Pedestre);

    while (true) {
        // Verifica se o botão está pressionado
        if (pedestre_acionado) {
          //led amarelo aceso por 5 s.
            gpio_put(PEDESTRE, 0); 
            gpio_put(LED_VERDE, 0);    
            gpio_put(LED_AMARELO, 1); 
            gpio_put(LED_VERMELHO, 0);
            sleep_ms(5000);
          //led vermelho aceso por 15 s
            gpio_put(PEDESTRE, 1); 
            gpio_put(LED_VERDE, 0);    
            gpio_put(LED_AMARELO, 0); 
            gpio_put(LED_VERMELHO, 1);
            pwm_set_gpio_level(BUZZER_PIN, 1024);
            sleep_ms(15000);
          //led vermelho e dos pedestres apagado s
            pwm_set_gpio_level(BUZZER_PIN, 0);
            gpio_put(PEDESTRE, 0);  
            gpio_put(LED_VERMELHO, 0);
          //mudança de valor da variável após executar os comandos de acionamento do botão
            pedestre_acionado = false;

        } else {
          for(uint i = 0; i<8; i++){
            //função que verifica se o botão foi pressionado
            if(pedestre_acionado) break;
            gpio_put(PEDESTRE, 0); 
            gpio_put(LED_VERDE, 1);    
            gpio_put(LED_AMARELO, 0); 
            gpio_put(LED_VERMELHO, 0);
            sleep_ms(1000);
          }
          for(uint j = 0; j<2; j++){
            //função que verifica se o botão foi pressionado
            if(pedestre_acionado) break;
            gpio_put(PEDESTRE, 0); 
            gpio_put(LED_VERDE, 0);    
            gpio_put(LED_AMARELO, 1); 
            gpio_put(LED_VERMELHO, 0);
            sleep_ms(1000);
          }
          for(uint k = 0; k<10; k++){
            //função que verifica se o botão foi pressionado
            if(pedestre_acionado) break;
            gpio_put(PEDESTRE, 0); 
            gpio_put(LED_VERDE, 0);    
            gpio_put(LED_AMARELO, 0); 
            gpio_put(LED_VERMELHO, 1);
            sleep_ms(1000);
          }
        }
    }
}
