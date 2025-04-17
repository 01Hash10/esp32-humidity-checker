# ESP32 Umidit Sensor

Este projeto utiliza o ESP32 para ler a umidade e temperatura com um sensor DHT22.

## Recursos

- Leitura de umidade e temperatura com a biblioteca DHT.
- Exemplo simples usando o Arduino Framework e PlatformIO.

## Configuração

- Certifique-se de ter o PlatformIO instalado.
- Placa configurada: `nodemcu-32s` com plataforma ESP32.
- Dependências e build configurados no arquivo `platformio.ini`.

## Compilação e Upload

- Utilize `PlatformIO: Build` para compilar.
- Utilize `PlatformIO: Upload` para enviar o firmware para o dispositivo.
- Abra o Monitor Serial (115200) para visualizar as leituras do sensor.

## Estrutura do Projeto

- `src/main.cpp`: Código principal com a leitura do sensor.
- `lib/`: Bibliotecas adicionais (se necessário).
- `.vscode/`: Configurações do VSCode.
- `platformio.ini`: Configurações do projeto PlatformIO.
