/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo de escritura de etiqueta mediante la libreria LibR200
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
    /* Frecuencia en Hopping recomendadas para Europa (MHz)	Potencia Máx ERP	Uso Típico
    4	865.7	   ​7	866.3    10	866.9	   ​13	867.5	   */
  uint8_t canales [4] = {4, 7, 10, 13};
  Lector.insertCanales(4,canales);
  Lector.hopping(true);
  Lector.transmisionContinua(false);
  Lector.set_Firmware(ALL,S1,A,5);    
  t_Demodulador demodulador;
  demodulador.campo.Mixer_G = MG_9;
  demodulador.campo.IF_G = IFG_36;
  demodulador.campo.Thrd = 0x01B0;
  Lector.set_Demodulador(demodulador);

  // Realizamos la escritura de la tarjeta en EPC desde la palabra 2 y 3 palabras(2bytes)  
  uint8_t clave[4] = {0x00,0x00,0x00,0x00};
  t_DatosTag dTag ;

  if (Lector.leeTAG(clave,BANK_EPC, 0x0002, 0x0004, &dTag))
  {
    impTagLeida(dTag);
    uint8_t nombre[]={'G','a','r','u','m','3',0x00,0x00};
    Lector.escribeTAG(clave,BANK_EPC, 0x0002,0x0004, nombre, &dTag);
    if (Lector.leeTAG(clave,BANK_EPC, 0x0002,0x0004, &dTag)) impTagLeida(dTag);
  }
  else Serial.println("****Error al leer ETIQUETA *****");
}

void loop() 
{
  delay(5000);
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