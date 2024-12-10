import paho.mqtt.client as mqtt
import mysql.connector
import json
from datetime import datetime
#from .models import Resumen

cnx = mysql.connector.connect(user='root', password='waDaDAkep1er', host='localhost', database='hidroponia', port='3306')

# https://www.youtube.com/watch?v=3UDYN0Lsv_Y&ab_channel=ChepeCarlos

# esta funcion se ejecuta cuando se conecta con el broker y suscribe el topico Humedad

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    client.subscribe("DATOS")

# imprime el topico y mensaje convirtiendo los bytes en string

def on_message(client, userdata, msg):
    print("Topico:"+msg.topic+", Mensaje:"+str(msg.payload))
    datos_JSON=msg.payload
    datos_diccionario = json.loads(datos_JSON)  # Convertir cadena de caracteres JSON a un diccionario

    fecha = datetime.now()
    dispositivo = datos_diccionario.get("Dispositivo")
    luz = datos_diccionario.get("Luminiscencia")
    humedad = datos_diccionario.get("Humedad_De_Ambiente")
    temperatura = datos_diccionario.get("Temperatura_De_Ambiente")
    humedadSuelo = datos_diccionario.get("Humedad_De_Suelo")

    cursor = cnx.cursor()
    query = """
        INSERT INTO cultivos_resumen (fecha, dispositivo, luz, humedad, temperatura, humedadSuelo, rendimiento)
    VALUES (%s, %s, %s, %s, %s, %s, %s)
    """
    values = (fecha, dispositivo, luz, humedad, temperatura, humedadSuelo, 0.0)

    try:
        cursor.execute(query, values)
        cnx.commit()  # Confirmar los cambios en la base de datos
        print("Datos guardados exitosamente.")
    except mysql.connector.Error as err:
        print(f"Error al insertar datos: {err}")
    finally:
        cursor.close()

#crea un nuevo objeto cliente de MQTT
mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("broker.emqx.io", 1883, 60)
mqttc.loop_forever() #mientras se reciban datos se va a ejecutar