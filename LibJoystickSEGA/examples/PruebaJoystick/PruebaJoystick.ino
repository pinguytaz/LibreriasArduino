/***************************************************************************************
 Autor:  Fco. Javier Rodriguez Navarro   
 WEB:    www.pinguytaz.net
 
 Descripción: Ejemplo de uso de la libreria JoystickSEGA, que nos permite realizar la prueba de un Josytick SEGA de 6 botones.

 1   2   3   4   5
    6   7   8   9
      
1(A0).-Arriba/Z
2(A1).-Abajo/Y
3(A2).-Izq./X
4(A3).-Der./Mod
5.-    +5
6(D11).-A/B
7(D8).-Select
8.-    GND
9(D9).-Start/C         

Librerias: JoystickSEGA
********************************************************************************************/

#include <JoystickSEGA.h>

// Definicion de la funcion que se llamara cuando se pulsa un boton
void Joystick1(BTN boton, boolean pulsa)
{
  String SEGA6[12] = {"Arriba", "Abajo", "Izquierda", "Derecha", 
                        "B", "C", "A", 
                        "START","Z", "Y", "X", 
                        "MODE"};

  if (pulsa)    // El boton ha sido pulsado.
  {
     Serial.print("Se pulsa ");
  }
  else   // El boton ha sido soltado
  {
     Serial.print("Se suelta ");
  }
   
  Serial.println(SEGA6[boton]);
}


// Creamos controlador, indicando los pines de este.
//                       PIN_DB9      1   2   3   4   6   7  9     5=+5V y 8=GND
//                       PIN_Arduino  14  15  16  17  11  8  9
//                                    Ar  Ab  Iz  De  AB  SE C                          
//const uint8_t DB9_1_PINS[6] = {14, 15, 16, 17, 11, 9 }; 
                       // D8,A0, A1, A2, A3, D11,D9
JoystickSEGA controlador1;


void setup()
{
  // Para debugear, quitar cuando este listo, al igual que los Serial.print.
  Serial.begin(9600);
  controlador1.iniciaJoystickSEGA(8, 14, 15, 16, 17, 11, 9);  

  Serial.println("Iniciamos programa");
}

void loop(void)
{
  controlador1.leeBotones(Joystick1);
}




