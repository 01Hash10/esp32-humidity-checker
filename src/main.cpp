#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4      // Pino digital conectado ao sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302). Para DHT11, use DHT11

DHT dht(DHTPIN, DHTTYPE);

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
  // Leitura da umidade (%)
  float humidity = dht.readHumidity();
  // Leitura da temperatura em Celsius
  float temperature = dht.readTemperature();

  // Verifica se houve erro na leitura (retorno é NAN em caso de erro)
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

  // Aguarda 2 segundos entre as leituras
  delay(2000);
}
