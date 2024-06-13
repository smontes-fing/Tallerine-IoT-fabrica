import boto3
import json
import logging
from datetime import datetime
import random

logger = logging.getLogger()
logger.setLevel(logging.INFO)

region = 'us-east-2'  # Reemplaza 'your-region' con tu región de AWS
iot_client = boto3.client('iot-data', region_name=region)
dynamodb = boto3.resource('dynamodb', region_name=region)
table = dynamodb.Table('tallerine-fabrica24')

def lambda_handler(event, context):
    logger.info(f"Event received: {json.dumps(event)}")
    
    try:
        for record in event['Records']:
            payload = json.loads(record['body'])
            device_id = payload['id']
            estado = payload['estado']
            prox_estado = payload.get('prox_estado', '')
            sirviendo_a = payload.get('sirviendo_a', 'reposo')  # Por defecto 'reposo' si no se especifica
            
            logger.info(f"Processing device: {device_id} with state: {estado} and serving: {sirviendo_a}")
    
            # Manejar estados en función del tipo de dispositivo y el estado recibido
            if device_id.startswith('auto'):
                handle_auto_state(device_id, estado, prox_estado, sirviendo_a)
            elif device_id.startswith('cinta'):
                handle_cinta_state(device_id, estado, prox_estado, sirviendo_a)
            elif device_id.startswith('grua'):
                handle_grua_state(device_id, estado, prox_estado, sirviendo_a)
            else:
                logger.warning(f"Unknown device type for device_id: {device_id}")
    
    except KeyError as e:
        logger.error(f"KeyError: {str(e)} - Event structure may be incorrect.")
        raise e
    except Exception as e:
        logger.error(f"Error processing event: {str(e)}")
        raise e

def handle_auto_state(device_id, estado, prox_estado, sirviendo_a):
    if estado == 'inicio':
        instance_new_machine(device_id, estado, prox_estado, sirviendo_a)
    if estado == 'reposo':
        update_device_state(device_id, estado, prox_estado, 'reposo')
    elif estado == 'posicionado_para_recibir':
        notify_cinta_to_deliver_package(device_id, sirviendo_a)
    elif estado == 'paquete_recogido':
        assign_grua_to_auto(device_id, sirviendo_a)
    elif estado == 'posicionado_para_entregar':
        notify_grua_to_receive_package(device_id, sirviendo_a)
    elif estado == 'paquete_entregado':
        update_device_state(device_id, 'reposo', prox_estado, 'reposo')

def handle_cinta_state(device_id, estado, prox_estado, sirviendo_a):
    if estado == 'inicio':
        instance_new_machine(device_id, estado, prox_estado, sirviendo_a)
    if estado == 'paquete_listo':
        assign_auto_to_cinta(device_id, sirviendo_a)
    elif estado == 'paquete_entregado':
        update_device_state(device_id, 'reposo', prox_estado, 'reposo')

def handle_grua_state(device_id, estado, prox_estado, sirviendo_a):
    if estado == 'inicio':
        instance_new_machine(device_id, estado, prox_estado, sirviendo_a)
    if estado == 'posicionado_para_recibir':
        notify_auto_to_deliver_package(device_id, sirviendo_a)
    elif estado == 'paquete_guardado':
        update_device_state(device_id, 'reposo', prox_estado, 'reposo')
        notify_auto_to_return_to_rest(device_id, sirviendo_a)

def instance_new_machine(device_id, estado, prox_estado, sirviendo_a):
    logger.info(f"inserting new machine {device_id} with state {estado} and serving to {sirviendo_a}")
    item = {
        'id': device_id,
        'maquina': "auto",  # Ajusta esto según tu lógica para el valor de 'maquina'
        'estado': estado,
        'prox_estado': prox_estado,
        'sirviendo_a': sirviendo_a,
        'last_ping': datetime.utcnow().isoformat()
    }
    try:
        response = table.put_item(
            Item=item
        )
        logger.info("Inserting Item succeeded:")
        logger.info(response)
    except Exception as e:
        logger.error(f"Error inserting item: {str(e)}")
        raise e
        


def update_device_state(device_id, estado, prox_estado, sirviendo_a):
    try:
        response = table.update_item(
            Key={'id': device_id},  # Ajusta 'valor_maquina' según tu lógica
            UpdateExpression='SET estado = :val, prox_estado = :prox, sirviendo_a = :srv, last_ping = :now',
            ExpressionAttributeValues={
                ':val': estado,
                ':prox': prox_estado,
                ':srv': sirviendo_a,
                ':now': datetime.utcnow().isoformat()
            }
        )
        print("UpdateItem succeeded:")
        print(response)
    except Exception as e:
        print(f"Error updating item: {str(e)}")
        raise e

def notify_cinta_to_deliver_package(auto_id, sirviendo_a):
    # Notificar a la cinta para entregar el paquete
    cinta_id = sirviendo_a
    topic = f"cinta/{cinta_id}/estado"
    payload = {'id': cinta_id, 'estado': 'entregar_paquete', 'sirviendo_a': auto_id}
    publish_message_to_topic(topic, payload)

def assign_grua_to_auto(auto_id, sirviendo_a):
    # Seleccionar una grúa aleatoria en estado 'reposo'
    grua_id = get_random_device_id_by_type_and_state('grua', 'reposo')
    
    # Asignar la grúa al auto y actualizar el estado de ambos dispositivos
    update_device_state(auto_id, 'camino_a_grua', 'posicionado_para_entregar', grua_id)
    notify_grua_to_receive_package(auto_id, grua_id)

def notify_grua_to_receive_package(auto_id, grua_id):
    # Notificar a la grúa para recibir el paquete
    topic = f"grua/{grua_id}/estado"
    payload = {'id': grua_id, 'estado': 'recibir_paquete', 'sirviendo_a': auto_id}
    publish_message_to_topic(topic, payload)

def notify_auto_to_deliver_package(grua_id, sirviendo_a):
    # Notificar al auto para entregar el paquete a la grúa
    auto_id = sirviendo_a
    topic = f"auto/{auto_id}/estado"
    payload = {'id': auto_id, 'estado': 'entregar_paquete', 'sirviendo_a': grua_id}
    publish_message_to_topic(topic, payload)

def notify_auto_to_return_to_rest(grua_id, sirviendo_a):
    # Notificar al auto para volver al estado de reposo
    auto_id = sirviendo_a
    topic = f"auto/{auto_id}/estado"
    payload = {'id': auto_id, 'estado': 'reposo', 'sirviendo_a': 'reposo'}
    publish_message_to_topic(topic, payload)

def publish_message_to_topic(topic, payload):
    logger.info(f"Publishing message to {topic}: {payload}")
    iot_client.publish(
        topic=topic,
        qos=1,
        payload=json.dumps(payload)
    )

def get_random_device_id_by_type_and_state(device_type, estado):
    response = table.scan(
        FilterExpression=boto3.dynamodb.conditions.Attr('tipo').eq(device_type) & boto3.dynamodb.conditions.Attr('estado').eq(estado)
    )
    devices = response['Items']
    if not devices:
        raise Exception(f"No {device_type} devices available in state {estado}")
    return random.choice(devices)['id']

def assign_auto_to_cinta(cinta_id, sirviendo_a):
    # Seleccionar un auto aleatorio en estado 'reposo'
    auto_id = get_random_device_id_by_type_and_state('auto', 'reposo')
    
    # Asignar la cinta al auto y actualizar el estado de ambos dispositivos
    update_device_state(auto_id, 'camino_a_cinta', 'posicionado_para_recibir', cinta_id)
    notify_cinta_to_deliver_package(auto_id, cinta_id)
