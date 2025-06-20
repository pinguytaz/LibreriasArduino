/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo streaming AI Thinker ESP32-CAM con WebServer
 *               Genera Página WEB para ver streaming
 *             Conectar http://192.168.50.1:8081/stream
 *               
 ********************************************************************************************/
#include "LibESPCAM.h"    // Cabecera de funciones

#include <WiFi.h>                 // Config. WIFI
#include <WebServer.h>

// Constantes para Streaming para MJPEG
// Define la cadena limite, para separar partes en la respuesta HTTP 
#define PART_BOUNDARY "123456789000000000000987654321"  


// ================================
// Credenciales de nuestra red WIFI de nuestro AP para que se conecten otros
// ================================
const char *ssid = "LaCamara";
const char *clave = "LaClaveLarga";


// Creamos clase de camara
/* Optimizado para transmisión de video
    calidad JPEG entre 10 y 15 y se podra cambiar con set_calidad()
    Tamaño del Frame VGA(640x480 o QVGA(320x240) que podremos cambiar con set_framesize(FRAMESIZE_VGA | FRAMESIZE_QVGA)
    Buffer 2 mejora fluidez (maximiza los FPS realies) y mejora FPS y aprobecha PSRAM sin llegar a agotarla.
    Modo grabacion: CAMERA_GRAB_LATEST para priorizar el frame más reciente.
*/
ESPCAM Camara(12, FRAMESIZE_VGA, 2, CAMERA_GRAB_LATEST );


// Creamo el servidor en el puerto 8081
WebServer camCtr(8081);

// **************   Función callBack que procesa el Frame
Error_ESPCAM procesa_frame(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   uint8_t *bufjpg = NULL;
   size_t tamanojpg = 0;
   esp_err_t res = ESP_OK;
   char part_buf[64];
   
   if (formato != PIXFORMAT_JPEG) // Debemos convertir a JPEG
   {   
     bool jpeg_convertido = fmt2jpg(buf, tamano, ancho, alto, formato, 80, &bufjpg, &tamanojpg);
     if(!jpeg_convertido) res = ESP_FAIL;
   }
   else 
   {
     tamanojpg = tamano;
     bufjpg = buf;
   }

   char partHeader[128];
   int partLen = snprintf(partHeader, sizeof(partHeader), 
                          "--%s\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", 
                          PART_BOUNDARY, tamanojpg);
   camCtr.client().write(partHeader, partLen);
   // Enviar imagen JPEG
   camCtr.client().write(bufjpg, tamanojpg);
    
   // Liberamos memoria, que se uso en los Frames
   if(formato != PIXFORMAT_JPEG) free(bufjpg);
   if(res != ESP_OK) return CAM_fCaptura;
        
   return CAM_OK;
}

////////////////    Configuración inicial.
void setup() 
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Inicio");

  // Inicializamos la Camara
   esp_err_t err = Camara.iniciaCAM();
   if (err != ESP_OK) 
   {
     log_e("Fallo en la inicializacion de la camara con error 0x%x", err);
     return ;
   }

   // Configuramos el WIFI en modo AP
   WiFi.mode(WIFI_AP) ;    // Nos ponemos en modo punto de acceso
   if (!WiFi.softAP(ssid, clave)) 
   {
     log_e("Fallo en la creacion del AP");
      while(1);
      //ESP.restart();
   }
   IPAddress    apIP(192, 168, 50, 1);
   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));  
   WiFi.softAP(ssid, clave);  
   WiFi.setSleep(false);  // Evitamos que se duerma el WiFi

   IPAddress miIP = WiFi.softAPIP();   // Obtenemos la IP del AP
   Serial.print("Estacion conectada en IP del AP: ");
   Serial.println(miIP);

 // configuramos el servidor, Tendra paginas captura, activar Flash, ...
 configuraServidor();

}

/////////////////     Bucle
void loop() 
{
   camCtr.handleClient();
   delay(2);
}

//  ***** Configuracion del servidor   **********************************
void configuraServidor(void)
{
    // Servidor Streaming   
    // Evitamos que los buscadores rastreen, por si pusieramos el sistema al publico
    camCtr.on("/robots.txt", HTTP_GET, [] { camCtr.send(200, "text/html", "User-Agent: *\nDisallow: /\n"); });
 
    camCtr.on("/stream", HTTP_GET,Pag_Stream);

    camCtr.begin();
}

void Pag_Stream()   // Streaming
{
   esp_err_t res = ESP_OK;
   Error_ESPCAM error=CAM_OK;
   
   // Cabecera HTTP inicial
   String header = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: multipart/x-mixed-replace; boundary=" PART_BOUNDARY "\r\n\r\n";
   camCtr.client().write(header.c_str(), header.length());

   while (camCtr.client().connected())    // Mientras existe conexión
   {
     // Captura, transforma a JPEG
     error = Camara.Foto(procesa_frame,NULL) ;
     if (error != CAM_OK)
     {
       res = ESP_FAIL; 
       break;
     }
     delay(33); // ~30 FPS
   }
 }

