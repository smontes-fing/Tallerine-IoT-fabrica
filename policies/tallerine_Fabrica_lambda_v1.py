import boto3
import json
import logging
from datetime import datetime

logger = logging.getLogger()
logger.setLevel(logging.INFO)

iot_client = boto3.client('iot-data', region_name='your-region')  # Reemplaza 'your-region' con tu región de AWS
dynamodb = boto3.resource('dynamodb', region_name='your-region')  # Reemplaza 'your-region' con tu región de AWS
table = dynamodb.Table('Devices')

def lambda_handler(event, context):
    logger.info(f"Event received: {json.dumps(event)}")
    for record in event['Records']:
        payload = json.loads(record['body'])
        device_id = payload['id']
        estado = payload['estado']
        prox_estado = payload.get('prox_estado', '')

        logger.info(f"Processing device: {device_id} with state: {estado}")

        # Manejar estados en función del tipo de dispositivo y el estado recibido
        if device_id.startswith('auto'):
            handle_auto_state(device_id, estado, prox_estado)
        elif device_id.startswith('cinta'):
            handle_cinta_state(device_id, estado, prox_estado)
        elif device_id.startswith('grua'):
            handle_grua_state(device_id, estado, prox_estado)

def handle_auto_state(device_id, estado, prox_estado):
    if estado == 'reposo':
        # Código para manejar el estado 'reposo' del auto
        pass
    elif estado == 'posicionado_para_recibir':
        notify_cinta_to_deliver_package(device_id)
    elif estado == 'paquete_recogido':
        assign_random_grua_to_auto(device_id)
    elif estado == 'posicionado_para_entregar':
        notify_grua_to_receive_package(device_id)
    elif estado == 'paquete_entregado':
        update_device_state(device_id, 'reposo', prox_estado)

def handle_cinta_state(device_id, estado, prox_estado):
    if estado == 'paquete_listo':
        assign_auto_to_cinta(device_id)
    elif estado == 'paquete_entregado':
        update_device_state(device_id, 'reposo', prox_estado)

def handle_grua_state(device_id, estado, prox_estado):
    if estado == 'posicionado_para_recibir':
        notify_auto_to_deliver_package(device_id)
    elif estado == 'paquete_guardado':
        update_device_state(device_id, 'reposo', prox_estado)
        notify_auto_to_return_to_rest(device_id)

def update_device_state(device_id, estado, prox_estado):
    logger.info(f"Updating state for {device_id} to {estado}")
    update_expression = 'SET estado = :val, prox_estado = :prox, last_ping = :now'
    expression_values = {':val': estado, ':prox': prox_estado, ':now': datetime.utcnow().isoformat()}
    table.update_item(
        Key={'id': device_id},
        UpdateExpression=update_expression,
        ExpressionAttributeValues=expression_values
    )

def notify_cinta_to_deliver_package(auto_id):
    # Notificar a la cinta para entregar el paquete
    cinta_id = 'cinta1'  # Asumimos una sola cinta por simplicidad
    topic = f"cinta/{cinta_id}/estado"
    payload = {'id': cinta_id, 'estado': 'entregar_paquete'}
    publish_message_to_topic(topic, payload)

def assign_random_grua_to_auto(auto_id):
    # Asignar una grúa aleatoriamente al auto
    grua_id = 'grua1'  # Asumimos una sola grúa por simplicidad
    topic = f"grua/{grua_id}/estado"
    payload = {'id': grua_id, 'estado': 'posicionar'}
    publish_message_to_topic(topic, payload)

def notify_grua_to_receive_package(auto_id):
    # Notificar a la grúa para recibir el paquete
    grua_id = 'grua1'  # Asumimos una sola grúa por simplicidad
    topic = f"grua/{grua_id}/estado"
    payload = {'id': grua_id, 'estado': 'recibir_paquete'}
    publish_message_to_topic(topic, payload)

def notify_auto_to_deliver_package(grua_id):
    # Notificar al auto para entregar el paquete a la grúa
    auto_id = 'auto1'  # Asumimos un solo auto por simplicidad
    topic = f"auto/{auto_id}/estado"
    payload = {'id': auto_id, 'estado': 'entregar_paquete'}
    publish_message_to_topic(topic, payload)

def notify_auto_to_return_to_rest(grua_id):
    # Notificar al auto para volver al estado de reposo
    auto_id = 'auto1'  # Asumimos un solo auto por simplicidad
    topic = f"auto/{auto_id}/estado"
    payload = {'id': auto_id, 'estado': 'reposo'}
    publish_message_to_topic(topic, payload)

def publish_message_to_topic(topic, payload):
    logger.info(f"Publishing message to {topic}: {payload}")
    iot_client.publish(
        topic=topic,
        qos=1,
        payload=json.dumps(payload)
    )
