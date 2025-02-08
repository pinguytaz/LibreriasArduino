# LibESPCAM (Libreria gestion de la camara ESP32)
[![license](https://www.pinguytaz.net/IMG_GITHUB/gplv3-with-text-84x42.png)](https://github.com/pinguytaz/Arduino-ESP32/blob/master/LICENSE)


__Libreria Lib_ESPCAM__
Actualizada a API V3 de ESP32

Libreria para la gestion y programación de la placas con camaras de ESP32, en este momento se ha realizado para AI-Thinker con la camara OV264.

En una primera version nos permite capturar imagenes y nos permitira llevarlas a un fichero, descargarlas a una BBDD o generar un servidor WEB de forma sencilla. Tambien se podran realizar Streaming.

Los parametros a cambiar para la configuración estan en este momento reducidos a: calidad, brillo, contraste y saturación, y se iran ampliando según necesidades, pero es bastante sencillo ampliarlos.

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
      

Funciones que sera implementando segun necesidades
set_special_effect()	
   0 – No Effect  
   1 – Negative  
   2 – Grayscale  
   3 – Red Tint  
   4 – Green Tint  
   5 – Blue Tint  
   6 – Sepia  
set_whitebal()	Balance de blancos  0 – disable  1 – enable  
set_awb_gain()	Ganancia de blancos 0 – disable  1 – enable  
set_wb_mode()	Modo balance de blancos  
	0 – Auto  
    1 – Sunny  
    2 – Cloudy  
    3 – Office  
    4 – Home  
set_exposure_ctrl()	contral de exposición  0 – disable  1 – enable  
set_aec2()		  0 – disable   1 – enable  
set_ae_level()	  -2 to 2  
set_aec_value()		0 to 1200  
set_gain_ctrl()  Control de ganancia  0 – disable  1 – enable  
set_agc_gain()		0 to 30  
set_gainceiling()		0 to 6  
set_bpc()	0 – disable  1 – enable  
set_wpc()		0 – disable  1 – enable  
set_raw_gma()	0 – disable  1 – enable  
set_lenc()	Set lens correction  	0 – disable   1 – enable  
set_hmirror()	Horizontal mirror	0 – disable   1 – enable  
set_vflip()	Vertical flip	0 – disable   1 – enable  
set_dcw()		0 – disable     1 – enable  
set_colorbar()	Set a colorbar	0 – disable  1 – enable  
  
  

Tenemos varios ejemplos de su uso:
**Ej_Fotos** Realiza una foto y la guarda en la tarjeta que dispone el dispositivo.
**Ej_FotoWeb** Nos permite tormar fotos y verlas en la WEB.
**Ej_StreamWeb** Ejemplo de desarrollo de un streaming en WEB.
**Ej_CamParam** Ejemplo de las modificación de parametros y un streaming para ver los cambios.


__Website__: https://www.pinguytaz.net
