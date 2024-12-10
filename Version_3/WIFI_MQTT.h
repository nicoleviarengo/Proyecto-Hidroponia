#ifndef WIFI_MQTT_h
#define WIFI_MQTT_h

  #include "defines.h"
  #include <WiFi.h>
  #include <ArduinoJson.h>

  //Las unicas funciones que utiliza son
  void InicializarWIFIMQTT();
  void PublicarDatos(DatosStruct DATOSWIFI);
  void printParametros(ParametrosStruct ParametrosPrint);

#endif    //WIFI-MQTT_h
