/*******************************************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Ejemplo que verifica que hemos conectado el lector R200-UHF, 
 *                dandonos datos como ID_Modulo, Region, Canal, Potencia y firmware localiza
 *               mediante la libreria LibR200
 ********************************************************************************************/
// INCLUDES
#include <HardwareSerial.h>
#include <LibR200.h>

 // GLOBALS
HardwareSerial Puerto(2); // definir un Serial para UART2
R200 Lector(&Puerto);

void setup() 
{
  // Inicializa Serial para debug 
  Serial.begin(115200);
  
  Lector.iniciaR200();
  
  // Obtenemos información del hardware
  t_Hardware infoModulo;
  if(Lector.hw_info(&infoModulo)) 
  {
    Serial.println("Información Hardware");
    Serial.print("SW: ");
    Serial.println(infoModulo.vSW);
    Serial.print("HW: ");
    Serial.println(infoModulo.vHW);
    Serial.print("Fabricante: ");
    Serial.println(infoModulo.fabricante);
  }
  else Serial.println("Comando incorrecto");
  
  // Region
  //Lector.set_Region(EU);   // Pone region de europa
  t_Region region = Lector.get_Region();
  Serial.print("Region: ");
  switch(region)
  {
    case Desconocido:
      Serial.println("Desconocido, posible error de parametro");
      break;
    case China_900:
      Serial.println("China 900Mhz");
      break;
    case US:
      Serial.println("Estados Unidos");
      break;
    case EU:
      Serial.println("Europa");
      break;
    case China_800:
      Serial.println("China 800Mhz");
      break;
    case Korea:
      Serial.println("Korea");
      break;
   default:
      Serial.println("Valor desconocido");
      break;
  } 

  // Canal
  //Lector.set_Canal(4);   
  float canal = Lector.get_Canal();
  Serial.println("Canal: " + String(canal) + " Mhz");

  // Potencia
  //Lector.set_Potencia(20);   
  int potencia = Lector.get_Potencia();
  Serial.println("Potencia: " + String(potencia) + " dBm");

  // Firmware
  //Lector.set_Firmware(ALL,S0,A,4);  
  Serial.println("Firmware: ");
   t_Firmware firmware = Lector.get_Firmware();
  if (firmware.campo.DR == 0) Serial.println("\tDR=8");
  else Serial.println("\tDR=64/3");
  Serial.println("\tM="+String((int) pow(2,firmware.campo.M)));
  if (firmware.campo.TRext == 0) Serial.println("\tNo usa Tono piloto");
  else Serial.println("\tUsa tono piloto");
  if (firmware.campo.Sel == 0 || firmware.campo.Sel == 1) Serial.println("\tSel=ALL");
  else if (firmware.campo.Sel == 2) Serial.println("\tSel=-SL");
  else Serial.println("\tSel=SL");
  Serial.println("\tSession=S"+String(firmware.campo.Session));
  if (firmware.campo.Target == 0) Serial.println("\tTarget=A");
  else Serial.println("\tTarget=B");
  Serial.println("\tQ="+String(firmware.campo.Q));
  
}

void loop() 
{
  
}
