/********************************************************************************
*  Fco. Javier Rodriguez Navarro 
*  https://www.pinguytaz.net
*
*  Programa basico de visualización de camara y activacion flash
*
*  Historico:
*     - Junio 2025    V1: Creación 
********************************************************************************/
#include <opencv2/opencv.hpp>

#include <iostream>
#include <curl/curl.h>

// Callback para almacenar la respuesta en un buffer
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) 
{
    size_t total_size = size * nmemb;   // Calcula el tamaño de la respuesta
    //printf("Contenido completo de la respuesta\n %s\n",contents);

    memcpy(userp, contents, total_size);   // Copia solo respuesta
    return total_size;
}

int main(int argc, char** argv) 
{
   // definimos la URL donde emite la camara
   std::string url = "http://192.168.50.1:8081/stream";
   cv::VideoCapture cap(url,cv::CAP_ANY);
   cv::namedWindow("Video desde URL", cv::WINDOW_GUI_NORMAL | cv::WINDOW_AUTOSIZE);

   if (!cap.isOpened()) 
   {
       std::cerr << "Error: No se pudo abrir el stream de video desde la URL: " << url << std::endl;
       return -1;
   }

   cv::Mat frame;
   while (true) 
   {
       bool ret = cap.read(frame);  // Leer un frame
       if (!ret || frame.empty()) 
       {
          std::cerr << "Error: No se pudo leer frame o el stream terminó." << std::endl;
          break;
       }
       cv::imshow("Video desde URL", frame);
       char tecla= cv::waitKey(30) ;

       if (tecla == 'q') 
       {
            break;
       }
       else 
       {
          if (tecla == 'f') 
          {
             CURL *curl;
             CURLcode res;
             char url[256];

             char respuesta[4096] ; // Buffer para la respuesta
             long http_code = 0;

             snprintf(url, sizeof(url), "http://192.168.50.1:8080/flash");
             //snprintf(url, sizeof(url), "http://192.168.50.1:8080/efecto?valor=5");  // Prueba efecto a azul
            
             // Inicia librería y crea una sesión 
             curl_global_init(CURL_GLOBAL_DEFAULT);   
             curl = curl_easy_init();
             if(curl) 
             {
                curl_easy_setopt(curl, CURLOPT_URL, url);    // Indicamos URL
                // Permite que la respuesta se vaya almacenando en la variable respuesta
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);   // Definimos funcion callBack que recoge respuesta
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, respuesta);           // Define donde almacenar datos

                // Realizar la petición GET
                res = curl_easy_perform(curl);
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);   // Codigo respuesta HTTP

                // Comprobar errores
                if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                else    // Todo correcto se gestiona codigo y retorno.
                {
                   printf("Código HTTP: %ld\n", http_code);
                   printf("Respuesta: %s\n", respuesta);
                }

                curl_easy_cleanup(curl);
             } // If del Curl
             curl_global_cleanup();
          }  // If tecla f
       } // else
    }   // While
    cap.release();
    cv::destroyAllWindows();
    return 0;
}    // main

