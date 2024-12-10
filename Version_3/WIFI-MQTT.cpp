#include "WIFI_MQTT.h"

/*  https://github.com/khoih-prog/AsyncMQTT_ESP32

    Esta libreria esta extraida de un ejemplo del repositorio de github anteriormente mencionado y adaptada a nuestro codigo.
    Su unica funcion es, inicializar el WIFI creando una maquina de estados que, cuando se conecta, inicializa la comunicacion
    con el brocker estableciendo una comunicacion MQTT donde, cuando se llama a la funcion publicar, esta publica la estructura
    datos con el topico seteado.

    y suscribe constantemente los mensajes con el topico definido.

    Los datos que publica, los transforma en formato Json antes, y los que suscribe, los recibe en JSON y los carga en la
    estructura parametros.
*/
#include <AsyncMQTT_ESP32.h>

//#define MQTT_HOST         IPAddress(192, 168, 2, 110)

const char *PubTopic  = "DATOS";               // Topic to publish

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

//Declaro un documento de json global, 
StaticJsonDocument<200> Datos_Json;
StaticJsonDocument<200> Parametros_Json;

ParametrosStruct  ParametrosWIFI;

void connectToWifi(){
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt(){
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event){
  
  switch (event){
    #if USING_CORE_ESP32_CORE_V200_PLUS

      case ARDUINO_EVENT_WIFI_READY:
        Serial.println("WiFi ready");
        break;

      case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("WiFi STA starting");
        break;

      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("WiFi STA connected");
        break;

      case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;

      case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Serial.println("WiFi lost IP");
        break;

      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    #else

      case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;

      case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    #endif

      default:
        break;
  }
}

void printSeparationLine(){
  Serial.println("************************************************");
}

void onMqttConnect(bool sessionPresent){
  Serial.print("Connected to MQTT broker: ");
  Serial.print(MQTT_HOST);
  Serial.print(", port: ");
  Serial.println(MQTT_PORT);
  Serial.print("PubTopic: ");
  Serial.println(PubTopic);

  printSeparationLine();
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  mqttClient.subscribe(SuscribeTopic, 2);

  mqttClient.publish(PublishTopic, 0, true, "ESP32 Test");

  printSeparationLine();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason){
  (void) reason;

  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(const uint16_t& packetId, const uint8_t& qos){
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(const uint16_t& packetId){
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

//cada vez que recibe un mensaje por MQTT con el topico que se suscribio previamente..
void onMqttMessage(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties,
                   const size_t& len, const size_t& index, const size_t& total){

  //Serial.println(String("incoming: ") + topic + " - " + payload);

  // Convierte el payload a un string JSON
  char jsonBuffer[len + 1];
  memcpy(jsonBuffer, payload, len);
  jsonBuffer[len] = '\0';

  // //  Cargo en el archivo "Parametros_JSON" el mensaje recibido y retorna error en caso de no
  // //  pasarse correctamente
  DeserializationError error = deserializeJson(Parametros_Json, jsonBuffer);

  if (error) {
    Serial.print("Error de parseo JSON: ");
    Serial.println(error.c_str());
    return;
  }

  //Carga todos los datos Json recibidos en la estructura Parametros
  ParametrosWIFI.SP_Humedad_Suelo_Minima        =Parametros_Json["SP_Humedad_Suelo_Minima"];
  ParametrosWIFI.SP_Humedad_Suelo_Maxima        =Parametros_Json["SP_Humedad_Suelo_Maxima"];

  ParametrosWIFI.SP_Humedad_Ambiente_Minima     =Parametros_Json["SP_Humedad_Ambiente_Minima"];
  ParametrosWIFI.SP_Humedad_Ambiente_Maxima     =Parametros_Json["SP_Humedad_Ambiente_Maxima"];

  ParametrosWIFI.SP_Temperatura_Ambiente_Minima =Parametros_Json["SP_Temperatura_Ambiente_Minima"];
  ParametrosWIFI.SP_Temperatura_Ambiente_Maxima =Parametros_Json["SP_Temperatura_Ambiente_Maxima"];

  ParametrosWIFI.SP_Luminiscencia_Minima        =Parametros_Json["SP_Luminiscencia_Minima"];
  ParametrosWIFI.SP_Luminiscencia_Maxima        =Parametros_Json["SP_Luminiscencia_Maxima"];

  ParametrosWIFI.Tiempo_De_Mensaje              =Parametros_Json["Tiempo_De_Mensaje"];

  xQueueSend(ColaParametros, &ParametrosWIFI, portMAX_DELAY);

}

void onMqttPublish(const uint16_t& packetId){
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void InicializarWIFIMQTT(){
  
  while (!Serial && millis() < 5000);

  delay(500);

  Serial.print("\nStarting FullyFeature_ESP32 on ");
  Serial.println(ARDUINO_BOARD);
  Serial.println(ASYNC_MQTT_ESP32_VERSION);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
                                    reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
  Serial.println("Conexión WiFi establecida.");

  // onMqttSubscribe("PARAMETROS",0);
}

void PublicarDatos(DatosStruct DATOSWIFI){

  Serial.println("Publicando");

  // //Cargo todos los datos que estan cargados en la estructura al momento de publicar,
  // //en un archivo de JSON para publicarlo en el brocker posteriormente.
  Datos_Json["Dispositivo"] =             Dispositivo;
  Datos_Json["Temperatura_De_Ambiente"] = DATOSWIFI.Temperatura_De_Ambiente;
  Datos_Json["Humedad_De_Ambiente"] =     DATOSWIFI.Humedad_De_Ambiente;
  Datos_Json["Humedad_De_Suelo"] =        DATOSWIFI.Humedad_De_Suelo;
  Datos_Json["Luminiscencia"] =           DATOSWIFI.Luminiscencia;

  // // Enviar los datos al servidor

  // //Creo un buffer de tipo char donde almaceno el Documento de JSON
  char buffer[256];
  size_t n = serializeJson(Datos_Json, buffer);

  // Publica el buffer en el brocker MQTT
  mqttClient.publish(PublishTopic, 0, true, buffer);
}

void printParametros(ParametrosStruct ParametrosPrint){
  // Imprime los valores recibidos
  Serial.print("SP_Humedad_Suelo_Minima: ");
  Serial.println(ParametrosPrint.SP_Humedad_Suelo_Minima);
  Serial.print("SP_Humedad_Suelo_Maxima: ");
  Serial.println(ParametrosPrint.SP_Humedad_Suelo_Maxima);
  Serial.print("SP_Humedad_Ambiente_Minima: ");
  Serial.println(ParametrosPrint.SP_Humedad_Ambiente_Minima);
  Serial.print("SP_Humedad_Ambiente_Maxima: ");
  Serial.println(ParametrosPrint.SP_Humedad_Ambiente_Maxima);
  Serial.print("SP_Temperatura_Ambiente_Minima: ");
  Serial.println(ParametrosPrint.SP_Temperatura_Ambiente_Minima);
  Serial.print("SP_Temperatura_Ambiente_Maxima: ");
  Serial.println(ParametrosPrint.SP_Temperatura_Ambiente_Maxima);
  Serial.print("SP_Luminiscencia_Minima: ");
  Serial.println(ParametrosPrint.SP_Luminiscencia_Minima);
  Serial.print("SP_Luminiscencia_Maxima: ");
  Serial.println(ParametrosPrint.SP_Luminiscencia_Maxima);
  Serial.print("Tiempo_De_Mensaje: ");
  Serial.println(ParametrosPrint.Tiempo_De_Mensaje);
}