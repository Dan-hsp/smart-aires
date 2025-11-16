# 🌱 Smart-Aires  
### Estação Inteligente de Monitoramento Ambiental com ESP32 + MQTT  
**Autor:** Danilo Herbert Silva Pedrosa  
**Disciplina:** Objetos Inteligentes Conectados – UPM  
**Ano/semestre:** 2025 / 2 

---

## Sobre o Projeto

O **Smart-Aires** é um protótipo IoT desenvolvido com o objetivo de monitorar a qualidade do ar em tempo real, utilizando sensores de baixo custo conectados a um ESP32.  
Os dados coletados incluem:

- 🌡️ Temperatura  
- 💧 Umidade relativa do ar  
- 🫁 Índice de qualidade do ar (estimado pelo MQ-135)  

As informações são processadas pelo ESP32 e publicadas via **MQTT**, permitindo visualização remota em dashboards como o **HiveMQ WebSocket Client**.

O LED RGB funciona como um **indicador visual imediato**:

- 🟢 Verde — Boa qualidade do ar  
- 🟡 Amarela — Moderada  
- 🔴 Vermelha — Crítica  

---

##  Arquitetura do Sistema

[Sensores] → [ESP32] → [MQTT Broker] → [Cliente/Servidor]
↓
[LED RGB]


**Sensores utilizados:**
- DHT22 – temperatura e umidade  
- MQ-135 – gases e qualidade do ar  

**Atuador:**
- LED RGB (visualização do estado do ar)

**Protocolo de comunicação:**
- MQTT (publish no tópico `smartaires/ambiente`)

---

## Tecnologias Utilizadas

- ESP32 DevKit V1  
- Arduino IDE (C++)  
- MQTT (HiveMQ / mqtt-dashboard.com)  
- Wokwi (simulação)  
- HiveMQ WebSocket Client  
- GitHub  

---

## Tópicos MQTT

### Publicação (dados ambientais)


smartaires/ambiente

Exemplo de payload:

{
  "temperatura": 25.60,
  "umidade": 53.10,
  "qualidade_ar": 185.0,
  "status": "Moderada"
}


## Assinatura (comandos opcionais para LED)

smartaires/cmd/led


## Comandos aceitos:

verde
amarelo
vermelho
off


## Como Executar:

✔ 1. Clonar o repositório
git clone https://github.com/Dan-hsp/smart-aires

✔ 2. Abrir o código na Arduino IDE
Arquivo principal:
code/smart_aires.ino

✔ 3. Selecionar a placa
ESP32 Dev Module

✔ 4. Conectar ao broker MQTT
broker.hivemq.com
port 1883

✔ 5. Testar com o HiveMQ WebClient
Acesse:
https://www.hivemq.com/demos/websocket-client/

### Assine:
smartaires/ambiente


### Capturas de Tela

As imagens devem ser colocadas na pasta:

/images

E referenciadas assim:

LED Verde – Ar Bom

LED Amarelo – Ar Moderado

LED Vermelho – Ar Crítico

Publicações MQTT


## Demonstração em Vídeo

Link (YouTube – não listado):
https://youtu.be/qKS55rGzOxA?si=Uj0MtRKBFYYEARNu


## Resultados
Tempos de resposta medidos:

| Nº        | Sensor → MQTT (s) | Sensor → LED (s) |
| --------- | ----------------- | ---------------- |
| 1         | 3.0               | 4.0              |
| 2         | 3.0               | 6.0              |
| 3         | 4.0               | 4.0              |
| 4         | 3.5               | 5.0              |
| **Média** | **3.4 s**         | **4.8 s**        |

### Prints dos gráficos estão disponíveis no artigo.


## Observações Técnicas

O sensor MQ-135 no Wokwi não simula ppm reais, apenas tensões analógicas convertidas via ADC.

O valor qualidade_ar representa uma estimativa proporcional, adequada para fins didáticos e demonstrativos.


## Artigo Completo

Quando o PDF final estiver pronto, coloque aqui:
/docs/OIC_A4-Artigo_Smart-Aires.pdf


## Licença

Projeto acadêmico — uso livre para fins educacionais.



