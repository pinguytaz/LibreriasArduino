/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: CamaraIP con AI Thinker ESP32-CAM
 *      Streaming  http://192.168.50.1:8081/stream
 *      Comandos  http://192.168.50.1:8080/<comando>
 *               
 ********************************************************************************************/
#include <LibESPCAM.h>    // Cabecera de funciones

#include <WiFi.h>                 // Config. WIFI
#include <esp_http_server.h>

// ======================================  Constantes para Streaming  =============================
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
const char *clave = "LaClaveLarga";

// Creamos clase de camara optimizada para streaming
/* Optimizado para transmisión de video
    calidad JPEG entre 10 y 15 y se podra cambiar con set_calidad()
    Tamaño del Frame VGA(640x480 o QVGA(320x240) que podremos cambiar con set_framesize(FRAMESIZE_VGA | FRAMESIZE_QVGA)
    Buffer 2 mejora fluidez (maximiza los FPS realies) y mejora FPS y aprobecha PSRAM sin llegar a agotarla.
    Modo grabacion: CAMERA_GRAB_LATEST para priorizar el frame más reciente.
*/
ESPCAM Camara(12, FRAMESIZE_VGA, 2, CAMERA_GRAB_LATEST );

// **************   Función callBack que procesa el Frame
Error_ESPCAM procesa_frame(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   uint8_t *bufjpg = NULL;
   size_t tamanojpg = 0;
   esp_err_t res = ESP_OK;
   char * part_buf[64];
   httpd_req_t *req = (httpd_req_t *)parametros;

   if (formato != PIXFORMAT_JPEG) // Debemos convertir a JPEG si no esta en el formato
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
   }
   IPAddress    apIP(192, 168, 50, 1);
   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));  
   WiFi.softAP(ssid, clave);  
   // Optimización del WIFI para la emisión de streaming
   WiFi.setSleep(false);  // Evitamos que se duerma el WiFi, para optimización
   //WiFi.setTxPower(WIFI_POWER_19_5dBm);    // Máxima potencia (19.5 dBm)  19.5 dBm * 4 = 78 (en unidades de 0.25 dBm)  NO con Flahs por consumo
   
   WiFi.setTxPower(WIFI_POWER_8_5dBm);  // 8.5 dBm (~80 mA)    Minima si lo tenemos al lado para reducir consumo
   // Verificamos la potencia
   int8_t current_power = WiFi.getTxPower();
   Serial.print("Potencia actual: ");
   Serial.println(current_power);  // Puede no coincidir con lo configurado


   IPAddress miIP = WiFi.softAPIP();   // Obtenemos la IP del AP
   Serial.print("Estacion conectada en IP del AP: ");
   Serial.println(miIP);

   // Configuramos los servidores WEB
   configuraServidorStream();    // 8081 el Streaming
   configuraServidorCtr();       // 8080 control de la camara.
}

/////////////////     Bucle
void loop() 
{
   delay(10000);
}

//  ***** Configuracion del servidores
void configuraServidorStream(void)
{
   httpd_config_t config_stream = HTTPD_DEFAULT_CONFIG();
   //config_stream.max_uri_handlers = 1;   // Solo tendremos el Streaming y robots
   config_stream.server_port = 8081;
   config_stream.stack_size = 6144;
   config_stream.max_open_sockets = 3;  // Para múltiples clientes de stream
   config_stream.task_priority = 5;      // Ponemos prioridad 5 a más alta mejor respuesta pero más recursos.
   config_stream.ctrl_port += 1;   

   //*********************** Definicion URIs
   httpd_uri_t uri_robots = {
        .uri = "/robots.txt",
        .method = HTTP_GET,
        .handler = robots,
        .user_ctx = NULL};   

   httpd_uri_t uri_stream = {
       .uri = "/stream",
       .method = HTTP_GET,
       .handler = Pag_Stream,
       .user_ctx = NULL};   

   httpd_handle_t server_stream = NULL;
   if( httpd_start(&server_stream, &config_stream) == ESP_OK)  // Si se inicia OK se registran las URIs
   {
       httpd_register_uri_handler(server_stream , &uri_robots);
       httpd_register_uri_handler(server_stream , &uri_stream);

     // Manipuladores de error
     httpd_register_err_handler(server_stream, HTTPD_404_NOT_FOUND, errorPagina);
   }
}

void configuraServidorCtr()
{
   httpd_config_t config_ctr = HTTPD_DEFAULT_CONFIG();
   config_ctr.max_uri_handlers = 15;
   config_ctr.server_port = 8080;
   config_ctr.stack_size = 4096;  
   
   //*********************** Definicion URIs
   httpd_uri_t uri_robots = {
        .uri = "/robots.txt",
        .method = HTTP_GET,
        .handler = robots,
        .user_ctx = NULL};   

   httpd_uri_t uri_raiz = {
       .uri = "/",
       .method = HTTP_GET,
       .handler = PagRaiz,
       .user_ctx = NULL};   
  
   httpd_uri_t uri_flash = {
       .uri = "/flash",
       .method = HTTP_GET,
       .handler = Flash,
       .user_ctx = NULL};   

   httpd_uri_t uri_status = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_control = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = controlParametros,
        .user_ctx = NULL}; 




   httpd_uri_t uri_carta = {
       .uri = "/carta",
       .method = HTTP_GET,
       .handler = Carta,
       .user_ctx = NULL};   

   httpd_uri_t uri_espejoH = {
       .uri = "/espejoH",
       .method = HTTP_GET,
       .handler = espejoH,
       .user_ctx = NULL};   

  httpd_uri_t uri_espejoV = {
       .uri = "/espejoV",
       .method = HTTP_GET,
       .handler = espejoV,
       .user_ctx = NULL};   
   
   httpd_handle_t server_ctr = NULL;
   if( httpd_start(&server_ctr, &config_ctr) == ESP_OK)  // Si se inicia OK se registran las URIs
   {
     httpd_register_uri_handler(server_ctr , &uri_robots);
     httpd_register_uri_handler(server_ctr , &uri_raiz);
     httpd_register_uri_handler(server_ctr , &uri_flash);
     httpd_register_uri_handler(server_ctr , &uri_status);
     httpd_register_uri_handler(server_ctr , &uri_control);
     httpd_register_uri_handler(server_ctr , &uri_carta);
     httpd_register_uri_handler(server_ctr , &uri_espejoH);
     httpd_register_uri_handler(server_ctr , &uri_espejoV);
     
     // Manipuladores de error
     httpd_register_err_handler(server_ctr, HTTPD_404_NOT_FOUND, errorPagina);
   }
}

//*********************+ Manipuladores de paginas
esp_err_t robots(httpd_req_t *req)
{
   const char* respuesta = "User-Agent: *\nDisallow: /\n";
   // Establece el tipo de contenido como texto plano ya que es el fichero robots.txt
   httpd_resp_set_type(req, "text/plain");
   // Enviar la respuesta
   return httpd_resp_send(req, respuesta, HTTPD_RESP_USE_STRLEN); 
}

esp_err_t PagRaiz(httpd_req_t *req)
{
   
   const char* respuesta = "Camara IP\n---------\n:8081/stream\nComandos:\n"
                            "/flash\t Enciende o apaga el Flash\n"
                            "/carta\t  Pone carta de ajuste \n";
   // Establece el tipo de contenido como texto plano, no queremos HTML ya que se puede usar como ayuda en clientes ¿JSON podria ser?
   httpd_resp_set_type(req, "text/plain");
   // Enviar la respuesta
   return httpd_resp_send(req, respuesta, HTTPD_RESP_USE_STRLEN); 
}

/*********************+ Flash *************/
esp_err_t Flash(httpd_req_t *req)
{
   static bool encendido = false;

   if (encendido)   // Esta encendido luego se apaga
   {
       Camara.Flash(Apagado);
       encendido = false;
   }
   else
   {
       Camara.Flash(Medio);
       encendido = true;
   }
    
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);

   //httpd_resp_set_type(req, "text/plain");
   //return httpd_resp_send(req, "OK-FLASH", HTTPD_RESP_USE_STRLEN); 
}

/********************** Status valores ***********************/
esp_err_t status_handler(httpd_req_t *req)
{
  static char json_respuesta[1024];

  char *p = json_respuesta;
  *p++ = '{';
  
  //p += sprintf(p, "\"pixformat\":%u,", Camara.get_pixformat());
  //p += sprintf(p, "\"framesize\":%u,", Camara.get_framesize());
  p += sprintf(p, "\"calidad\":%u,", Camara.get_calidad());
  p += sprintf(p, "\"brillo\":%d,", Camara.get_brillo());
  p += sprintf(p, "\"contraste\":%d,", Camara.get_contraste());
  p += sprintf(p, "\"saturacion\":%d,", Camara.get_saturacion());
  p += sprintf(p, "\"efectoespecial\":%d,", Camara.get_efectoEspecial());
  p += sprintf(p, "\"espejov\":%d,", Camara.get_espejoV());
  p += sprintf(p, "\"espejoh\":%d", Camara.get_espejoH());


  *p++ = '}';
  *p++ = 0;

 Serial.println(p);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_respuesta, strlen(json_respuesta));
}

/**************  Cambio valores   ***********/
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
  /*  if (!strcmp(variable, "framesize")) 
    {
        Camara.set_framesize((framesize_t)val);
    }
    else */
    if (!strcmp(variable, "calidad")) Camara.set_calidad(val);
    else if (!strcmp(variable, "contraste")) Camara.set_contraste(val);
    else if (!strcmp(variable, "brillo")) Camara.set_brillo(val);
    else if (!strcmp(variable, "saturacion")) Camara.set_saturacion(val);
    else if (!strcmp(variable, "efectoespecial")) Camara.set_efectoEspecial(val);
    
    
    else log_e("Parametro desconocido: %s", variable);

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}


esp_err_t espejoH(httpd_req_t *req)
{
   static bool estado = false;

   if (estado)   // Esta encendido luego se apaga
   {
       Camara.set_espejoH(false);
       estado = false;
   }
   else
   {
       Camara.set_espejoH(true);
       estado = true;
   }

   
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);

   //httpd_resp_set_type(req, "text/plain");
   //return httpd_resp_send(req, "OK-ESPEJOH", HTTPD_RESP_USE_STRLEN); 
}

esp_err_t espejoV(httpd_req_t *req)
{
   static bool estado = false;

   if (estado)   // Esta encendido luego se apaga
   {
       Camara.set_espejoV(false);
       estado = false;
   }
   else
   {
       Camara.set_espejoV(true);
       estado = true;
   }

   
   httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
   return httpd_resp_send(req, NULL, 0);
   //httpd_resp_set_type(req, "text/plain");
   //return httpd_resp_send(req, "OK-ESPEJOV", HTTPD_RESP_USE_STRLEN); 
}


esp_err_t Carta(httpd_req_t *req)
{
   //const char* respuesta = "/Flash\t Enciende el Flash\n/Carta/t  Pone carta de ajuste \n";
   static bool estado = false;

   if (estado)   // Esta encendido luego se apaga
   {
       Camara.set_carta(false);
       estado = false;
   }
   else
   {
       Camara.set_carta(true);
       estado = true;
   }

   httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
   return httpd_resp_send(req, NULL, 0);
   //httpd_resp_set_type(req, "text/plain");
   //return httpd_resp_send(req, "OK-CARTA", HTTPD_RESP_USE_STRLEN); 
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


// Gestion de error de paginas 
esp_err_t errorPagina(httpd_req_t *req, httpd_err_code_t err)
{
  // Si tenemos error 404 se reenvia la principal de comandos
  if(err == HTTPD_404_NOT_FOUND)
  {
     httpd_resp_set_status(req, "302 Found");
     httpd_resp_set_hdr(req, "Location", "http://192.168.50.1:8080");
     httpd_resp_send(req, NULL, 0); 
     return ESP_OK;
  }
  else
  {
     // Si el error que llega es el 404   
     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error en servidor");
     return ESP_FAIL; // O ESP_OK si quieres mantener la conexión
  }
}

/*
void controlFlash()   // /flash sin parametro apaga, parametro nivel= nivel flash
{ 
   if (camServ.hasArg("nivel") == 0)  Camara.Flash(Apagado);  
   else
   {
     String nivel = camServ.arg("nivel");
     if (nivel == "Alto") Camara.Flash(Alto);
     else if (nivel == "Medio") Camara.Flash(Medio);
     else if (nivel == "Bajo") Camara.Flash(Bajo);
     else Camara.Flash(Apagado);  
   }
   camServ.send(200, "text/html", "Nivel de Flash");

}

*/

