/***************************************************************************************
 Autor:  Fco. Javier Rodriguez Navarro   
 WEB:    www.pinguytaz.net
 
 Descripción: Ejemplo de uso, utilizando dos Joystick y por lo tanto
 dos funciones de recolección.

 1   2   3   4   5
    6   7   8   9
    JoyStick  
    1     2   Funcion     
1   A0    D11  Arriba/Z
2   A1    D10  Abajo/Y
3   A2    D9   Izq./X
4   A3    D8   Der./Mod
5             +5
6   A4    D7  A/B
7   D12   D13 Select
8             GND
9   A5    D6  Start/C         

Librerias: JoystickSEGA
********************************************************************************************/

#include <JoystickSEGA.h>

JoystickSEGA controlador1;
JoystickSEGA controlador2;

String SEGA6[12] = {"Arriba", "Abajo", "Izquierda", "Derecha", 
                        "B", "C", "A", 
                        "START","Z", "Y", "X", 
                        "MODE"};

void setup()
{
  Serial.begin(9600);
  // Select,1,2,3,4,6,9
  controlador1.iniciaJoystickSEGA(12, A0, A1, A2, A3, A4, A5);  
  controlador2.iniciaJoystickSEGA(13, 11, 10, 9, 8, 7, 6);  


  Serial.println("Iniciamos programa");
}

void loop(void)
{
  controlador1.leeBotones(Joystick1);
  controlador2.leeBotones(Joystick2);
}

// Definicion de la funcion que se llamara cuando se pulsa un boton
void Joystick1(BTN boton, boolean pulsa)
{
  
  if (pulsa)    // El boton ha sido pulsado.
  {
     Serial.print("Se pulsa el Joy1-> ");
  }
  else   // El boton ha sido soltado
  {
     Serial.print("Se suelta en Joy1-> ");
  }
   
  Serial.println(SEGA6[boton]);
}

// Definicion de la funcion que se llamara cuando se pulsa un boton del Joystick2
void Joystick2(BTN boton, boolean pulsa)
{
  
  if (pulsa)    // El boton ha sido pulsado.
  {
     Serial.print("Joystick2-> ");
  }
  else   // El boton ha sido soltado
  {
     Serial.print("Suelta Joystick2-> ");
  }
   
  Serial.println(SEGA6[boton]);
}


