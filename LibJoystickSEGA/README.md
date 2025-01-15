# LibJoystickSEGA (Libreria control mando SEGA de 6 botones.)
[![license](https://www.pinguytaz.net/IMG_GITHUB/gplv3-with-text-84x42.png)](https://github.com/pinguytaz/Arduino-ESP32/blob/master/LICENSE)
  
  
__Libreria LibJoystickSEGA__

Mos permite leer los mandos SEGA de 6 botones con el conector DB9.

##Instalación  

1. Download LibJoystickSEGA.zip  
2. Abre el IDE de Arduino  
3. "Sketch > Include Library > Add .ZIP Library..."  
4. Selecciona el archivo ZIP descargado.  
  
##Descripción del conector DB9
    1 2 3 4 5
     6 7 8 9

1   Arriba/Z  
2   Abajo/Y  
3   Izq./X  
4   Der./Mod  
5   Alimentación +5V  
6   A/B  
7   Select    Entrada que va realizando los cambios de estado  
8   GND  
9   Start/C   

## Ejemplos ##
    PruebasJoystick:   Nos permite probar que todos los botones del mando funcionan, a la vez que vemos como se usa la libreria.
    PruebasJoystick2:  Ejemplo usando dos mandos simultaneamente.
  
##Uso de LibJoystickSEGA
1. Deberemos crear una función CallBack que es donde llegaran los cambios de los botones del mando  
   Prototipo de la función "void nombre(BTN, boolean)  
2. Realizamos la instancia a la clase  
3. En la función setup debemos inicializar la clase con los pines, siendo el primer parametro el pin de SELECT.  
   iniciaJoystick(pin7,pin1,pin2,pin3,pin4,pin6,pin9)  
4. En loop llamaremos al metodo de lectura de botones pasandole el nombre de la funcion creada en el punto1.  
   leeBotones(funcion)  


<BR><BR>
__Website__: https://www.pinguytaz.net
