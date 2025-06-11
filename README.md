# Sistema de Controle de Acesso AgilizAI com ESP32 e MQTT

Este projeto implementa um protótipo de sistema de controle de acesso utilizando um microcontrolador ESP32, um sensor ultrassônico, um servo motor, LEDs e um botão de pressão. O sistema monitora a distância de um objeto (simulando um "paciente"), verifica uma "senha" (simulada por um pressionamento de botão) e controla uma barreira (servo motor) de acordo. Ele também publica o status de acesso e os dados do sensor em tempo real para um broker MQTT.

## Tabela de Conteúdo

* [Visão Geral](#visão-geral)
* [Funcionalidades](#funcionalidades)
* [Componentes de Hardware](#componentes-de-hardware)
* [Configuração do Software](#configuração-do-software)
* [Diagrama do Circuito](#diagrama-do-circuito)
* [Como Funciona](#como-funciona)
* [Comunicação MQTT](#comunicação-mqtt)
* [Configuração](#configuração)
* [Bibliotecas Utilizadas](#bibliotecas-utilizadas)

## Visão Geral

O projeto "AgilizAI_ControleAcesso_ESP32" visa criar um sistema de controle de acesso inteligente que pode ser adaptado para diversas aplicações, como controle de entrada em ambientes restritos, validação de presença ou até mesmo sistemas de segurança simples. A integração com MQTT permite monitoramento e controle remoto, tornando-o ideal para soluções IoT (Internet das Coisas).

## Funcionalidades

* **Detecção de Proximidade:** Utiliza um sensor ultrassônico (HC-SR04) para medir a distância de um objeto, simulando a presença de um "paciente" ou pessoa.
* **Validação de "Senha":** Um botão de pressão simula a entrada de uma senha. O acesso é liberado apenas se o objeto estiver próximo e a "senha" for "correta".
* **Controle de Barreira:** Um servo motor (SG90) atua como uma barreira, abrindo ou fechando o acesso com base nas condições de proximidade e senha.
* **Indicadores Visuais:** LEDs verde e vermelho fornecem feedback visual instantâneo sobre o status do acesso (liberado/bloqueado).
* **Conectividade Wi-Fi:** O ESP32 se conecta a uma rede Wi-Fi local para comunicação.
* **Publicação MQTT:** Envia dados em tempo real sobre a distância detectada, status da barreira (aberta/fechada), e o status geral da entrada (liberada/bloqueada, com ou sem erro) para um broker MQTT.

## Componentes de Hardware

Os seguintes componentes de hardware são essenciais para este projeto:

* **Placa de Desenvolvimento ESP32:** O cérebro do sistema, responsável por processar os dados e controlar os periféricos.
* **Sensor Ultrassônico HC-SR04:** Para medição de distância.
* **Servo Motor SG90:** Para atuar como barreira de acesso.
* **LED Verde:** Indicador de acesso liberado.
* **LED Vermelho:** Indicador de acesso bloqueado.
* **Botão de Pressão:** Simula a "senha" de acesso.
* **Resistores:** 2 resistores de 220 Ohm ou 330 Ohm para os LEDs (para limitar a corrente).
* **Protoboard e Fios Jumper:** Para montagem do circuito.

## Configuração do Software

Para compilar e carregar o código para o seu ESP32, você precisará:

* **Arduino IDE:** Recomenda-se a versão mais recente.
* **Pacote de Placas ESP32:** Instale o pacote de placas para ESP32 via `Ferramentas > Placa > Gerenciador de Placas...`.
* **Bibliotecas Necessárias:**
    * `PubSubClient` by Nick O'Leary
    * `ArduinoJson` by Benoit Blanchon
    * `ESP32Servo` by John K. Bennett

Você pode instalar essas bibliotecas através do Gerenciador de Bibliotecas da Arduino IDE (`Sketch > Incluir Biblioteca > Gerenciar Bibliotecas...`).

## Diagrama do Circuito

A imagem abaixo ilustra as conexões dos componentes com o ESP32:

![Diagrama de Circuito do Projeto de Controle de Acesso ESP32](image_8f4314.png)

**Conexões Detalhadas:**

* **Sensor Ultrassônico (HC-SR04):**
    * `VCC` ao `5V` ou `3.3V` do ESP32.
    * `GND` ao `GND` do ESP32.
    * `TRIG` ao GPIO `13` do ESP32.
    * `ECHO` ao GPIO `12` do ESP32.
* **Servo Motor (SG90):**
    * `VCC` (fio vermelho) ao `5V` do ESP32.
    * `GND` (fio marrom/preto) ao `GND` do ESP32.
    * `Sinal` (fio laranja/amarelo) ao GPIO `5` do ESP32.
* **LED Verde:**
    * `Anodo` (perna mais longa) via resistor (ex: 220 Ohm) ao GPIO `27` do ESP32.
    * `Catodo` (perna mais curta) ao `GND` do ESP32.
* **LED Vermelho:**
    * `Anodo` (perna mais longa) via resistor (ex: 220 Ohm) ao GPIO `22` do ESP32.
    * `Catodo` (perna mais curta) ao `GND` do ESP32.
* **Botão de Pressão:**
    * Um terminal ao GPIO `2` do ESP32.
    * O outro terminal ao `GND` do ESP32. (O código utiliza o pull-up interno do ESP32, então não é necessário um resistor externo de pull-up para o botão, mas um divisor de tensão ou um resistor de pull-down externo para maior robustez pode ser considerado em aplicações reais).

## Como Funciona

1.  **Inicialização (`setup()`):**
    * Configura os pinos do ESP32 como entrada ou saída.
    * Anexa o servo motor ao seu pino e o posiciona no estado "fechado" (0 graus).
    * Realiza um "blink" inicial nos LEDs para indicar o início.
    * Conecta o ESP32 à rede Wi-Fi configurada.
    * Estabelece a conexão com o broker MQTT.
2.  **Loop Principal (`loop()`):**
    * **Manutenção da Conexão:** Garante que a conexão com o broker MQTT esteja sempre ativa.
    * **Medição de Distância:** O sensor ultrassônico mede continuamente a distância do objeto à frente. A distância é limitada ao `DISTANCIA_LIMITE_PACIENTE` (200 cm) para evitar leituras irrelevantes.
    * **Simulação de Senha:** O estado do botão de pressão é lido. Um pressionamento do botão simula uma "senha correta".
    * **Lógica de Acesso:**
        * Se o objeto estiver **próximo** (`distanciaPacienteCm < DISTANCIA_LIMITE_PACIENTE`) **E** a "senha" for **correta**, a barreira é **ABERTA** (servo em 90 graus), o LED verde acende e o LED vermelho apaga.
        * Caso contrário, a barreira permanece **FECHADA** (servo em 0 graus), o LED verde apaga e o LED vermelho acende. Mensagens detalhadas são impressas no Serial Monitor, explicando o motivo do bloqueio (ex: "Paciente próximo, mas senha INCORRETA.").
    * **Publicação MQTT:** A cada `sendInterval` (3000 ms), o sistema publica um objeto JSON para o tópico MQTT contendo o status atual do sistema.

## Comunicação MQTT

O projeto utiliza o protocolo MQTT para comunicação com um broker externo.

* **Broker:** `broker.hivemq.com`
* **Porta:** `1883`
* **ID do Cliente:** `AgilizAI_ControleAcesso_ESP32` (deve ser único para cada dispositivo conectado ao broker)
* **Tópico de Publicação:** `agilizai/controle_acesso/status`

O formato do payload JSON publicado no tópico `agilizai/controle_acesso/status` é o seguinte:

```json
{
  "distancia_paciente_cm": 150,  // Distância atual em cm
  "passagem_aberta": false,     // true se a passagem está aberta, false se está fechada
  "status_entrada": "BLOQUEADA",// Pode ser "LIBERADA" ou "BLOQUEADA"
  "erro_entrada": "NENHUM"      // Pode ser "NENHUM" ou "SENHA_INCORRETA"
}
