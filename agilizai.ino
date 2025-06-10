#include <WiFi.h>          
#include <PubSubClient.h>    
#include <ArduinoJson.h>     
#include <ESP32Servo.h>      


#define TRIGGER_PIN 13 
#define ECHO_PIN 12 
#define DISTANCIA_LIMITE_PACIENTE 200 /

#define SERVO_PIN 5      
#define LED_VERDE 27    
#define LED_VERMELHO 22    
#define BOTAO_SENHA 2    

// Config WI-FI
const char* ssid = "Wokwi-GUEST"; 
const char* password = "";        


// Config MQTT
const char* mqtt_broker = "broker.hivemq.com"; 
const int mqtt_port = 1883;
const char* mqtt_client_id = "AgilizAI_ControleAcesso_ESP32"; 
const char* mqtt_topic_publish = "agilizai/controle_acesso/status"; 

WiFiClient espClient;
PubSubClient client(espClient);

Servo meuServo;     
long duracao;       
int distanciaPacienteCm;
bool senhaCorreta = false;

unsigned long lastSendTime = 0;
const long sendInterval = 3000;

// Funçao para conexao WI-FI
void setup_wifi() {
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Funçao para reconetar MQTT
void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT Broker...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// Funçao para medir distancia ULTRASSONICO 
long measureDistanceCm() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distanceCm = duration * 0.034 / 2; 

  if (distanceCm == 0 || distanceCm > DISTANCIA_LIMITE_PACIENTE) {
    return DISTANCIA_LIMITE_PACIENTE;
  }
  return distanceCm;
}


// Funçao para enviar dados via MQTT
void sendAccessStatusViaMQTT(int distancia, bool aberta, const String& status, const String& erro) {
  if (!client.connected()) {
    reconnect_mqtt();
  }

  StaticJsonDocument<256> doc;
  doc["distancia_paciente_cm"] = distancia;
  doc["passagem_aberta"] = aberta;
  doc["status_entrada"] = status;
  doc["erro_entrada"] = erro;

  String jsonString;
  serializeJson(doc, jsonString);

  Serial.print("Publicando MQTT no topico '");
  Serial.print(mqtt_topic_publish);
  Serial.print("': ");
  Serial.println(jsonString);

  if (client.publish(mqtt_topic_publish, jsonString.c_str())) {
    Serial.println("Dados de acesso publicados com sucesso via MQTT.");
  } else {
    Serial.println("Falha ao publicar dados de acesso via MQTT.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("--- Sistema de Controle de Acesso AgilizAI (MQTT) ---");

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT); 
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BOTAO_SENHA, INPUT_PULLUP);

  meuServo.attach(SERVO_PIN);
  meuServo.write(0);

  // Blink inicial dos LEDs para indicar inicializaçao
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, HIGH);
  delay(1000);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  delay(1000);

  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  reconnect_mqtt();
}


void loop() {
  // Garante que o cliente MQTT esteja sempre conectado
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); 
  distanciaPacienteCm = measureDistanceCm(); 

  // Simula a validação da senha (acionamento do botão)
  if (digitalRead(BOTAO_SENHA) == LOW) { // Se o botão for pressionado (simulando senha correta)
    if (!senhaCorreta) { // Detecta apenas a borda de descida para evitar repetição
      senhaCorreta = true;
      Serial.println("Senha CORRETA detectada! (Botão pressionado)");
    }
  } else {
    senhaCorreta = false; 
  }

  String currentStatus = "BLOQUEADA";
  String currentError = "NENHUM";
  bool passagemAberta = false;

  if (distanciaPacienteCm < DISTANCIA_LIMITE_PACIENTE && senhaCorreta) {
    // Abrir Passagem
    meuServo.write(90); // Abre o servo (ajuste o ângulo conforme a necessidade)
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println("PASSAGEM ABERTA: Paciente correto.");
    currentStatus = "LIBERADA";
    passagemAberta = true;
  } else {
    // Manter Fechado
    meuServo.write(0); // Fecha o servo
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    if (distanciaPacienteCm < DISTANCIA_LIMITE_PACIENTE && !senhaCorreta) {
        Serial.println("PASSAGEM FECHADA: Paciente próximo, mas senha INCORRETA.");
        currentError = "SENHA_INCORRETA";
    } else {
        Serial.println("PASSAGEM FECHADA: Aguardando paciente ou senha.");
    }
  }

  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    sendAccessStatusViaMQTT(distanciaPacienteCm, passagemAberta, currentStatus, currentError);
  }

  Serial.println("----------------------------------------------------------");
  delay(50); 
}
