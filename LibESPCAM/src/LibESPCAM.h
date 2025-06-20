/***********************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *  Descripción: Cabecera LibESPCAM
 *
 *  Version 1.0 Mayo 2023
 *  Versión 2.0: Junuo 2025    Se añaden nuevos metodos 
 **********************************************************/
#ifndef LIBESPCAM_H
#define LIBESPCAM_H

#include "ESPCAM_PIN.h"   // Definimos los pines
#include "tipos_LibESPCAM.h"   // Definimos tipos de datos

#include <esp_camera.h>

//================ Tipos funcion
typedef Error_ESPCAM(*pf_timagen)(uint8_t *, size_t, size_t, size_t, pixformat_t, struct timeval, void *) ;

class ESPCAM
{
   private:
     camera_config_t _config;
     sensor_t *_sensor;
     camera_sensor_info_t *_infoSensor;
      
   public:
      //ESPCAM();  // Constructor por defecto
      ESPCAM(int calidadjpeg=10, framesize_t tamano=FRAMESIZE_UXGA, size_t buffers=2, camera_grab_mode_t modo=CAMERA_GRAB_LATEST );
 
      esp_err_t iniciaCAM(void);  // Inicializacion de camara, Se debe ejecutar en setup

      //==== Tratamiento Flash y led
      void Flash(Nivel_Flash);
      void ledRojo(bool);

      // Realizacion de Fotos
      Error_ESPCAM Foto(pf_timagen funcion,void *parametros=NULL);

      // Recogida de datos del sensor
      pixformat_t get_pixformat();
      const char* get_camara();
      const bool get_soporta_jpeg();
      framesize_t get_framesize();
      uint8_t get_calidad();   // 0 -63
      int8_t get_brillo();//-2 - 2
      int8_t get_contraste();//-2 - 2
      int8_t get_saturacion();//-2 - 2
      int8_t get_efectoEspecial();//0 - 6
      int8_t get_espejoV();   // 0-1
      int8_t get_espejoH();   // 0-1
      
      //Cambia datos sensor
      void set_framesize(framesize_t);
      void set_calidad(uint8_t);
      void set_brillo(int8_t);
      void set_contraste(int8_t);
      void set_saturacion(int8_t);
      void set_efectoEspecial(int8_t);
      void set_espejoV(bool);
      void set_espejoH(bool);
      void set_carta(bool);
};

#endif
