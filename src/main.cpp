#include <Arduino.h>
#include <DHT.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 4      // Pino digital conectado ao sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302). Para DHT11, use DHT11

DHT dht(DHTPIN, DHTTYPE);

const int BUFFER_SIZE = 60; // Último 1 minuto (2s por leitura)
float humidityBuf[BUFFER_SIZE] = {0};
float tempBuf[BUFFER_SIZE] = {0};
int bufIndex = 0;
int sampleCount = 0;
unsigned long lastPrintTime = 0; // Controle para impressão a cada 1 minuto

// Função para ler os dados do sensor
void readSensorData(float &humidity, float &temperature)
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void printMediaTimestamp()
{
  struct tm timeinfo;
  if (getLocalTime(&timeinfo))
  {
    char timeBuffer[32];
    // Formata a string para: Media (HH:MM -- dd/mm)
    strftime(timeBuffer, sizeof(timeBuffer), "Media (%H:%M -- %d/%m)", &timeinfo);
    Serial.println(timeBuffer);
  }
  else
  {
    Serial.println("Media (Hora não definida)");
  }
}

void sendData(float avgH, float avgT)
{
  HTTPClient http;
  // URL da sua API (substitua pela URL real)
  http.begin("http://192.168.15.150:3000/api");
  http.addHeader("Content-Type", "application/json");

  // Monta o payload com os dados (você pode incluir o timestamp se desejar)
  char jsonPayload[128];
  sprintf(jsonPayload, "{\"humidity\":%.2f, \"temperature\":%.2f}", avgH, avgT);

  int httpResponseCode = http.POST(jsonPayload);
  if (httpResponseCode > 0)
  {
    Serial.print("Código de resposta HTTP: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println("Resposta: " + response);
  }
  else
  {
    Serial.print("Erro na requisição, código: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void setup()
{
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Configura o Wi-Fi
  WiFi.begin("VIVO43B-2G", "17182031");
  Serial.print("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado ao Wi-Fi!");

  // Configura o NTP para obter a hora; ajuste os offsets conforme sua região se necessário.
  configTime(-3 * 3600, 0, "pool.ntp.org");

  // Espera até a hora ser definida
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Aguardando sincronização do horário...");
    delay(1000);
  }

  Serial.println("Hora sincronizada.");
  // Inicia o sensor DHT
  dht.begin();

  delay(1000);
}

void loop()
{
  float humidity, temperature;
  readSensorData(humidity, temperature);

  // Se leitura válida, atualiza os buffers
  if (!isnan(humidity) && !isnan(temperature))
  {
    humidityBuf[bufIndex] = humidity;
    tempBuf[bufIndex] = temperature;
    bufIndex = (bufIndex + 1) % BUFFER_SIZE;
    if (sampleCount < BUFFER_SIZE)
      sampleCount++;
  }

  // Calcula as médias das últimas amostras
  float sumH = 0, sumT = 0;
  for (int i = 0; i < sampleCount; i++)
  {
    sumH += humidityBuf[i];
    sumT += tempBuf[i];
  }
  float avgH = (sampleCount > 0) ? (sumH / sampleCount) : NAN;
  float avgT = (sampleCount > 0) ? (sumT / sampleCount) : NAN;

  // Imprimir somente a média móvel a cada 1 minuto
  if (millis() - lastPrintTime >= 60000)
  {
    printMediaTimestamp();
    Serial.println("-------------------------------------------------");
    Serial.print("Umidade: ");
    Serial.print(avgH);
    Serial.print(" %\tTemperatura: ");
    Serial.print(avgT);
    Serial.println(" *C");
    lastPrintTime = millis();

    sendData(avgH, avgT); // Envia os dados para a API
  }

  // Aguarda 1 segundos entre as leituras
  delay(1000);
}
