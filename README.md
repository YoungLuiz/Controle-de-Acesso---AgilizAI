
# AgilizAI - Controle de Acesso (ESP32 + MQTT)

##  Descrição do Projeto

O **AgilizAI - Controle de Acesso** é um protótipo de sistema automatizado ideal para ambientes hospitalares. Ele visa otimizar o fluxo de pessoas e garantir um acesso higiênico e controlado.

Utilizando **ESP32**, **sensor ultrassônico**, **botão físico (senha)**, **servo motor** e **LEDs**, o sistema detecta a aproximação de uma pessoa e valida o acesso, publicando os dados via **MQTT** para integração com sistemas externos.

##  Objetivos

- **Automação do Acesso**: Controle automático de passagem com base em proximidade e senha.
- **Higiene e Sem Toque**: Redução de contato físico com uso de sensores.
- **Monitoramento em Tempo Real**: Publicação de status via MQTT.
- **Demonstração de IoT**: Aplicação prática de ESP32 + sensores + MQTT.

##  Funcionalidades

- **Detecção de Proximidade** via sensor ultrassônico.
- **Controle de Portão** com servo motor.
- **Indicação Visual** com LEDs (verde/vermelho).
- **Validação de Senha** por botão físico.
- **Comunicação MQTT** com envio de mensagens em JSON.
- **Monitoramento Serial** para debug em tempo real.

##  Requisitos do Sistema

### Hardware

- ESP32 Dev Board (ESP32-WROOM-32)
- Sensor Ultrassônico HC-SR04
- Micro Servo Motor (SG90)
- 1x LED Verde
- 1x LED Vermelho
- 2x Resistores 220 Ohm
- 1x Botão Push-Button
- Protoboard + fios
- Fonte 5V (via USB ou externa)

### Software

- Arduino IDE
- Bibliotecas: `WiFi.h`, `PubSubClient.h`, `ArduinoJson.h`, `ESP32Servo.h`
- Broker MQTT (ex: HiveMQ)
- (Opcional) Cliente MQTT (ex: MQTT Explorer)

##  Dependências

| Biblioteca      | Descrição                                   |
|----------------|----------------------------------------------|
| WiFi.h          | Conexão do ESP32 com Wi-Fi                   |
| PubSubClient.h  | Comunicação com broker MQTT                 |
| ArduinoJson.h   | Serialização de mensagens JSON              |
| ESP32Servo.h    | Controle de servo motor com ESP32           |

## Instruções de Uso

###  Instalar Bibliotecas

No Arduino IDE:  
`Sketch > Incluir Biblioteca > Gerenciar Bibliotecas...`

Instale:

- PubSubClient (Nick O'Leary)
- ArduinoJson (Benoit Blanchon)
- ESP32Servo (John Boxall)

###  Conexões ESP32

#### Sensor Ultrassônico HC-SR04
```
VCC  → 5V (ou 3.3V)
GND  → GND
Trig → D13
Echo → D12
```

#### Servo Motor
```
VCC   → 5V
GND   → GND
Sinal → D5
```

#### LED Verde
```
Anodo (+) → Resistor 220Ω → D27
Catodo (-) → GND
```

#### LED Vermelho
```
Anodo (+) → Resistor 220Ω → D22
Catodo (-) → GND
```

#### Botão (Push-Button)
```
Um lado  → D2
Outro lado → GND
```
> **Nota**: INPUT_PULLUP interno é usado no pino D2.

###  Configurar Código

```cpp
const char* ssid = "SEU_SSID_AQUI";
const char* password = "SUA_SENHA_AQUI";
```

Broker MQTT padrão:
```cpp
broker.hivemq.com, porta 1883
```

Se estiver usando o **Wokwi**, use:
```cpp
const char* ssid = "Wokwi-GUEST";
const char* password = "";
```

Selecione a placa: `ESP32 Dev Module`  
Selecione a porta COM correta  
Faça o upload do código

###  Testar o Sistema

- Abra o **Monitor Serial** (baud rate: `115200`)
- Verifique conexão Wi-Fi e MQTT
- Aproxime a mão do sensor (ex: < 1 cm)
- Pressione o botão

####  Acesso Liberado:
- Sensor detecta proximidade **E** botão pressionado
- Servo abre portão (90°)
- LED verde acende

####  Acesso Negado:
- Sensor detecta, **mas** botão **não** está pressionado
- Servo não se move
- LED vermelho acende

####  MQTT
- Dados enviados a cada 3s para o tópico:  
  `agilizai/controle_acesso/status`

- Payload (exemplo):
```json
{
  "distancia_cm": 0.94,
  "status": "LIBERADA",
  "mensagem": "Acesso permitido"
}
```

## Diagrama do Sistema

> ![image](https://github.com/user-attachments/assets/852abc0a-e6c6-459f-8101-3d0e0f1c8856)


## Dashboard e Fluxo MQTT

- Use ferramentas como **TagoIO**, **Node-RED** ou **MQTT Explorer** para visualizar os dados MQTT.
- Possível estrutura no dashboard:
  - Distância do paciente
  - Status da passagem
  - Mensagem de erro

## Captura de tela e Link Wokwi
![image](https://github.com/user-attachments/assets/0a93b95a-6432-41f8-8525-5bc9bdeb4fbc)

[🔗 Simulação no Wokwi](https://wokwi.com/projects/432870828762816513)

## Dashboard NODE-RED
![image](https://github.com/user-attachments/assets/2703e605-9a42-4450-9bd4-bd11f5f0f8ea)


## Vídeo do Projeto

[📺 Assista ao vídeo](https://youtu.be/bRXgr2MLq4M)

## Integrantes

- Arthur Gomes - RM 560771
- Luiz Silva - RM 560110
- Matheus Siroma - RM 560248
- Pedro Estevam - RM 560642
