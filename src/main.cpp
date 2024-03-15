#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <iostream>
#include <string>

const char* ssid = "Willyan's Galaxy S20 FE";
const char* password = "123456789";

// TEMPERATURA: Sensor de temperatura usando o LM35
 
const int LM35 = 34; // Define o pino que lera a saída do LM35
float temperature; // Variável que armazenará a temperatura medida
float cal = 0.4;
float tempC;
float temp;

// UMIDADE
const int pinoUmidade = 35;
float moisture;
// Pino de entrada para verificar o nível lógico
// const int pin34 = 34;

float measureTemperature() {
  temperature = (float(analogRead(LM35))*3.3/(1023))/0.01;
  tempC = cal*temperature;

  return tempC;
}

float measureMoisture() {
  float umidade = analogRead(pinoUmidade);
  umidade = (umidade/1023);
  umidade = umidade*100.;

  return umidade;
}

void sendRequest(float temperature, float moisture) {
  
  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://ec2-18-220-239-10.us-east-2.compute.amazonaws.com:5900/current-params");
  http.addHeader("Content-Type", "application/json");

  // Crie um novo objeto JSON com nome e idade
  DynamicJsonDocument jsonPayload(1024);
  jsonPayload["Temperature"] = temperature;
  jsonPayload["Moisture"] = moisture;
  jsonPayload["MicroSerialNumber"] = "123456789";
  //delay(30000);

  // Serialize o objeto JSON como uma string
  String jsonStr;
  serializeJson(jsonPayload, jsonStr);

  if (WiFi.status() == WL_CONNECTED) {
    int httpCode = http.POST(jsonStr);

    // Verifique se a solicitação foi bem-sucedida
    if (httpCode > 0) {
      Serial.printf("[HTTP] Response Code: %d\n", httpCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.printf("[HTTP] Bad Request. Error Code: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

void setup() {
  // Inicialize a comunicação serial
  Serial.begin(9600);

  // Conecte-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Configure o pino 34 como entrada
  pinMode(LM35, INPUT);
  pinMode(pinoUmidade, INPUT);
}

void loop() {

while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Perda de conexão!");
    WiFi.begin(ssid, password);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conectado novamente!");
    }
  }

temp = measureTemperature();
moisture = measureMoisture();

sendRequest(temp, moisture);
delay(1000);
}