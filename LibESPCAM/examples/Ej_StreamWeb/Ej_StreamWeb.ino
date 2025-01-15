/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo AI Thinker ESP32-CAM
 *               Genera Página WEB para ver streaming
 *               
 ***********************************************************************************************/
#include "LibESPCAM.h"    // Cabecera de funciones
#include <WiFi.h>
#include <esp_http_server.h>

// Constantes para Streaming
// Define la cadena limite, para separar partes en la respuesta HTTP
#define PART_BOUNDARY "123456789000000000000987654321"  
// Tipo de contenido HTTP, varias partes que se remplazan
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY; 
// Cadena limite utilizada para separar las parte, indicando que sera un flujo continuo de datos (los marcos)
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
// Formato de una parte, es el tipo de imagen y la longitud
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";


// ================================
// Credenciales de nuestra red WIFI de nuestro AP para que se conecten otros
// ================================
const char *ssid = "LaCamara";
const char *clave = "LaClave-debeserlarga";


ESPCAM Camara(10, FRAMESIZE_VGA, 1, CAMERA_GRAB_WHEN_EMPTY );
//ESPCAM Camara(12, FRAMESIZE_UXGA, 1, CAMERA_GRAB_WHEN_EMPTY );
//WebServer servidor(8080);  // Servidor WEB que solo permite una llamada.

// Funcion que captura evento de conexion de una estación
void WiFiConectaEstacion(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Se conecta una estación al AP.");
  Serial.println("Estaciones conectadas: " + String(WiFi.softAPgetStationNum())); 
}

Error_ESPCAM procesa_frame(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   uint8_t *bufjpg = NULL;
   size_t tamanojpg = 0;
   esp_err_t res = ESP_OK;
   char * part_buf[64];
   httpd_req_t *req = (httpd_req_t *)parametros;

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

   if(res == ESP_OK) res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
   if(res == ESP_OK)
   {
     size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, tamanojpg);
     res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
   }
   if(res == ESP_OK) res = httpd_resp_send_chunk(req, (const char *)bufjpg, tamanojpg);

   // Liberamos memoria, que se uso en los Frames
   if(formato != PIXFORMAT_JPEG) free(bufjpg);
   if(res != ESP_OK) return CAM_fCaptura;
        
   return CAM_OK;
}


// Funcion que torma Frame para Streaming


void setup() 
{
   Serial.begin(115200);
   
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
   WiFi.onEvent(WiFiConectaEstacion, ARDUINO_EVENT_WIFI_AP_STACONNECTED);  // Llamada al conectar una estacion.

  // configuramos el servidor
  configuraServidor();
}

void loop() 
{
   //servidor.handleClient();
   delay(2);
}

//  ***** Configuracion del servidor
httpd_handle_t configuraServidor(void)
{
   httpd_config_t config = HTTPD_DEFAULT_CONFIG();
   httpd_handle_t stream_httpd  = NULL;
   
   // ==========  Definimos las URIs
   httpd_uri_t uri_raiz = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = Pag_Raiz,
        .user_ctx = NULL};   

   // Iniciamos servidor
   config.server_port = 8080;
   log_i ("Iniciamos el servidor en el puerto %d");
   if(httpd_start(&stream_httpd , &config) == ESP_OK)  // Si la inicializacion es OK se registran las URis
   {
     httpd_register_uri_handler(stream_httpd , &uri_raiz);
   }

   // Se podria iniciar un servidor propio de Stream con un puerto superior y el primero para pagina
   // config.server_port += 1;
   // config.ctrl_port += 1;

   return stream_httpd;
}



//  ======================= Funciones tratamientos de paginas (URI)
// ===  Funciones tratamiento de URIs
esp_err_t Pag_Raiz(httpd_req_t *req)   // Genera el Streaming
{
   esp_err_t res = ESP_OK;
   Error_ESPCAM error=CAM_OK;

   res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);  //Definimos el tipo de contenido
   if(res != ESP_OK)  return res;

   // Bucle que captura un cuadro, lo convierte a JPEG si es necesario y lo envia como respuesta HTTP
   while (true)
   {
     // Captura, transforma a JPEG
     error = Camara.Foto(procesa_frame, req) ;
     if (error != CAM_OK)
     {
       res = ESP_FAIL; 
       break;
     }
   }
}   
