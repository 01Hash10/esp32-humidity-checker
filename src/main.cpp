#include <Arduino.h>
#include <DHT.h>

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

// Função para exibir os dados do sensor
void printSensorData(float humidity, float temperature)
{
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Falha na leitura do sensor!");
  }
  else
  {
    Serial.print("Umidade: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" *C");
  }
}

void setup()
{
  // Inicializa a comunicação serial
  Serial.begin(115200);
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
    Serial.println("Média móvel (último 1 minuto): ");
    Serial.println("-------------------------------------------------");
    Serial.print("Umidade: ");
    Serial.print(avgH);
    Serial.print(" %\tTemperatura: ");
    Serial.print(avgT);
    Serial.println(" *C");
    lastPrintTime = millis();
  }

  // Aguarda 1 segundos entre as leituras
  delay(1000);
}
