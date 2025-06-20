/*************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Definicion de la clase uso de la camara ESP32-CAM
 *               para camara OV264
 *  
 *  NOTAS: Para RGB se recomienda capturar en JPEG y luego convertir 
 * 
 *  Version 1.0 Mayo 2023
 *  Versión 1.1: Febrero 2025  Se modifica para que funcione con version 3 del API ESP
 *  Versión 1.2: Mayo 2025     Solución migración a version 3 del Flash
 *  Versión 2.0: Junuo 2025    Se añaden nuevos metodos para gestion de la camara
 **********************************************************/
#include "LibESPCAM.h"

#include <Arduino.h>
#include <esp32-hal-log.h>
#include <esp32-hal-ledc.h>


// === Constructores.
ESPCAM::ESPCAM(int calidadjpeg, framesize_t tamano, size_t buffers, camera_grab_mode_t modo) 
{
  // Datos por defecto de configuracion de la camara IA
   _config.ledc_channel = LEDC_CHANNEL_0;
   _config.ledc_timer = LEDC_TIMER_0;
   _config.pin_d0 = Y2_GPIO_NUM;
   _config.pin_d1 = Y3_GPIO_NUM;
   _config.pin_d2 = Y4_GPIO_NUM;
   _config.pin_d3 = Y5_GPIO_NUM;
   _config.pin_d4 = Y6_GPIO_NUM;
   _config.pin_d5 = Y7_GPIO_NUM;
   _config.pin_d6 = Y8_GPIO_NUM;
   _config.pin_d7 = Y9_GPIO_NUM;
   _config.pin_xclk = XCLK_GPIO_NUM;
   _config.pin_pclk = PCLK_GPIO_NUM;
   _config.pin_vsync = VSYNC_GPIO_NUM;
   _config.pin_href = HREF_GPIO_NUM;
   _config.pin_sccb_sda = SIOD_GPIO_NUM;
   _config.pin_sccb_scl = SIOC_GPIO_NUM;
   _config.pin_pwdn = PWDN_GPIO_NUM;
   _config.pin_reset = RESET_GPIO_NUM;
   _config.xclk_freq_hz = 20000000;
   _config.fb_location = CAMERA_FB_IN_PSRAM;   // Localizacion del Frame CAMERA_FB_IN_PSRAM, CAMERA_FB_IN_DRAM
   
   // Posibles de configuración  
   // Maxima resolucion de camara OV264 con PSRAM
   // Dejamos fijo formato a JPEG pues se recomienda capturar en el y luego convertir (fmt2rgb888 o fmt2bmp/frame2bmp)
   _config.pixel_format = PIXFORMAT_JPEG; // Formato de los datos JPEG|YUV422|GRAYSCALE|RGB565(para reconocimiento)
   
   _config.frame_size = tamano;  // Tamaño salida de la imagen QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA FRAMESIZE_240X240 Para reconocimiento
   _config.jpeg_quality = calidadjpeg; //Calidad 0-63 a menor numero mas calidad
   _config.fb_count = buffers;  // Numero de frames capturador
                                // Mas de 1 funciona en continuo, ya que mientra se procesa uno la camara puede llenar otro
                                // Requieren más PSRAM 2 optimizado 
   _config.grab_mode = modo; // Indica como se rellenan lo buffers
                             //CAMERA_GRAB_WHEN_EMPTY llena cuando esta vacio, menos recursos pero se recogen fotogramas antiguos.
                             // CAMERA_GRAB_LATEST  siempre tendremos los ultimos, es decir al llamar a fb_get (interesante para fb_count distinto de uno.
}

//==============  Incializacion de la camara que debe ejecutarse en SetUp
esp_err_t ESPCAM::iniciaCAM(void)
{
   // Enciende camara
   pinMode(PWDN_GPIO_NUM, OUTPUT);
   digitalWrite(PWDN_GPIO_NUM, LOW);
   
   // Inicializa
   esp_err_t err = esp_camera_init(&_config);
   if (err != ESP_OK) return err;

   //Iniciamos el sensor
   _sensor = esp_camera_sensor_get();  //Obtenemos los datos del sensor, que luego se modificaran.
   _infoSensor = esp_camera_sensor_get_info(&_sensor->id);

   // Incializamos Flash
   /*    Codigo de V2 del API de ESP
   ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
   ledcAttachPin(LED_GPIO_NUM, LED_LEDC_CHANNEL);
   */
   //ledcAttach(LED_GPIO_NUM, 5000, 8);   // Asignación canal automatica
   ledcAttachChannel(LED_GPIO_NUM, 5000, 8, LED_LEDC_CHANNEL);  // Asignamos nosotros el canal
   Flash(Apagado);  // Inicialmente apagamos el Flash

   //Configuramos LED Rojo
   pinMode(GPIO_LEDROJO, OUTPUT);
   ledRojo(false);
   
   return ESP_OK;
}

// Funciones de Flash y Leds
void ESPCAM::Flash(Nivel_Flash intensidad)
{ 
   ledcWrite(LED_GPIO_NUM, intensidad);
}
void ESPCAM::ledRojo(bool encender)
{
   digitalWrite(GPIO_LEDROJO, !encender);
}

// =====  Funciones para realizar fotos.
Error_ESPCAM ESPCAM::Foto(pf_timagen funcion, void *parametros) 
{
   Error_ESPCAM error = CAM_OK;
   camera_fb_t *fb = NULL;
   fb = esp_camera_fb_get();  // Obtenemos un Frame 
   if(!fb) 
   {
     log_e("Error en la captura de la foto");
     return CAM_Captura;
   }
  
   // Lanzamos la funcion de tratamiento de la foto capturada.

   error = funcion(fb->buf, fb->len, fb->width, fb->height, fb->format, fb->timestamp, parametros);
   
   esp_camera_fb_return(fb);  // Liberamos frame para su uso.

   return error;
}
//==================== Funciones get
pixformat_t ESPCAM::get_pixformat() { return _sensor->pixformat;}
const char * ESPCAM::get_camara() {return _infoSensor->name; }
const bool ESPCAM::get_soporta_jpeg() {return _infoSensor->support_jpeg; }

framesize_t ESPCAM::get_framesize() { return _sensor->status.framesize; }
uint8_t ESPCAM::get_calidad() { return _sensor->status.quality; }
int8_t ESPCAM::get_brillo() { return _sensor->status.brightness; }
int8_t ESPCAM::get_contraste() { return _sensor->status.contrast; }
int8_t ESPCAM::get_saturacion() { return _sensor->status.saturation; }
int8_t ESPCAM::get_efectoEspecial() { return _sensor->status.special_effect; }
int8_t ESPCAM::get_espejoV() { return _sensor->status.vflip; }
int8_t ESPCAM::get_espejoH() { return _sensor->status.hmirror; }



//===================  Funciones SET
void ESPCAM::set_framesize(framesize_t val){ _sensor->set_framesize(_sensor, val); }
void  ESPCAM::set_calidad(uint8_t val) { _sensor->set_quality(_sensor, val); }
void ESPCAM::set_brillo(int8_t val) { _sensor->set_brightness(_sensor,val); }
void ESPCAM::set_contraste(int8_t val) { _sensor->set_contrast(_sensor,val); }
void ESPCAM::set_saturacion(int8_t val) { _sensor->set_saturation(_sensor,val); }
void ESPCAM::set_efectoEspecial(int8_t val) { _sensor->set_special_effect(_sensor,val); }
void ESPCAM::set_espejoV(bool val) { if (val) _sensor->set_vflip(_sensor,1); else _sensor->set_vflip(_sensor,0); }
void ESPCAM::set_espejoH(bool val) { if (val) _sensor->set_hmirror(_sensor,1); else _sensor->set_hmirror(_sensor,0); }
void ESPCAM::set_carta(bool val) { if (val) _sensor->set_colorbar(_sensor,1); else _sensor->set_colorbar(_sensor,0);}


