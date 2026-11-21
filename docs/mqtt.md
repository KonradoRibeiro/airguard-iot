# MQTT

Broker público: `broker.hivemq.com`, porta 1883 (sem TLS, apenas para fins didáticos).
Os tópicos usam o prefixo `airguard-upm/` para evitar colisão com outros usuários do broker público.

Exemplo de telemetria:

```json
{"temperatura":24.0,"umidade":40.0,"gas":2762,"ts":1795137930}
```

Teste manual pelo HiveMQ Web Client (https://www.hivemq.com/demos/websocket-client/): assine `airguard-upm/#` e publique `ON` ou `OFF` em `airguard-upm/d2/comando`.
