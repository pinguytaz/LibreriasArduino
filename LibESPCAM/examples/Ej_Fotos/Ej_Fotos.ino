/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo AI Thinker ESP32-CAM
 *               Toma fotos y se graban en Tarjeta
 ***********************************************************************************************/
#include "LibESPCAM.h"    // Cabecera de funciones
#include <SD_MMC.h>

ESPCAM Camara;
// Funciones que informa de la foto capturada
Error_ESPCAM procesa_imagen(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   log_i("Procesamos la imagen de longitud %d Alto %d y Ancho %d en formato %d",tamano, alto, ancho, formato);
   log_i("En el tiempo %lu en microsegundos", tiempo.tv_usec);
   return CAM_OK;
}
// Funciones que toma una foto y la salva en la tarjeta
Error_ESPCAM grabafoto(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   if(!SD_MMC.begin()) 
   { 
     log_e("Fallo al montar") ;
     return CAM_fCaptura;
   }
  
   if(SD_MMC.cardType() == CARD_NONE)
   {
     log_e("Sin Tarjeta") ;
     return CAM_fCaptura;
   }
 
   // Iniciamos el proceso de salvar el fichero
   String path = "/Foto_" + String((char *)parametros) + String(tiempo.tv_usec) +".jpg";   //Se define nombre con el tiempo.

   fs::FS &fs = SD_MMC; 
   File file = fs.open(path.c_str(), FILE_WRITE);
   if(!file)
   {
     log_e("Fallo al abrir el fichero");
     SD_MMC.end();  // Desmontamos para liberar.
     return CAM_fCaptura;
   } 
   else 
   {
     file.write(buf, tamano); // Escribimos los datos de la imagen pasada.
     log_i("Fichero salvado: %s\n", path.c_str()); 
     file.close();
   }

   SD_MMC.end();  // Desmontamos para liberar.

   return CAM_OK;
}

void setup() 
{
   Serial.begin(115200);
   Serial.println("Inicio");

   esp_err_t err = Camara.iniciaCAM();
   if (err != ESP_OK) 
   {
      log_e("Fallo en la inicializacion de la camara con error 0x%x", err);
     return ;
   }
   
   char *cadenas[]={"Flash-Medio", "Flash-Alto", "Flash-Bajo"};  // Las tres fotos que tomara con distinto nivel de Flash
   Camara.Flash(Bajo);
   if(Camara.Foto(grabafoto, cadenas[2]) != CAM_OK) log_e("Error al realizar foto con %s",cadenas[2]);
   Camara.Flash(Apagado);

   Camara.Flash(Medio);
   if(Camara.Foto(grabafoto, cadenas[0]) != CAM_OK) log_e("Error al realizar foto con %s",cadenas[0]);
   Camara.Flash(Apagado);

   Camara.Flash(Alto);
   if(Camara.Foto(grabafoto, cadenas[1]) != CAM_OK) log_e("Error al realizar foto con %s",cadenas[1]);
   Camara.Flash(Apagado);
}

void loop() 
{

}
