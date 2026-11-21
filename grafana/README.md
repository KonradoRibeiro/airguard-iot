# Dashboards Grafana

Fonte de dados: InfluxDB Cloud (Flux), bucket `airguard`. Os JSON não contêm tokens.

| Arquivo | Conteúdo |
|---|---|
| `dashboard-monitoramento.json` | CO2 (série e medidor), temperatura, umidade e estado do exaustor |
| `dashboard-interno-externo.json` | Temperatura e umidade internas x externas (OpenWeather) e linha do tempo da ventilação |
| `Air Guard - Monitoramento.pdf` | Captura do dashboard de monitoramento |
| `AirGuard - Interno x Externo e Ventilação.pdf` | Captura do dashboard interno x externo |

Para importar: Dashboards → New → Import → enviar o JSON → escolher a fonte de dados InfluxDB.
