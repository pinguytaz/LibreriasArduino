# LibESPCAM (Libreria gestion de la camara ESP32)
[![license](https://www.pinguytaz.net/IMG_GITHUB/gplv3-with-text-84x42.png)](https://github.com/pinguytaz/Arduino-ESP32/blob/master/LICENSE)


__Libreria Lib_ESPCAM__
Actualizada a API V3 de ESP32  
  
Libreria para la gestion y programación de la placas con camaras de ESP32, en este momento se ha realizado para AI-Thinker con la camara OV264.  
  
  
El autor __NO asume ninguna responsabilidad__ por la forma en que elija utilizar cualquiera de los ejecutables/código fuente de cualquier archivo provistos. El autor y cualquier persona afiliada no serán responsables de ninguna pérdida o daño en relación con el uso de esta libreria o parte de su código.  
  
Usted ACEPTA USARLO BAJO SU PROPIO RIESGO.  
  
##Uso de LibESPCAM
Libreria que  facilita la programación del modulo con Camara "ESP32-CAM", lo primero que deberemos de hacer es crear la clase y despues inicializarla "iniciaCAM".  
  
Para obtener una captura bastara con invocar al metodo "Foto" al que le pasaremos la función que tratara esta captura y si necesita parametro extra.  
**Prototipo de funcion de tratamiento**  
  
Error_ESPCAM procesa_imagen(uint8_t *buf, size_t tamano, size_t ancho, size_t alto, pixformat_t formato, struct timeval tiempo, void *parametros)  
  
funciones para cambiar 'set_' y recuperar 'get_' configuraciones  
get_pixformat   Formato, solo lectura        
xxx_framesize    Tamaño de la imagen           
xxx_calidad      Calidad de la imagen 0 -63    
xxx_brillo       Brillo  -2 - 2                
xxx_contraste    Contraste -2 - 2              
xxx_saturacion   Saturación -2 - 2             
      

**Métodos LibESPCAM**  
  
|  Funcion|Descripción                                                                                                              |
|---------------------------------------------------|-------------------------------------------------------------------------------|
| **Flash(Nivel_Flash)**                            | Activa o desactiva el flash al nivel que definamos: Alto, Medio, Bajo, Apagado|
| **ledRojo(bool)**                                 | Enciende o apaga el led rojo de la placa.                                     |
| **Foto(pf_timagen funcion, void parametros=NULL)**| Realizamos foto y se pasa como parámetro: función a la que se llama, parámetros extras que se pasán a la función.|
| **pixformat_t get_pixformat()**                   | Formato de los datos,  sera JPEG ya que se pone fijo en la clase |
| **get_camara()**                                  | Retorna el modelo de la camara                                 |
| **get_soporta_jpeg()**                            | Indica si soporta o no JPEG                                    |
| **get_framesize()**                               | Tamaño de la imagen |
| **get_calidad()**                                 | Calidad de la imagen un número de 0-63, a menor más calidad |
| **get_brillo()**                                  | 	Brillo entre -2 y 2 |
| **get_contraste()**                               | Contraste entre -2 y 2 |
| **get_saturacion()**                              | Saturación entre -2 y 2 |
| **get_efectoEspecial()**                          | Efecto especial 0 - 6 |
| **get_espejoV()**                                 | Espejo Vertical 0-NO y 1-SI |
| **get_espejoH()**                                 | Espejo Horizontal 0-NO y 1-SI |
| **set_framesize(framesize_t)**                    | Configura el tamaño de la imagen |
| **set_calidad(uint8_t)**                          | Configura la calidad de la imagen |
| **set_brillo(int8_t)**                            | Configura el brillo |
| **set_contraste(int8_t)**                         | Configura el contraste |
| **set_saturacion(int8_t)**                        | Configura la saturación |
| **set_carta(bool)**                               | Pone o quita la carta de ajuste de la cámara. |
| **set_espejoV(bool)**                             | Realiza o no el espejo Vertical |
| **set_espejoH(bool)**                             | Realiza o no el espejo Horizontal |
| **set_efectoEspecial(int8_t)**                    | 0 – Sin efecto 1 – Negativo 2 – Escala de grises 3 – Rojo 4 – verde 5 – Azul 6 – Sepia |  
  
<BR>  
Tenemos varios ejemplos de su uso:  
  
- **Ej_Fotos** Realiza una foto y la guarda en la tarjeta que dispone el dispositivo.  
- **Ej_FotoWeb** Nos permite tormar fotos y verlas en la WEB.  
- **Ej_StreamWeb** Ejemplo de desarrollo de un streaming en WEB.  
- **Ej_CamParam** Ejemplo de las modificación de parametros y un streaming para ver los cambios.  
- **Camara** Ejemplo simple de servidor de streamieng con libreria WebServer  
- **CamaraIP** Ejemplo con la libreria *esp_http_server* ademas de streaming tiene comandos y es el utilizado en ejemplos openCV  
- **openCV** Ejemplos para generar aplicaciones de escritorio 'C' y en python  
  
<BR>  
__Website__: https://www.pinguytaz.net
