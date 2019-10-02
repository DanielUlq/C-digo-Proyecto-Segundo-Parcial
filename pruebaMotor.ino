/*librerias*/
#include <Ultrasonic.h>
#include "Stepper.h"

//variables para sensor ultrasonico
#define TRIG 12                     //trigger del sensor
#define ECHO 13                     //echo del sensor

//variables para alarma
#define led 5

/*----- Variables, Pins -----*/
#define STEPS  32   // numero de pasos por revolución
int  Steps2Take;  // 2048 = 1 revolución

/*variables para detectar la caja*/
const int foto = 0;
int valorLDR = 0;

/*variables para proceso 2 contar los objetos*/
const int fotoContar = 1;
int valorLDRContar = 0;
int val = 0;   //cuenta los objetos y los va almacenando

int estadoFinal = 0;  //detecta el estado en el que se llega al final del recorrido


/*-----( Declare objects )-----*/
// Configuración de la secuencia correcta para los pines del controlador del motor
// In1, In2, In3, In4 en la secuencia 1 - 3 - 2 - 4

Stepper small_stepper(STEPS, 8, 10, 9, 11);
Ultrasonic ultrasonico(TRIG, ECHO); //se inicializa la libreria

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  Serial.println("CLEARDATA");
  Serial.println("LABEL,Hora,distancia,estadoFinal, mensaje");
}

void loop()
{
  valorLDR = analogRead(foto);
  Serial.println(valorLDR);


  /*proceso en el que la banda transportadora inferior mueve una caja*/
  while (valorLDR < 620) {
    valorLDR = analogRead(foto);
    Serial.println(valorLDR);

    small_stepper.setSpeed(500);
    Steps2Take  =  -200;  // rota en sentido contrario al reloj
    small_stepper.step(Steps2Take);
    val = 0;
    /*proceso en el que se verifica si la caja llega al final de su recorrido y que haya pasado por el proceso de llenado*/
    float distancia = ultrasonico.Ranging(CM);  //se obtiene distancia en centimetros

    if (distancia < 5.0 && estadoFinal == 1) {
      Serial.print("DATA,TIME,");
      Serial.print(distancia);
      Serial.print(",");
      Serial.print(estadoFinal);
      Serial.print(",");
      Serial.println("Alarma activada");
      
      digitalWrite(led, HIGH);
      delay(2000);
      estadoFinal = 0;
    }
    digitalWrite(led, LOW);
  }

  /*proceso en el que se cuentan  los objetos que ingresan a la caja*/
  while (valorLDR >= 620 && val < 3) {
    valorLDR = analogRead(foto);
    //Serial.println(valorLDR);

    small_stepper.setSpeed(500);
    Steps2Take  =  0;  // rota en sentido contrario al reloj
    small_stepper.step(Steps2Take);

    valorLDRContar = analogRead(fotoContar);
    Serial.println(valorLDRContar);

    if (valorLDRContar >= 620) {
      val++;
      delay(4000);
    }//cierre de if
    if(val==3){
      estadoFinal = 1;
    }
    
    Serial.println(val);
  } //se cierra while de LDR

  /*proceso en el que detecta si la caja esta llena para poder seguir moviendose*/
  while (valorLDR >= 620 && val == 3) { //para salir de la sombra de la caja
    valorLDR = analogRead(foto);
    Serial.println(valorLDR);

    small_stepper.setSpeed(500);
    Steps2Take  =  -200;  // rota en sentido contrario al reloj
    small_stepper.step(Steps2Take);
  }


}
