/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo AI Thinker ESP32-CAM
 *               Genera Página WEB para ver streaming y cambiar parametros desde esta
 *
 *    Conectarse a la red WIFI definida en las variables: ssid y clave
 *    En el monitor serie indicara la dirección IP pero esta definida a 192.168.50.1 y el
 *    puerto es el 8080 por lo tanto en el navegador http://192.168.50.1:8080/
 ***********************************************************************************************/
#include "LibESPCAM.h"    // Cabecera de funciones
#include "PaginaRaiz.h"

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
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

// ================================
// Credenciales de nuestra red WIFI de nuestro AP para que se conecten otros
// ================================
const char *ssid = "LaCamara";
const char *clave = "LaClave-debeserlarga";

ESPCAM Camara(9, FRAMESIZE_VGA, 1, CAMERA_GRAB_WHEN_EMPTY );

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
     size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, tamanojpg, tiempo.tv_sec, tiempo.tv_usec);
     res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
   }
   if(res == ESP_OK) res = httpd_resp_send_chunk(req, (const char *)bufjpg, tamanojpg);
   
   // Liberamos memoria, que se uso en los Frames
   if(formato != PIXFORMAT_JPEG) free(bufjpg);
   if(res != ESP_OK) 
   {
     log_e("Error envio %X",res);
     return CAM_fCaptura;
   }
        
   return CAM_OK;
}

Error_ESPCAM procesa_foto(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   esp_err_t res = ESP_OK;
   uint8_t *bufjpg = NULL;
   size_t tamanojpg = 0;
   httpd_req_t *req = (httpd_req_t *)parametros;

   if (formato != PIXFORMAT_JPEG) // Debemos convertir a JPEG
   {   
     bool jpeg_convertido = fmt2jpg(buf, tamano, ancho, alto, formato, 80, &bufjpg, &tamanojpg);
     if(!jpeg_convertido) 
     {
        log_e("Error en la conversion");
        return CAM_fCaptura;
     }
   }
   else 
   {
     tamanojpg = tamano;
     bufjpg = buf;
   }

   res = httpd_resp_set_type(req, "image/jpeg");
   if(res != ESP_OK) 
   {
       log_e("Error al enviar tipo");
       return CAM_fCaptura;
   }
   res = httpd_resp_set_hdr(req, "Content-Disposition","inline; filename=Foto.jpg");  
   if(res != ESP_OK) 
   {
       log_e("Error al enviar cabecera");
       return CAM_fCaptura;
   }
   
   if(res == ESP_OK) 
   {
     res = httpd_resp_send(req, (const char *)buf, tamano); 
   }
   else 
   {
      log_e("Error al enviar tipo %d", res);
      return CAM_fCaptura;
   }

   if(res != ESP_OK) 
   {
      log_e("Error al enviar datos %d", res);
      return CAM_fCaptura;
   }
   return CAM_OK;
}

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
   delay(10000);
}

//  ***** Configuracion del servidor
void configuraServidor(void)
{
   httpd_config_t config = HTTPD_DEFAULT_CONFIG();
   config.max_uri_handlers = 16;
   
   httpd_handle_t stream_httpd = NULL;
   httpd_handle_t camara_httpd = NULL;

   // ==========  Definimos las URIs
   httpd_uri_t uri_raiz = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = Pag_Raiz,
        .user_ctx = NULL};   

   httpd_uri_t uri_flash = {
        .uri = "/flash",
        .method = HTTP_GET,
        .handler = controlFlash,
        .user_ctx = NULL};   

   httpd_uri_t uri_control = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = controlParametros,
        .user_ctx = NULL}; 

  httpd_uri_t uri_status = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL};

   httpd_uri_t uri_stream = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = Pag_Stream,
        .user_ctx = NULL};   

   // Iniciamos servidor puerto 8080
   config.server_port = 8080;
   log_i ("Iniciamos el servidor en el puerto %d",config.server_port);
   if(httpd_start(&camara_httpd , &config) == ESP_OK)  // Si la inicializacion es OK se registran las URis
   {
     httpd_register_uri_handler(camara_httpd , &uri_raiz);
     httpd_register_uri_handler(camara_httpd , &uri_flash);
     httpd_register_uri_handler(camara_httpd , &uri_control);
     httpd_register_uri_handler(camara_httpd , &uri_status);      
   }
   
   // Se inicia el servidor de streaming
   config.server_port += 1;
   config.ctrl_port += 1;
   log_i ("Iniciamos streaming en el puerto %d",config.server_port);
   
   if(httpd_start(&stream_httpd , &config) == ESP_OK)  // Si la inicializacion es OK se registran las URis
   {
     httpd_register_uri_handler(stream_httpd , &uri_stream);
   }
}

//  ======================= Funciones tratamientos de paginas (URI)
// ===  Funciones tratamiento de URIs
esp_err_t Pag_Raiz(httpd_req_t *req)   // Genera el Streaming
{
  /* Pagina con archivo en ZIP
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)Pagina_html_gz, Pagina_html_gz_len);
  */
  /* Pagina descomprimida */ 
   return httpd_resp_send(req, Pagina_html, HTTPD_RESP_USE_STRLEN); 
}

// Genera streaming
esp_err_t Pag_Stream(httpd_req_t *req)   // Genera el Streaming
{
   esp_err_t res = ESP_OK;
   Error_ESPCAM error=CAM_OK;

   res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);  //Definimos el tipo de contenido
   
   if(res != ESP_OK)  return res;
   httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
   httpd_resp_set_hdr(req, "X-Framerate", "60");

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
   return res;
}   

esp_err_t Pag_Foto(httpd_req_t *req)   // Genera foto
{
   Error_ESPCAM error=CAM_OK;

   error = Camara.Foto(procesa_foto, req) ;
   if (error != CAM_OK)
   {
     log_e("Error en el procesado de fotos %d",error);
     httpd_resp_send_408(req);
     return ESP_FAIL;
   }

   return ESP_OK;
}   


// controladores
esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
   char *buf = NULL;
   size_t buf_len = 0;

   buf_len = httpd_req_get_url_query_len(req) + 1;
   if (buf_len > 1) 
   {
     buf = (char *)malloc(buf_len);
     if (!buf) 
     {
       httpd_resp_send_500(req);
       return ESP_FAIL;
     }
     if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) 
     {
       *obuf = buf;
       return ESP_OK;
     }
     free(buf);
    }
    
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

// Activa o desactiva Flash
esp_err_t controlFlash(httpd_req_t *req)
{
   char *buf = NULL;
   char activar[5];
  
   if (parse_get(req, &buf) != ESP_OK) 
   {
     log_e("Fallo al analizar parametro de Flash");
     httpd_resp_send_500(req);
     return ESP_FAIL;
   }

   if (httpd_query_key_value(buf, "activa", activar, sizeof(activar)) != ESP_OK)
   {
     log_e("Fallo en los parametros");
     free(buf);
     httpd_resp_send_404(req);
     return ESP_FAIL;
   }
   free(buf);

   int val = atoi(activar);
   if(val == 0)  Camara.Flash(Apagado);
   else Camara.Flash(Medio);

   httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
   return httpd_resp_send(req, NULL, 0);
}

esp_err_t controlParametros(httpd_req_t *req)
{
 char *buf = NULL;
    char variable[32];
    char valor[32];

    if (parse_get(req, &buf) != ESP_OK) 
    {
       log_e("Fallo al analizar parametro de Parametros");
       return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", valor, sizeof(valor)) != ESP_OK) 
    {
       log_e("Fallo en los parametros");
       free(buf);
       httpd_resp_send_404(req);
       return ESP_FAIL;
    }
    free(buf);

    int val = atoi(valor);
  
    log_i("Se cambia %s-->%d",variable,val);
    if (!strcmp(variable, "framesize")) 
    {
       if (Camara.get_pixformat() == PIXFORMAT_JPEG) 
       {
           Camara.set_framesize((framesize_t)val);
       }
    }
    else if (!strcmp(variable, "calidad")) Camara.set_calidad(val);
    else if (!strcmp(variable, "contraste")) Camara.set_contraste(val);
    else if (!strcmp(variable, "brillo")) Camara.set_brillo(val);
    else if (!strcmp(variable, "saturacion")) Camara.set_saturacion(val);
    else log_e("Parametro desconocido: %s", variable);

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}



esp_err_t status_handler(httpd_req_t *req)
{
  static char json_respuesta[1024];

  char *p = json_respuesta;
  *p++ = '{';
  
  p += sprintf(p, "\"pixformat\":%u,", Camara.get_pixformat());
  p += sprintf(p, "\"framesize\":%u,", Camara.get_framesize());
  p += sprintf(p, "\"calidad\":%u,", Camara.get_calidad());
  p += sprintf(p, "\"brillo\":%d,", Camara.get_brillo());
  p += sprintf(p, "\"contraste\":%d,", Camara.get_contraste());
  p += sprintf(p, "\"saturacion\":%d", Camara.get_saturacion());

  *p++ = '}';
  *p++ = 0;

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_respuesta, strlen(json_respuesta));
}
