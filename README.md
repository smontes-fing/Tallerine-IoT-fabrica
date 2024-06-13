# Tallerine-IoT-fabrica
proyecto Tallerine de fabrica mediante control IoT en AWS 2024 


### Para implementar esta solución, necesitaremos definir las siguientes componentes:

AWS IoT Core: Para manejar la comunicación entre los dispositivos (autos, cintas y grúas) y AWS Lambda.
- AWS Lambda: Para procesar los mensajes recibidos desde AWS IoT Core y actualizar DynamoDB.
- DynamoDB: Para almacenar el estado de cada dispositivo y las órdenes en curso.
- Arduino (ESP8266): Para cada dispositivo, que se conectará a AWS IoT Core mediante pub/sub.

### Definición de los Tópicos MQTT
Cinta: cinta/{cintaId}/estado
Auto: auto/{autoId}/estado
Grúa: grua/{gruaId}/estado

### Estructura de DynamoDB

- Tabla: Devices
- id (Primary Key): ID del dispositivo (por ejemplo, auto1, cinta1, grua1)
- tipo: Tipo de dispositivo (auto, cinta, grua)
- estado: Estado actual del dispositivo
- prox_estado: Próximo estado previsto para el dispositivo
- last_ping: Último ping recibido (para keep-alive)
- orden_actual: Orden actual en proceso (si aplica)




## Resumen de la Solución

- AWS IoT Core se usa para la comunicación pub/sub entre dispositivos.
- AWS Lambda se encarga de procesar mensajes y actualizar DynamoDB.
- DynamoDB almacena el estado de cada dispositivo.
- CloudWatch monitorea los logs y métricas.
- Arduino ESP8266 en cada dispositivo para manejar la lógica de conexión y suscripción a tópicos MQTT.