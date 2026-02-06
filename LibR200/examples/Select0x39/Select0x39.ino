/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo de lectura 0x39 mediante Select con la libreria LibR200
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
  Lector.set_Region(EU);   
  Lector.set_Potencia(26);   
  Lector.set_Firmware(ALL,S0,A,4);  
  uint8_t canales [4] = {4, 7, 10, 13}; // Frecuencia en Hopping recomendadas para Europa (MHz)
  Lector.insertCanales(4,canales);
  Lector.hopping(true);
  Lector.transmisionContinua(false);   // Lectura continua
  t_Demodulador demodulador;
  // Ajusta Thrd en pasos de 32–64 (0x0020)  Mixer_G/IF_G en pasos de 1–2 niveles
  demodulador.campo.Mixer_G = MG_9;  // Ganancia, muy alto(15,16) produciria interferencias y bajo(0-3) podrian dar problemas si se alejan.MG_9
  demodulador.campo.IF_G = IFG_36;   // Ganancia _36 media mejora relacion señal/ruido
  demodulador.campo.Thrd = 0x0180;   // Umbral de decisión 0x01B0 exigente, Permite ver etiquetas con peor RSSI
                                      // Si no lee todas 0x0160 o 0x0140  si duplica  0x01A0 o 0x01C0 y bajar MG y IFG
  Lector.set_Demodulador(demodulador); 
}

void loop()
{ 
  // Realizamos una lectura dcampo de memoria EPC
  uint8_t clave[4] = {0x00,0x00,0x00,0x00};
  t_DatosTag dTag ;

  // Eliminamos Filtro
  uint8_t mask[]={'G','a','r','u','m',0x00};
  Lector.setSelect(obSL, Coincide, BANK_EPC, 32, mask, 5, NoTrunca);
  Serial.println("************************************** Solo lectura");
  Lector.modoSelect(NoPooling); 
  busca();     // Pooling
  if(Lector.leeTAG(clave,BANK_EPC, 0x0000, 0x0006, &dTag))  {  impTagLeida(dTag);  }
  else { Serial.println("No localizada"); }

  
  Serial.println("************************************** No Filtra Nada");
  Lector.modoSelect(Nunca); // 0x00 Todo, 0x01 Nada, 0x02 Solo L/E.
  busca();     // Pooling
  if(Lector.leeTAG(clave,BANK_EPC, 0x0000, 0x0006, &dTag))  {  impTagLeida(dTag);  }
  else { Serial.println("No localizada"); }

  Serial.println("************************************* Filtra pooling y lectura 0x39");
  Lector.modoSelect(Siempre); 
  busca();     // Pooling
  if(Lector.leeTAG(clave,BANK_EPC, 0x0000, 0x0006, &dTag))  {  impTagLeida(dTag);  }
  else { Serial.println("No localizada"); }
  
  delay(8000);
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

void impTagLeida(t_DatosTag dTag)
{
  Serial.printf("PC: 0x%04X", dTag.PC);
  Serial.printf(" EPC(%d) 0x: ", dTag.longEPC );
  for (int i = 0; i < dTag.longEPC ; i++) Serial.printf("%02X",dTag.Datos_EPC[i]);

  if (dTag.bancoMemoria == BANK_RFU || dTag.bancoMemoria == BANK_TID)
  {
    if (dTag.bancoMemoria == BANK_RFU) Serial.printf(" Datos Reservados(%d): ", dTag.longDatos );
    else Serial.printf(" Datos TID(%d): ", dTag.longDatos );
    for (int i = 0; i < dTag.longDatos ; i++) Serial.printf("0x%02X ",dTag.Datos[i]);
  }
  else
  {
    if (dTag.bancoMemoria == BANK_EPC) Serial.printf(" Datos EPC(%d): ", dTag.longDatos );
    else Serial.printf(" Datos USER(%d): ", dTag.longDatos );
    for (int i = 0; i < dTag.longDatos ; i++) 
    {
      if (dTag.Datos[i] >= 32 && dTag.Datos[i] <= 126) Serial.printf("0x%02X(%c) ",dTag.Datos[i],dTag.Datos[i]);
      else Serial.printf("0x%02X ",dTag.Datos[i]);
    }
  }
  Serial.println("");
}