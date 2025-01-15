/*************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Definicion de la clase para el uso de Joystick SEGA
 *
 * Version: 1.0 Enero 2025

 ****************************************************************/

#include "JoystickSEGA.h"

void JoystickSEGA::iniciaJoystickSEGA(byte db9_7_SE, byte db9_1_Ar, byte db9_2_Ab, byte db9_3_Iz, 
                          byte db9_4_De, byte db9_6_AB, byte db9_9_C)
{
   _pinDB9Select = db9_7_SE;
   _pinDB9Entrada[0]= db9_1_Ar;
   _pinDB9Entrada[1]= db9_2_Ab;
   _pinDB9Entrada[2]= db9_3_Iz;
   _pinDB9Entrada[3]= db9_4_De;
   _pinDB9Entrada[4]= db9_6_AB;
   _pinDB9Entrada[5]= db9_9_C;

   // Configuramos pines de salida y entrada
   pinMode(_pinDB9Select, OUTPUT);
   for (byte i = 0; i < PINES_ENTRADA_DB9; i++)
   {
      pinMode(_pinDB9Entrada[i], INPUT_PULLUP);
   }

   _ultimoTiempo = millis();

}
// Lee botones si paso el tiempo de retardo
void JoystickSEGA::leeBotones(pf_cambioBoton funcion)
{
  if(max(millis() - _ultimoTiempo,0) < RETARDO_LECTURA_MS) return; // No paso tiempo
  
  // Paso tiempo por lo tanto se incica lectura y se reinicia tiempo;
  for (int i=0; i < 8; i++)
  {
    ciclo(i, funcion);
  }
  
  _ultimoTiempo = millis();   // Reiniciamos tiempo
  
}

/*   Ejecuta el estado que corresponda y se prepara para el proximo.
  8 estados del 0-7
    Estado 	Select (Pin 7) 	Pin 1 	Pin 2 	Pin 3 	Pin 4 	Pin 6 	Pin 9
      0           LOW 						
      1 	        HIGH 						
      2 	        LOW 			                  * 	    * 	    A 	  Start     Pin 3 y 4 a bajo indican que el mando esta conectado
      3 	        HIGH 	      Ar 	  Ab 	    Izq 	  Der    	  B 	    C
      4 	        LOW 	      ** 	   ** 				                              Pin 1 y 2 a bajo indican que el mando 6 botones conectado
      5 	        HIGH 	      Z 	   Y 	    X 	    Mode 		                  Lee X, Y, Z y Mode.
      6 	        LOW 						                                            
      7 	        HIGH 						                                   
         
*/
void JoystickSEGA::ciclo(int estado, pf_cambioBoton funcion)
{
    boolean seCambio = false;
    digitalWrite(_pinDB9Select, estado % 2); //Pone select en pin

    switch (estado)
    {
       case 0: // Estado 0 LOW No realiza nada, sale a estado 1.
          break;
       case 1: // Estado 1 HIGH No realiza nada solo pasa a estado 2
          break;
       case 2:  // Estado 2 LOW Si esta conectado (Izq y Der a LOW) el mando mira si se pulsa A o Start 
         if ((digitalRead(_pinDB9Entrada[2]) == LOW) && (digitalRead(_pinDB9Entrada[3]) == LOW))
         {
            for ( uint8_t i = 4; i < PINES_ENTRADA_DB9; i++ ) // Leemos solo A (4) y Start (5)
            {
               if (digitalRead(_pinDB9Entrada[i]) == LOW) seCambio = comprueba(i+2, true); // Se esta pulsando.
               else  seCambio = comprueba(i+2, false); // Es que el boton esta soltado.
               if(seCambio) funcion(i+2, _estadoBoton[i+2]);
            }
         } 
         break;
       case 3: // Estado 3 HIGH Lee Arriba, Abajo, Izquierda, Derecha, B, C
          for ( uint8_t i = 0; i < PINES_ENTRADA_DB9; i++ ) // Leemos toda la secuencia
          {
             if (digitalRead(_pinDB9Entrada[i]) == LOW) seCambio = comprueba(i, true); // Se esta pulsando.
             else  seCambio = comprueba(i, false); // Es que el boton esta soltado.
             if(seCambio) funcion(i, _estadoBoton[i]);
          }
         break;
       case 4:   // Estado 4 LOW Mira si esta conectado mando (iz y der a low) de 6 botones y pasa estado
           _seisBotones = (digitalRead(_pinDB9Entrada[0]) == LOW && digitalRead(_pinDB9Entrada[1]) == LOW);
           break;
       case 5:  // Estado 5 HIGH Lee Z 	   Y 	    X 	    Mode
          if(_seisBotones)
          {
             for ( uint8_t i = 0; i < 4; i++ ) // Leemos posibles Z Y X Mode
             {
                // Se suma 8 para llegar al boton Z
                if (digitalRead(_pinDB9Entrada[i]) == LOW) seCambio = comprueba(i+8, true); // Se esta pulsando.
                else  seCambio = comprueba(i+8, false); // Es que el boton esta soltado.
                if(seCambio) funcion(i+8, _estadoBoton[i+8]);
             }
          }
          break;
       case 6: // Estado 6 LOW no realiza nada.
         break;
       case 7:  // Estado 7 HIGH No realiza nada y pasa a estado 0
         break;
     }
}

boolean JoystickSEGA::comprueba(int boton, boolean pasarA)
{
  if(_estadoBoton[boton] != pasarA) // Cambio de estado
   {
      _estadoBoton[boton] = pasarA;   // Ponemos nuevo estado
      return true;
    // Solicita ejecutar accion boton.
   }
   return false;
}


