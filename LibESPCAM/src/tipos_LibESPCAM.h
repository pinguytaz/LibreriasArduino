/*************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Definicion de los tipos utilizados en LibESPCAM
 *  
 *  Version 1.0 Mayo 2023
 **********************************************************/

#ifndef TIPOS_LIBESPCAM_H
#define TIPOS_LIBESPCAM_H

typedef enum
{
   Alto = 255,
   Medio = 100,
   Bajo =25,
   Apagado = 0
} Nivel_Flash;

typedef enum
{
   CAM_OK = 0,

   CAM_Captura = 0x100,   // Error al capturar camara
   CAM_fCaptura = 0x101,  // El error se produce en la funcion de tratamiento.

   CAM_SD = 0x200,         // Error al montar SD
   CAM_Card = 0x201    // No Hay tarjeta.
} Error_ESPCAM;

#endif
