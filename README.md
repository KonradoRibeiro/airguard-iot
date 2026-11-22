# AirGuard IoT

Monitoramento e controle inteligente da qualidade do ar em ambientes internos.
Projeto da disciplina **Objetos Inteligentes Conectados** (UPM / Mackenzie) — Grupo 04.
Alinhado ao **ODS 11 (Cidades e Comunidades Sustentáveis), meta 11.6** (reduzir o impacto ambiental negativo, com atenção à qualidade do ar).

**Autores:** Konrado Ribeiro, Melissa Rodrigues, Matheus Silva
**Professor:** Wallace Rodrigues de Santana

## Visão geral

Um dispositivo mede temperatura, umidade e um indicador de gás/CO₂. Os dados vão por MQTT ao Node-RED, que valida as leituras, estima o CO₂, classifica a qualidade do ar, decide quando acionar o exaustor (um segundo dispositivo) e compara com o clima externo (OpenWeather). Os dados são gravados no InfluxDB Cloud e exibidos em dashboards no Grafana Cloud.

```
Dispositivo 1 (ESP32: DHT22 + sensor de gás)
        │  MQTT  airguard-upm/d1/telemetria
        ▼
   Broker público HiveMQ  ◄───────────────┐
        │                                  │ MQTT  airguard-upm/d2/comando (ON/OFF, retido)
        ▼                                  │
     Node-RED  ── regras R1 a R5 ──────────┘──►  Dispositivo 2 (ESP32: LED = exaustor)
        │   ▲
        │   └── OpenWeather (clima externo)
        ▼
  InfluxDB Cloud  ──►  Grafana Cloud (2 dashboards)
```

## Dispositivos (simulados no Wokwi)

| Dispositivo | Função | Hardware simulado |
|---|---|---|
| 1 | Sensores | ESP32, DHT22 (GPIO 15), potenciômetro (GPIO 34) simulando o sensor de gás |
| 2 | Atuador | ESP32, LED + resistor de 220 Ω (GPIO 26) representando o exaustor |

Códigos em `dispositivo1/sketch.ino` e `dispositivo2/sketch.ino`.

## Tópicos MQTT

| Tópico | Direção | Conteúdo |
|---|---|---|
| `airguard-upm/d1/telemetria` | D1 → Node-RED | JSON `{temperatura, umidade, gas, ts}` a cada 10 s |
| `airguard-upm/d1/status` | D1 → todos | `online` / `offline` (retido, com Last Will) |
| `airguard-upm/d2/comando` | Node-RED → D2 | `ON` / `OFF` (retido, QoS 1) |
| `airguard-upm/d2/estado` | D2 → Node-RED | `ON` / `OFF` (retido) |

## Regras de negócio (Node-RED)

- **R1 – Validação:** descarta leituras com campos ausentes, não numéricos ou fora de faixa.
- **R2 – CO₂ estimado:** `CO₂ (ppm) = 400 + gas/4095 × 1600`.
- **R3 – Classificação:** Boa (< 700 ppm), Moderada (700 a 1000), Ruim (> 1000).
- **R4 – Exaustor:** liga após 3 leituras consecutivas "Ruim"; desliga quando o CO₂ fica abaixo de 800 ppm (histerese).
- **R5 – Contexto externo:** consulta o clima externo (OpenWeather) a cada 10 min; com ar interno ruim e externo ameno (18 a 28 °C), recomenda ventilação natural.

## Banco de dados (InfluxDB Cloud, bucket `airguard`)

- `qualidade_ar_interna`: temperatura, umidade, gas, co2, leituras_ruim, exaustor_on (tag `classe`)
- `clima_externo`: temperatura, umidade (tag `descricao`)
- `eventos_atuador`: comando (1/0), co2 (tag `estado`)

## Como executar

1. Abra os dois projetos no Wokwi (cole o código de cada pasta, adicione os componentes e o `libraries.txt` com `PubSubClient` e `DHT sensor library`). Rode-os em janelas separadas e visíveis.
2. Instale o Node.js e o Node-RED: `npm install -g node-red` e rode `node-red`.
3. No Node-RED, instale a paleta `node-red-contrib-influxdb` (Menu → Gerenciar paleta).
4. Importe `node-red/flows.json`. Na aba **AirGuard IoT**, configure as variáveis `OPENWEATHER_KEY` e `CIDADE`; no nó de configuração **InfluxDB Cloud**, informe URL, organização, bucket e token. Veja `.env.example`.
5. Implemente (Deploy). Gire o potenciômetro do Dispositivo 1 até o CO₂ passar de 1000 ppm: após 3 leituras o LED do Dispositivo 2 acende.
6. Importe os dois dashboards de `grafana/` no Grafana Cloud (fonte de dados: InfluxDB, linguagem Flux).

> **Segurança:** nenhuma chave ou token deve ser commitado. Use `.env` (ignorado pelo Git) e mantenha os placeholders no `flows.json`.

## Estrutura do repositório

```
dispositivo1/   código do ESP32 de sensores
dispositivo2/   código do ESP32 atuador
node-red/       fluxo exportado (flows.json)
grafana/        dashboards exportados (JSON e PDF)
docs/           documentação complementar
.env.example    variáveis de ambiente (sem segredos)
```

## Links

- Vídeo de apresentação: https://www.youtube.com/watch?v=K-wyigap5oY&t=1s
- Wokwi Dispositivo 1: https://wokwi.com/projects/481053124865694721
- Wokwi Dispositivo 2: https://wokwi.com/projects/481053921174396929
