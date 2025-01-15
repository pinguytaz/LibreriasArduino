/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo AI Thinker ESP32-CAM
 *               Genera Página WEB para poder tomar fotos y verla en
 ***********************************************************************************************/
#include "LibESPCAM.h"    // Cabecera de funciones
#include <WebServer.h>
 //#include "esp_http_server.h"

#include <WiFi.h>


// ================================
// Credenciales de nuestra red WIFI de nuestro AP para que se conecten otros
// ================================
const char *ssid = "LaCamara";
const char *clave = "LaClave-debeserlarga";

//ESPCAM Camara;
ESPCAM Camara(12);  // Definimos una calidad JPEG de 12
//ESPCAM Camara(FRAMESIZE_SVGA);   // Maximo framesize_t si se desea convertir a BMP

WebServer servidor(8080);  // Servidor WEB que solo permite una llamada.

// Funciones que procesan las imagenes de fotos capturadas
Error_ESPCAM procesa_imagen(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)
{
   WiFiClient cliente = servidor.client();

   String respuesta = "HTTP/1.1 200 OK\r\n";
   
   // Retorna la foto en JPEG
   respuesta += "Content-disposition: inline; filename=Foto.jpg\r\n";
   respuesta += "Content-type: image/jpeg\r\n\r\n";
   servidor.sendContent(respuesta);
   cliente.write(buf,tamano);

   // Retorna la foto en BMP, debemos acordarnos de crear la clase con un maximo de 800x600 en framesize
   /*
   uint8_t *bufbmp = NULL;
   size_t tamanobmp = 0;
   //bool converted = frame2bmp(fb, &bufbmp, &tamanobmp);
   bool convertido = fmt2bmp(buf, tamano, ancho, alto, formato, &bufbmp, &tamanobmp);
      
   respuesta += "Content-disposition: inline; filename=Foto.bmp\r\n";
   respuesta += "Content-type: image/x-windows-bmp\r\n\r\n";
   servidor.sendContent(respuesta);
   cliente.write(buf,tamano);
*/
 
   return CAM_OK;
}

// Funcion que captura evento de conexion de una estación
void WiFiConectaEstacion(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Se conecta una estación al AP.");
  Serial.println("Estaciones conectadas: " + String(WiFi.softAPgetStationNum())); 
}

void setup() 
{
   Serial.begin(115200);
   
   // Configuramos la Camara
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


   //Configuramos funciones de paginas y recursos e iniciamos el servidor WEB
   servidor.onNotFound(PagNoEncontrada);  // Solicitud no encontrada es el error 404.
   //servidor.on("/", PagRaiz);  // Pagina principal
   servidor.on("/", HTTP_GET, PagRaiz);
   /*[] {
    server.setContentLength(sizeof(FRONTPAGE));
    server.send(200, "text/html");
    server.sendContent(FRONTPAGE, sizeof(FRONTPAGE));
  });
  */
   servidor.on("/Foto", TomaFoto);

   servidor.begin();  // Iniciamos el Servidor WEB

}

void loop() 
{
   servidor.handleClient();
   delay(2);
}


//==================0 Funciones de paginas
void PagNoEncontrada()   // Error 404 Pagina no encontrada
{
   String texto = "Página no encontrada\n\n";
   texto += "URI: ";
   texto += servidor.uri();
   texto += "\nMethod: ";
   texto += (servidor.method() == HTTP_GET) ? "GET" : "POST";
   texto += "\nArguments: ";
   texto += servidor.args();
   texto += "\n";
   servidor.send(404, "text/plain", texto);
}

void PagRaiz()   // RAIZ
{
   // Cabecera de todas las paginas WEB
   String index_html = R"rawliteral(
   <!DOCTYPE HTML><html>
     <head>
         <meta charset=\"utf-8\">
         <meta name="viewport" content="width=device-width, initial-scale=1">
         <style>
           body { text-align:center; }
           .vert { margin-bottom: 10%; }
           .hori{ margin-bottom: 0%; }
         </style>
     </head>
     <title>Servidor de camara</title>
     <body>
       <p>Ejemplo Foto a WEB con ESP32-CAM.</p>
       <p><button onclick="location.reload();">Actualiza Foto</button>
    </p>
  )rawliteral";

  // Generamos información de camara y sensor.
  

  index_html = index_html+ R"rawliteral(
  <div><img src="Foto" id="laFoto" width="70%"></div>
  </body>
  </html>)rawliteral";
   servidor.send(200, "text/html", index_html);
}

void TomaFoto() // Toma la foto
{
   Camara.Foto(procesa_imagen); // Procesa imagen en JPEG
}
