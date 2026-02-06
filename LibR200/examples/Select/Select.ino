/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo de Pooling con SELECT con la libreria LibR200
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
  
  uint8_t mask[]={'G','a','r','u','m','3',0x00};
  Lector.setSelect(obSL, Coincide, BANK_EPC, 32, mask, 6, NoTrunca);

  t_Parametros parametros;
  parametros = Lector.getSelect();
  Serial.printf("Parametros:\n\tSelParam: Target->%02X Accion->%02X Banco memoria_>%02X\n",parametros.sel.campo.target,
                                                                         parametros.sel.campo.action,
                                                                         parametros.sel.campo.mem_bank);
  Serial.printf("\tPTR: %02X:%02X:%02X:%02X\n",parametros.ptr[0],parametros.ptr[1],parametros.ptr[2],parametros.ptr[3]);
  Serial.printf("\tMascara(%d) --> ",parametros.MaskLen/8);
  for(int pos=0; pos <parametros.MaskLen/8; pos++) Serial.printf("%0X(%c) ",parametros.Mask[pos],parametros.Mask[pos]);
  Serial.printf("\n\tTruncate: %02X\n",parametros.Truncate);

  Serial.println("***************************  INICIO todo filtro 0x00 ***************************");
  Lector.modoSelect(Siempre);   // 00 filtro para todo pooling y lecturas/escritura, el 02 es solo L/E
  busca();
  Serial.println("***************************  FIN todo filtro 0x00 ***************************");

  Serial.println("***************************  INICIO sin filtro 0x01 ***************************");
  Lector.modoSelect(Nunca);  
  busca();
  Serial.println("***************************  FIN todo filtro 0x01 ***************************"); 

  Serial.println("***************************  INICIO filtro solo L/E 0x02 ***************************");
  Lector.modoSelect(NoPooling);  
  busca();
  Serial.println("***************************  FIN todo filtro 0x02 ***************************");

  Serial.println("***************************  INICIO todo filtro 0x00 segunda vez***************************");
  Lector.modoSelect(Siempre);   // 00 filtro para todo pooling y lecturas/escritura, el 02 es solo L/E el 0x01 cancela
  busca();
  Serial.println("***************************  FIN todo filtro 0x00 segunda vez***************************");

  
}

void loop()
{

}

void busca()
{
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
      Serial.printf("\tRSSI: 0x%02X\n", etiquetas[i].RSSI);
      Serial.printf("\tPC: 0x%02X%02X\n",etiquetas[i].PC_msb, etiquetas[i].PC_lsb);
      Serial.print("\tEPC: 0x");
      for (int x=0; x<etiquetas[i].LongEPC; x++) Serial.printf("%02x(%c) ", etiquetas[i].Datos_EPC[x],etiquetas[i].Datos_EPC[x]);
      Serial.println("");
      Serial.printf("\tCRC: 0x%02X%02X\n", etiquetas[i].CRC_msb, etiquetas[i].CRC_lsb);
    }
  }
  // Liberamos el espacio de los EPC de las etiquetas
  for (int i =0; i<numEtiquetas; i ++) free(etiquetas[i].Datos_EPC);

  Serial.println("\n--------------------------------------------------------------");
}
