//Incluyo librerias Propias
#include "WIFI_MQTT.h"//Libreria donde incluyo todo lo que trata respecto al WIFI y el MQTT de manera
                      //Que simplifico el codigo de muestreo

#include "defines.h"  //Libreria con definiciones globales, para que el usuario solo tenga que acceder
                      //allí para configurar el ESP32

#include <NTPClient.h>//El Protocolo de tiempo de red (NTP) es una aplicación cliente/servidor. 
                      //Cada estación de trabajo, enrutador o servidor debe estar equipado con 
                      //software cliente NTP para sincronizar su reloj con el servidor de tiempo 
                      //de red. En la mayoría de los casos, el software cliente ya reside en el 
                      //sistema operativo de cada dispositivo.
#include <WiFiUdp.h>
#include <DHT.h>//Libreria para usar el DHT

const char* ntpServer = "pool.ntp.org";
QueueHandle_t ColaParametros; //Utiliza colas para determinar si se recibe un mensaje
bool ESTADO = false;
//DECLARACION DE ESTRUCTURAS
DatosStruct       Datos;
ParametrosStruct  Parametros;
WiFiUDP ntpUDP;
NTPClient         timeClient(ntpUDP, ntpServer);
DHT               dht(DHTPIN, DHTTYPE);

void setup() {
  //Inicializo puerto Serie
  Serial.begin(115200);

  //Inicializa Cola
  ColaParametros = xQueueCreate(DATA_QUEUE_SIZE, sizeof(Parametros));

  //Inicializa WIFI y se conecta automaticamente del brocker, los datos
  //del mismo se encuentran en el define.
  InicializarWIFIMQTT();

  timeClient.begin();//Inicializo NTPclient
  timeClient.setTimeOffset(4);//Seteo la hora de Argentina

  //Inicializo DHT
  dht.begin();

  //Declaro Pines de Salida
  pinMode(PinControlRegadora,OUTPUT);
  pinMode(PinControlLampara, OUTPUT);
  pinMode(LED, OUTPUT);
}

//BUCLE PRINCIPAL
void loop() {

  //Las Defino como static para que se declaren una unica vez
  static uint8_t TiempoActual=0;
  static uint8_t TiempoAnterior=0;
  static uint8_t Contador=0;

  digitalWrite(LED, !ESTADO);  // BECAUSE PAINTED BRO, GIVE ME THE BAG BRO, FOR YOUR OWN SAFETY

  //Actualiza la estructura timeClient donde se guardan todos los valores de tiempo
  timeClient.update();

  #if TIEMPO == SEGUNDOS
    TiempoActual = timeClient.getSeconds();
  #elif TIEMPO == MINUTOS
    TiempoActual = timeClient.getMinutes();
  #elif TIEMPO == HORAS
    TiempoActual = timeClient.getHours();
  #endif

  //Cada vez que se actualiza la variable "TiempoActual"
  if(TiempoActual!=TiempoAnterior){

    Serial.println(TiempoActual);

    TiempoAnterior=TiempoActual;
    
    Contador++; //Si lo que se actualiza en la variable "TiempoActual" son los segundos, cuenta los segundos
                //Si lo que se actualiza en la variable "TiempoActual" son los minutos, cuenta los minutos

    /*LAS FUNCIONES ACTUALIZAR REALIZAN LAS LECTURAS DE LOS SENSORES, LAS CARGAN EN LA ESTRUCTURA DATOS
    Y DETERMINAN QUE HACER SEGUN LOS PARAMETROS ESTABLECIDOS*/

    ActualizarHumedadDeSuelo(); //Realiza una lectura de la humedad del Suelo y determina que hacer 
                                  //segun los parametros correspondientes

    ActualizarHumedadYTemperaturaDeAmbiente();  //Realiza una lectura de la humedad y temperatura de ambiente y 
                                                  //determina que hacer segun los parametros correspondientes
    
    ActualizarLuminiscencia();  //Realiza una lectura de la luminiscencia y determina que hacer segun los 
                                  //parametros correspondientes


    // Datos.Luminiscencia           = random(1,100);
    // Datos.Humedad_De_Ambiente     = random(1,100);
    // Datos.Temperatura_De_Ambiente = random(1,100);
    // Datos.Humedad_De_Suelo        = random(1,100);
    
    ESTADO = !ESTADO;

    if(Contador==Parametros.Tiempo_De_Mensaje){//una vez se cumplio el tiempo seteado se publican los datos en el brocker
      Contador=0;
      PublicarDatos(Datos);
    }
  }

  //Como la libreria WIFI-MQTT trabaja de manera asincrona al procesador principal, no puede arriesgarse a que este busque
  //modificar la estructura parametros mientras la esta leyendo el procesador principal. asi que la protego con colas de manera
  //que solo lee durante 100 ms y continua haciendo otra cosa.
  if(xQueueReceive(ColaParametros, &Parametros, 100) == pdPASS) {
    Serial.println("Cola Recibida");
  }
}

void ActualizarLuminiscencia(){

  static int LecturaDeLuminiscencia;

  LecturaDeLuminiscencia = analogRead(PinSensorLuminiscencia);//Lee el sensor de Luz

  Datos.Luminiscencia=(LecturaDeLuminiscencia*100)/4095;//Devuelve un porentaje de Luz que es el rango de lectura del sensor

  Serial.print("Luminiscencia: ");
  Serial.print(Datos.Luminiscencia);
  Serial.println(" %");
  
  if (Parametros.MODO_Luminiscencia=="AUTOMATICO") {
    if(Datos.Luminiscencia <= Parametros.SP_Luminiscencia_Minima){
        digitalWrite(PinControlLampara, HIGH);//Si hay demasiada luz prende la lampara
    }
    else{
      digitalWrite(PinControlLampara, LOW);//Si hay demasiada luz apaga la lampara
      //lo de la luminiscencia maxima igual seria un parametro el que carece de sentido
      //ya que empezaria a oscilar la lampara  
    }
  }
  else if (Parametros.MODO_Luminiscencia=="MANUAL") {
    digitalWrite(PinControlLampara, HIGH); 
  }
  else if (Parametros.MODO_Luminiscencia=="DESACTIVADO") {
    digitalWrite(PinControlLampara, LOW); 
  }
  else{
    Serial.print("ERROR MODO LUMINISCENCIA");
  }
}

void ActualizarHumedadYTemperaturaDeAmbiente(){

  //Los valores que leo del DHT los cargo en la estructura
  Datos.Humedad_De_Ambiente     = dht.readHumidity();
  Datos.Temperatura_De_Ambiente = dht.readTemperature();
  
  Serial.print("Humedad de Ambiente: ");
  Serial.println(Datos.Humedad_De_Ambiente);
  Serial.print("Temperatura de Ambiente: ");
  Serial.println(Datos.Temperatura_De_Ambiente);


  if (isnan(Datos.Humedad_De_Ambiente) || isnan(Datos.Temperatura_De_Ambiente)) {
    Serial.println("Error al leer del sensor DHT");
  }
}

void ActualizarHumedadDeSuelo(){

  static int LecturaHumedadDeSuelo;

  LecturaHumedadDeSuelo = analogRead(PinSensorHumedadDeSuelo);//Lee el sensor de humedad

  Datos.Humedad_De_Suelo=(LecturaHumedadDeSuelo*100)/4095;//Devuelve un porentaje de humedad que es el rango de lectura del sensor

  Serial.print("Humedad De Suelo: ");
  Serial.print(Datos.Humedad_De_Suelo);
  Serial.println(" %");

  if (Parametros.MODO_Humedad_De_Suelo=="AUTOMATICO") {
    if(Datos.Humedad_De_Suelo <= Parametros.SP_Humedad_Suelo_Minima){
        digitalWrite(PinControlRegadora, HIGH); 
    }
    else if(Datos.Humedad_De_Suelo>=Parametros.SP_Humedad_Suelo_Maxima){
      digitalWrite(PinControlRegadora, LOW); // Ej: cuando llego al 80% de humedad deja de regar
    }
  }
  else if (Parametros.MODO_Humedad_De_Suelo=="MANUAL") {
    digitalWrite(PinControlRegadora, HIGH); 
  }
  else if (Parametros.MODO_Humedad_De_Suelo=="DESACTIVADO") {
    digitalWrite(PinControlRegadora, LOW); 
  }
  else{
    Serial.print("ERROR MODO HUMEDAD DE SUELO");
  }
}