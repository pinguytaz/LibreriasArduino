/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo que busca etiquetas a su alcance
 *               mediante la libreria LibR200
 ********************************************************************************************/
// INCLUDES
#include <HardwareSerial.h>
#include <LibR200.h>

// Maximo de etiquetas a leer
#define MAX_TAG 10

 // GLOBALS
HardwareSerial Puerto(2); // definir un Serial para UART2
R200 Lector(&Puerto);

void setup() 
{
  // Inicializa Serial para debug 
  Serial.begin(115200);
  
  Lector.iniciaR200();
  
  // Definimos Reigion Europa y potencia a 20dB
  Lector.set_Region(EU);   
  Lector.set_Potencia(20);   
  // Firmware
  //Lector.set_Firmware(ALL,S0,A,4);  

  
}

void loop() 
{
  // Realizamos un Pool para ver cuantas etiquetas estan en el alcance
  t_Tag etiquetas[MAX_TAG];
  int numEtiquetas = Lector.simplePool(etiquetas, MAX_TAG);

  t_CodError error = Lector.get_Error();
  if (error != ERR_NINGUNO && error !=ERR_FALLO_INVENTARIO)  Serial.println("COD. ERROR: " + String(error));
  else if (error == ERR_FALLO_INVENTARIO ) Serial.println("No hay etiquetas al alcance");
  else
  {
    Serial.println("Se han leido "+String(numEtiquetas)); 
    for (int i =0; i<numEtiquetas; i ++)
    {
      Serial.println("Etiqueta: " + String(i+1));
      Serial.println("\tRSSI: 0x" + String(etiquetas[i].RSSI,HEX));
      Serial.println("\tPC: 0x"+ String(etiquetas[i].PC_msb,HEX) + String(etiquetas[i].PC_lsb,HEX));
      Serial.print("\tEPC: 0x");
      for (int x=0; x<etiquetas[i].LongEPC; x++) Serial.print(etiquetas[i].Datos_EPC[x],HEX);
      Serial.println("");
      Serial.println("\tCRC: 0x"+ String(etiquetas[i].CRC_msb,HEX) + String(etiquetas[i].CRC_lsb,HEX));
    }
  }
  // Liberamos el espacio de los EPC de las etiquetas
  for (int i =0; i<numEtiquetas; i ++) free(etiquetas[i].Datos_EPC);

  Serial.println("--------------------------------------------------------------");
  delay(5000);
}