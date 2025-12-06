/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo de lectura de etiqueta mediante la libreria LibR200
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
  //Lector.insertCanales(0,canales);   // Limpia y pone todas en aleatorio.
  Lector.insertCanales(4,canales);
  Lector.hopping(true);
  Lector.transmisionContinua(false);

  // Firmware ideal  UCODE 9 Varias etiquetas Q4-6(16-64 slots)  Evita saturacion Session=1 Target=A
  Lector.set_Firmware(ALL,S1,A,5);    

  /* Parametros de modulación para U9
     Mixer Gain medio, IF Amp Gain alto, Threshold bajo para maximizar señales débiles sin saturación.
     Escenario          Mixer	  IF AMP	Threshold	  Ratio de lectura
     Multi UCODE9     	  9dB	    36dB      01B0	          98%       Nota. Bajar Threshold a 00F0 si pierde TAGy subir a 01C0 muchos falsos.
     Single tag fuerte  	6dB	    27dB	    02A0	          100%
     Tags débiles	        12dB    12dB	    00B0	          90%
     
     Mixer Gain Aumentalo mejora la sensibilidad del receptor al amplificar la señal RF entrante tempranamente (antes de la conversión a IF), 
                elevando señales débiles sobre el ruido del mixer y etapas posteriores.​
     IF_AMP valores medios (21-30 dB) optimizan multi-tags UCODE9 (98%) y bajos (12-18 dB) evitan saturación en tags fuertes. 
        Un IF_G más alto amplifica señales débiles, mejorando la capacidad del receptor "sensibilidad" para detectar tags con señal muy baja. 
        Pero muy alto puede saturar si hay señales fuertes produce saturación con tags fuertes.
        , o valores altos para máxima sensibilidad en entornos con tags débiles
    Threshold: umbral de detección RSSI en la etapa IF del receptor. (nivel mínimo de señal para reconocer un tag válido.
         Threshold bajo (ej. 00B0): Mayor sensibilidad, detecta tags débiles/distantes pero más falsos positivos/ruido.
         Threshold alto (ej. 02A0): Menos falsos positivos, ideal tags fuertes/cercanos pero pierde tags débiles.
         Unidad típica: Hex (00B0 ~ -80 dBm, 02A0 ~ -60 dBm estimado).
  */
  t_Demodulador demodulador;
  demodulador.campo.Mixer_G = MG_9;
  demodulador.campo.IF_G = IFG_36;
  demodulador.campo.Thrd = 0x01B0;
  Lector.set_Demodulador(demodulador);
  
  /*demodulador = Lector.get_Demodulador();
  Serial.println("Mixer Gain " + String(demodulador.campo.Mixer_G));
  Serial.println("IF AMP " + String(demodulador.campo.IF_G));
  Serial.printf("Threshold 0x%04X", demodulador.campo.Thrd);*/
  
  uint8_t clave[4] = {0x00,0x00,0x00,0x00};

  t_DatosTag dTag ;
  Lector.leeTAG(clave,BANK_EPC, 0x0002,0x0003, &dTag);
  Serial.printf("PC: 0x%04X", dTag.PC);
  Serial.printf(" EPC(%d) 0x: ", dTag.longEPC );
  for (int i = 0; i < dTag.longEPC ; i++) Serial.printf("%02X",dTag.Datos_EPC[i]);

  Serial.printf(" Datos(%d): ", dTag.longDatos );
  for (int i = 0; i < dTag.longDatos ; i++) 
  {
    if (dTag.Datos[i] >= 32 && dTag.Datos[i] <= 126) Serial.printf("%c ",dTag.Datos[i]);
    else Serial.printf("0x%02X ",dTag.Datos[i]);
  }
  Serial.println("");
  
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
      Serial.printf("\tPC: PC: 0x%02X%02X\n",etiquetas[i].PC_msb,etiquetas[i].PC_lsb);
      Serial.print("\tEPC: 0x");
      for (int x=0; x<etiquetas[i].LongEPC; x++) Serial.printf("%02X",etiquetas[i].Datos_EPC[x]);
      Serial.println("");
      Serial.printf("\tCRC: 0x%02X%02X\n",etiquetas[i].CRC_msb, etiquetas[i].CRC_lsb);
    }
  }
  // Liberamos el espacio de los EPC de las etiquetas
  for (int i =0; i<numEtiquetas; i ++) free(etiquetas[i].Datos_EPC);
}

void loop() 
{ 
  delay(5000);
}
