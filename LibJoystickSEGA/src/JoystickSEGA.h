/***************************************************************************************
  Autor: Fco. Javier Rodriguez Navarro   
  WEB:   www.pinguytaz.net
 
  Descripción: Libreria para el uso de Josytick SEGA de 6 botones y conector DB9        
  Version: 1.0 Enero 2025
********************************************************************************************/
#ifndef JOYSTICKSEGA_H
#define JOYSTICKSEGA_H

#include <Arduino.h>

const byte PINES_ENTRADA_DB9 = 6;
const unsigned long RETARDO_LECTURA_MS = 17; // Retardo lectura de 17ms

// Descipcion botones
typedef enum
{
   BTN_Arriba = 0,
   BTN_Abajo  = 1,
   BTN_Izq    = 2,
   BTN_Der    = 3,
   BTN_B      = 4,
   BTN_C      = 5,
   BTN_A      = 6,
   BTN_Start  = 7,
   BTN_Z      = 8,
   BTN_Y      = 9,
   BTN_X      = 10,
   BTN_Mode   = 11
} BTN;

// Definicion tipo funcion que se llamara al recibir un boton
typedef void (*pf_cambioBoton)(BTN boton, boolean pulsa);

class JoystickSEGA 
{
    public:
        // JoystickSEGA();
        void iniciaJoystickSEGA(byte db9_7_SE, byte db9_1_Ar, byte db9_2_Ab, byte db9_3_Iz, 
                                byte db9_4_De, byte db9_6_AB, byte db9_9_C);

        void leeBotones(pf_cambioBoton);
        

    private:
      void ciclo(int, pf_cambioBoton);
      boolean comprueba(int, boolean);

      byte _pinDB9Select;
      byte _pinDB9Entrada[PINES_ENTRADA_DB9];
      unsigned long _ultimoTiempo;
      boolean _seisBotones=false;
      
      // Control del ultimo estado del boton incialmente sueltos.
      boolean _estadoBoton[12] = { false, false, false, false, false, false, false, false, false, false, false, false }; 
};
#endif
