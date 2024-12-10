#ifndef defines_h
#define defines_h

  /*LIBRERIA DONDE SE DEFINEN TODAS LAS VARIABLES PARA EL USUARIO*/

  //DEFINICIONES
  #include <Arduino.h>
  extern "C"
  {
    #include "freertos/FreeRTOS.h"
    #include "freertos/timers.h"
    #include "freertos/queue.h"
  }

  //DECLARACION DE VARIABLES
  // #define WIFI_SSID               "Utn_Libre Max" // wifi
  // #define WIFI_PASSWORD           "" // contraseña
  #define WIFI_SSID               "Utn_Libre Max" // wifi
  #define WIFI_PASSWORD           "" // contraseña
  
  #define MQTT_HOST               "broker.emqx.io"        // Broker address
  #define MQTT_PORT               1883

  #define PublishTopic            "DATOS"
  #define SuscribeTopic           "PARAMETROS"

  #define Dispositivo             "ESP32"

  #define PinSensorHumedadDeSuelo 36
  #define PinSensorLuminiscencia  39

  #define PinControlRegadora      13
  #define PinControlLampara       12
  #define LED 2

  #define DHTTYPE                 DHT11
  #define DHTPIN                  25

  #define _ASYNC_MQTT_LOGLEVEL_   1
  #define DATA_QUEUE_SIZE         10

  #define SEGUNDOS 1
  #define MINUTOS 2
  #define HORAS 3

  #define TIEMPO MINUTOS

  typedef struct {
    float Humedad_De_Suelo=0;
    float Humedad_De_Ambiente=0;
    float Temperatura_De_Ambiente=0;
    float Luminiscencia=0;
  }DatosStruct;

  typedef struct{
    uint8_t SP_Humedad_Suelo_Minima = 20;
    uint8_t SP_Humedad_Suelo_Maxima = 80;

    uint8_t SP_Humedad_Ambiente_Minima = 20;
    uint8_t SP_Humedad_Ambiente_Maxima = 80;

    int SP_Temperatura_Ambiente_Minima = 20;
    int SP_Temperatura_Ambiente_Maxima = 80;

    uint8_t SP_Luminiscencia_Minima = 20;
    uint8_t SP_Luminiscencia_Maxima = 80;

    uint8_t Tiempo_De_Mensaje = 30;

    char* MODO_Humedad_De_Suelo =         "AUTOMATICO";
    char* MODO_Humedad_De_Ambiente =      "AUTOMATICO";
    char* MODO_Temperatura_De_Ambiente =  "AUTOMATICO";
    char* MODO_Luminiscencia =            "AUTOMATICO";

  }ParametrosStruct;

  //Declaro externa la cola para que puedan trabajar con ella todas las librerias
  extern QueueHandle_t ColaParametros;

#endif    //defines_h
