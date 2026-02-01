/*************************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *
 *  Descripción: Clase manejo lector R200
 **********************************************************/
#include <LibR200.h>
#include <Utiles.h>

//  Definir si es arduino o ESP32  Para Includes
#if defined(ESP32)
#include <Arduino.h>
#endif 

// Constructor
R200::R200(HardwareSerial *serial, int baud, uint8_t RxPin, uint8_t TxPin) 
{
  _serial = serial;
  _baud = baud;
  _RxPin =RxPin;
  _TxPin = TxPin;
};

bool R200::iniciaR200()
{ 
  _serial->begin(_baud, SERIAL_8N1, _RxPin, _TxPin);
  return true;
}

t_CodError R200::get_Error() { return _error; }

/******************* 
  0x03  hw_info(t_Hardware *): Obtencion de información del modulo.
          t_Hardware   Estructura que devolvera la información del modulo
          retorna true si todo correcto
**********************/
bool R200::hw_info(t_Hardware *informacion)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x03;   // Comando solicitud de hardware 0x03
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB 
  _commandFrame[5] = 0x00;  // Solicita HW
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  if( enviaComando(_commandFrame, 8) > 0)  // Solicitamos version de HW
  {
    informacion->vHW ="";
    for (uint8_t i=0; i<RX_BUFFER_LENGTH-8; i++) 
    {
      informacion->vHW = informacion->vHW + (char)_buffer[6 + i];
      if (_buffer[8 + i] == R200_FrameFinal) { break; }
    }
  }

  _commandFrame[5] = 0x01;  // Solicita SW
  _commandFrame[6] = crc(_commandFrame,6);
  if( enviaComando(_commandFrame, 8) > 0)  // Solicitamos version de SW
  {
    informacion->vSW ="";
    for (uint8_t i=0; i<RX_BUFFER_LENGTH-8; i++) 
    {
      informacion->vSW = informacion->vSW + (char)_buffer[6 + i];
  if (_buffer[8 + i] == R200_FrameFinal) { break; }
    }
  }
  _commandFrame[5] = 0x02;  // Solicita fabricante
  _commandFrame[6] = crc(_commandFrame,6);
  if( enviaComando(_commandFrame, 8) > 0)  // Solicitamos fabricante
  {
    informacion->fabricante ="";
    for (uint8_t i=0; i<RX_BUFFER_LENGTH-8; i++) 
    {
      informacion->fabricante = informacion->fabricante + (char)_buffer[6 + i];
      if (_buffer[8 + i] == R200_FrameFinal) { break; }
    }
  }
  return true;
}

/******************* 
  0x07  set_Region(t_Region): Ponemos Region.
          t_Region Valores de region
          retorna true si todo correcto.
**********************/
bool R200::set_Region(t_Region region)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x07;   // Comando pone Region 0x07
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB 
  _commandFrame[5] = (uint8_t) region;
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 8) > 0)  
  return true;
}
/******************* 
  0x08  t_Region get_Region(void): Nos da la región configurada.
          Retorna el valor de la region.
**********************/
t_Region R200::get_Region(void)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x08;   // Comando solicitud Region 0x08
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7) > 0)  
  {
    return (t_Region) _buffer[5];
  }
  return Desconocido;
}

/******************* 
  0xAB  set_Canal(uint8): Ponemos el canal.
          uint8 el canal a configurar
          retorna true si todo correcto.
**********************/
bool R200::set_Canal(uint8_t canal)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xAB;   // Comando pone Canal 0xAB
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB 
  _commandFrame[5] = (uint8_t) canal;
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 8) > 0)  return true;
  else return false;
}
/******************* 
  0xAA  get_Canal(void): obtiene el canal.
          retorna un flotante con el canal convertido a frecuencia
**********************/
float R200::get_Canal(void)
{
  float frecuencia = 0;
  
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xAA;   // Obtenemos el canal 0xAA
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7) > 0)  
  {
     int CH_Index = (int) _buffer[5];   // Valor del canal
     t_Region region = get_Region();  // Obtenemos la region configurada para el calculo.
     switch(region)
     {  
      case China_900:
        frecuencia = CH_Index * 0.25 + 920.125;
        break;
      case US:
        frecuencia = CH_Index * 0.5 + 902.25;
        break;
      case EU:
        frecuencia = CH_Index * 0.2 + 865.1;
        break;
      case China_800:
        frecuencia = CH_Index * 0.25 + 840.125;
        break;
      case Korea:
        frecuencia = CH_Index * 0.2 + 917.1;
        break;
     default:
        frecuencia = 0;
        break;
    }      
  }
  return frecuencia;
}

/******************* 
  0xB6  set_Potencia(int): Ponemos la potencia.
          int Potencia en Db
          retorna true si todo correcto
**********************/
bool R200::set_Potencia(int potencia)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xB6;   // Comando poner potencia 0xB6
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x02; // ParamLen LSB 

  uint16_t valor = (uint16_t) potencia * 100; // Valor a convertir
  _commandFrame[5] = (uint8_t) (valor >> 8) & 0xFF;
  _commandFrame[6] = (uint8_t) valor & 0xFF;
  
  _commandFrame[7] = crc(_commandFrame,7);
  _commandFrame[8] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 9) > 0)  return true;
  else return false;
}
/******************* 
  0xB7  get_Potencia(void): Obtenemos la potencia configurada
          retorna un entero con la potencia configurada
**********************/
int R200::get_Potencia(void)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xB7;   // Comando obtener potencia 0xB7
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7) > 0)  
  {
    return  (((int) _buffer[5] * 256) + (int) _buffer[6]) / 100;
  }
  return 0;
}

/******************* 
  0x0D  get_Firmware(void): Obtenemos la configuración del Firmware
          retorna una estructura t_Firmware con la configuración
**********************/
t_Firmware R200::get_Firmware(void)
{
  t_Firmware retorno;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x0D;   // Comando obtener Firmware 0x0D
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7)> 0)  
  {
    retorno.dato[0] = _buffer[6];
    retorno.dato[1] = _buffer[5]; 
  }
  return retorno;
}

/******************* 
  0x0E  set_Firmware(t_Sel, t_Session, t_Target, uint8_t Q ): Ponemos parametros del Firmware
          t_Sel banco de población logica
          t_Session control anticolisión
          t_Target objetivo a barrer
          uint8_t algoritmo anticolisión Q.
          retorna true si todo correcto.
**********************/
bool R200::set_Firmware(t_Sel Sel, t_Session Session, t_Target Target, uint8_t Q )
{
  t_Firmware retorno;
  
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x0E;   // Comando Configurar Firmware 0x0E
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x02; // ParamLen LSB 

  t_Firmware firmware;
  firmware.campo.DR = 0; // Solo soporta DR=8
  firmware.campo.M = 0; // Solo soporta M=1 que se codifica con 0
  firmware.campo.TRext = 1; // Solo soporta con tono piloto

  firmware.campo.Sel = Sel; 
  firmware.campo.Session = Session; 
  firmware.campo.Target = Target; 
  firmware.campo.Q = Q;
  
  _commandFrame[5] = (uint8_t) firmware.dato[1];
  _commandFrame[6] = (uint8_t) firmware.dato[0];

  _commandFrame[7] = crc(_commandFrame,7);
  _commandFrame[8] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 9) > 0)  return true;
  return false;
}

/******************* 
  0xA9  insertCanales(uint8_t, uint8_t*): Insertamos canales para hopping
          uint8_t Numero de canales a insertar
          uint8_t* array con los canales a inseertar.
**********************/
bool R200::insertCanales(uint8_t nCanales, uint8_t *canales)
{
  int pos =0;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xA9;   // Comando de insercion de canales 0xA9

  if (nCanales == 0)    // Limpiamos la frecuencioas de hopping seran aleatorios todos
  {
    _commandFrame[3] = 0x00; // ParamLen MSB
    _commandFrame[4] = 0x01; // ParamLen LSB   Solo un parametro indicando sin canales 
    _commandFrame[5] = 0x00;   // No hay canales
    _commandFrame[6] = crc(_commandFrame,6);
    _commandFrame[7] = R200_FrameFinal;
    pos = 8;
  }
  else
  {
    _commandFrame[3] = 0x00; // ParamLen MSB
    _commandFrame[4] = nCanales + 1; // ParamLen LSB   Numero de canales + 1
    _commandFrame[5] = nCanales;     // Numero de canales
    // Rellenamos canales
    pos = 6;  // Primer canal
    for (int i =0; i< nCanales; i++)
    {
      _commandFrame[pos] = canales[i];
      pos++;
    }
    _commandFrame[pos] = crc(_commandFrame,pos);
    pos++;
    _commandFrame[pos] = R200_FrameFinal;
    pos++;
  }
  if(enviaComando(_commandFrame, pos) > 0)  return true;
  else return false;
}

/******************* 
  0xAD  hopping(bool): Activa o desactiva hopping.
          bool activa o desactiva
**********************/
bool R200::hopping(bool poner)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xAD;   // Comando poner o cancelar hopping
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB Tiene un parametro

  if (poner) _commandFrame[5] = 0xFF;
  else _commandFrame[5] = 0x00;
  
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 8) > 0)  
  return true;
}

/******************* 
  0xB0  transmisionContinua(bool continua): Activa o desactiva la transmisión continua, consejo es no y solo si para mediciones antenas
          bool activa o desactiva
**********************/
bool R200::transmisionContinua(bool continua)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xB0;   // Pone o quita la tramision continua
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB Tiene un parametro

  if (continua) _commandFrame[5] = 0xFF;
  else _commandFrame[5] = 0x00;
  
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 8) > 0)  
  return true;
}

/******************* 
  0xF0  set_Demodulador(t_Demodulador): Ponemos datos del modulador
          t_Demodulador estructura con los datos del modulador
          retorna true si todo correcto.
**********************/
bool R200::set_Demodulador(t_Demodulador parametros)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xF0;   // Comando poner datos demodulador 0xF0
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x04; // ParamLen LSB 

  _commandFrame[5] = parametros.dato[0];
  _commandFrame[6] = parametros.dato[1];
  _commandFrame[7] = parametros.dato[3];
  _commandFrame[8] = parametros.dato[2];

  _commandFrame[9] = crc(_commandFrame,9);
  _commandFrame[10] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 11) > 0)  return true;
  else return false;
}
/******************* 
  0xF1  get_Demodulador(void): Obtiene datos del modulador
          Retorna t_Demodulador estructura con los datos del modulador
**********************/
t_Demodulador R200::get_Demodulador(void)
{
  t_Demodulador retorno;

  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0xF1;   // Comando recoge parametros demodulador
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7) > 0)  
  {
    retorno.dato[0] = _buffer[5];
    retorno.dato[1] = _buffer[6];
    retorno.dato[2] = _buffer[8];
    retorno.dato[3] = _buffer[7];
    
    return retorno;
  }

  return retorno;
}

/******************* 
  0x22  simplePool(t_Tag*, int): Realiza un simple pool que detecta los TAG que estan en el RANGO
          t_Tag*  Array con las etiquetas localizadas.
          int     Numero máximo de etiquetas a leer
**********************/
int R200::simplePool(t_Tag *etiquetas, int maxEtiquetas)
{
  int etiquetasLeidas = 0;
  uint8_t bytesRecibidos;

  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x22;   // Comando para realizar un Pool, para ver etiquetas al alcance
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  bytesRecibidos = enviaComando(_commandFrame, 7);
  if (bytesRecibidos > 0)
  {
    // Tipo R200_TipoRespuesta [1] No se localizan etiquetas
    if (_buffer[1] == R200_TipoRespuesta) return 0; // No se han leido etiquetas.

    // Vamos procesando etiquetas
    int pos = 3; // Primera posición de etiqueta que marca PL_msb
       // Si encontraramos más
    while (pos < bytesRecibidos)
    {
      if(etiquetasLeidas >= maxEtiquetas) {return etiquetasLeidas;}  // Si hay más etiquetas en la trama no se leen
      int longitudDatos = (_buffer[pos] *256 ) + _buffer[pos+1];  
      // RSSI(1) PC(2) EPC (longDat-1-2-2)
      pos = pos + 2; // Nos posicionamos en RSSI
      etiquetas[etiquetasLeidas].RSSI = _buffer[pos++];
      etiquetas[etiquetasLeidas].PC_msb = _buffer[pos++];
      etiquetas[etiquetasLeidas].PC_lsb = _buffer[pos++];
      etiquetas[etiquetasLeidas].LongEPC = longitudDatos - 5;  // Datos EPC (longDat-5)
      etiquetas[etiquetasLeidas].Datos_EPC = (uint8_t*)malloc(etiquetas[etiquetasLeidas].LongEPC * sizeof(uint8_t));
      // Rellenamos los datos
      for (int i =0 ; i < etiquetas[etiquetasLeidas].LongEPC; i++) etiquetas[etiquetasLeidas].Datos_EPC[i] = _buffer[pos++];
      etiquetas[etiquetasLeidas].CRC_msb= _buffer[pos++];
      etiquetas[etiquetasLeidas].CRC_lsb= _buffer[pos++];  
      etiquetasLeidas++;
      pos = pos + 2 + 3;  // Avanza 2 de Checksum a sig +3 quita AA0222
    }
  }
  return etiquetasLeidas;
}

/************
     Leemos un TAG, simple en localizacion
     4 bytes de clave
     Banco de memoria, desplazamiento y longitud, en palabras (2 bytes)
     Donde nos llegan los datos
*/
bool R200::leeTAG(uint8_t *clave, t_BankMemory  bancoMemory, unsigned int desplazamiento, unsigned int longitud, t_DatosTag *datosLeidos)
{
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x39;   // Comando lectura un TAG 0x39
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x09; // ParamLen LSB 

  _commandFrame[5] = clave[0];    // Clave
  _commandFrame[6] = clave[1];
  _commandFrame[7] = clave[2];
  _commandFrame[8] = clave[3];
  _commandFrame[9] = bancoMemory;  // Banco de memoria

  union 
  {
    unsigned int ui;
    uint8_t bytes[2];
  } eldato;
  eldato.ui = desplazamiento;         // desplazamiento
  _commandFrame[10] = eldato.bytes[1];     
  _commandFrame[11] = eldato.bytes[0];    
  eldato.ui = longitud;         // longitud
  _commandFrame[12] = eldato.bytes[1];     
  _commandFrame[13] = eldato.bytes[0];    
  
  _commandFrame[14] = crc(_commandFrame,14);
  _commandFrame[15] = R200_FrameFinal;

  enviaComando(_commandFrame, 16);
  if (_buffer[1] == R200_TipoRespuesta && _buffer[2] == CMD_ERROR)
  {
    impErrores(_buffer[5]);   // Imprimimos el error producido

    if(_buffer[5] == ERR_ACCESO_FALLIDO || (_buffer[5] > 0xA0 && _buffer[5] <=0xAF))
    {
      Serial.printf("UL(%d) PC: 0x%02X%02X EPC: ", _buffer[6],_buffer[7],_buffer[8]);
      for (int pos = 9; pos < 7 + _buffer[6]; pos++) Serial.printf("%02X", _buffer[pos]);
      Serial.println("");
    }
  }
  else
  {
    datosLeidos->bancoMemoria = bancoMemory;
    eldato.bytes[0] = _buffer[7];     
    eldato.bytes[1] = _buffer[6];
    datosLeidos->PC = eldato.ui;

    datosLeidos->longEPC = (int) (_buffer[5] - 2);
    datosLeidos->Datos_EPC =  (uint8_t*)malloc(datosLeidos->longEPC * sizeof(uint8_t));
    for (int pos = 8; pos < 6 + _buffer[5]; pos++) datosLeidos->Datos_EPC[pos-8] = _buffer[pos];

    eldato.bytes[0] = _buffer[4]; 
    eldato.bytes[1] = _buffer[3];
    datosLeidos->longDatos = eldato.ui - _buffer[5] - 1;
    datosLeidos->Datos =  (uint8_t*)malloc(datosLeidos->longDatos * sizeof(uint8_t));
    for (int pos = 6 + _buffer[5]; pos < (6 + _buffer[5] + datosLeidos->longDatos); pos++) datosLeidos->Datos[pos-(6 + _buffer[5])] = _buffer[pos];

    return true;
  }
  return false;
}

/*
     Escribimos datos en un TAG, simple 
     4 bytes de clave
     Banco de memoria, desplazamiento y longitud, en palabras (2 bytes)
     datos que seran longitud*2
*/
bool R200::escribeTAG(uint8_t *clave, t_BankMemory  bancoMemory, unsigned int desplazamiento, unsigned int longitud, 
                      uint8_t *datos,t_DatosTag *etiquetaEscrita)
{
  union 
  {
    unsigned int ui;
    uint8_t bytes[2];
  } eldato;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x49;   // Comando escritura un TAG 0x49

  eldato.ui = longitud*2 + 9;
  _commandFrame[3] = eldato.bytes[1]; // ParamLen MSB
  _commandFrame[4] = eldato.bytes[0]; // ParamLen LSB 

  _commandFrame[5] = clave[0];    // Clave
  _commandFrame[6] = clave[1];
  _commandFrame[7] = clave[2];
  _commandFrame[8] = clave[3];

  _commandFrame[9] = bancoMemory;  // Banco de memoria

  eldato.ui = desplazamiento;         // desplazamiento
  _commandFrame[10] = eldato.bytes[1];     
  _commandFrame[11] = eldato.bytes[0];    
  
  eldato.ui = longitud;         // longitud
  _commandFrame[12] = eldato.bytes[1];     
  _commandFrame[13] = eldato.bytes[0];    
  
  for(int pos=0; pos<longitud*2; pos++) _commandFrame[pos+14] = datos[pos];
  _commandFrame[14+(longitud*2)] = crc(_commandFrame,14+(longitud*2));
  _commandFrame[15+(longitud*2)] = R200_FrameFinal;

  enviaComando(_commandFrame, 16+(longitud*2));
  // Verificamos errores
  if (_buffer[1] == R200_TipoRespuesta && _buffer[2] == CMD_ERROR)
  {
    if (_buffer[1] == R200_TipoRespuesta && _buffer[2] == CMD_ERROR)
    {
      impErrores(_buffer[5]);   // Imprimimos el error producido
    
      if(_buffer[5] == ERR_ACCESO_FALLIDO || (_buffer[5] > 0xA0 && _buffer[5] <=0xBF))
      {
        Serial.printf("UL(%d) PC: 0x%02X%02X EPC: ", _buffer[6],_buffer[7],_buffer[8]);
        for (int pos = 9; pos < 7 + _buffer[6]; pos++) Serial.printf("%02X", _buffer[pos]);
        Serial.println("");
      }
    }
  }
  else
  {
    Serial.println("Etiqueta escrita");
    eldato.bytes[0] = _buffer[7];     
    eldato.bytes[1] = _buffer[6];
    etiquetaEscrita->PC = eldato.ui;

    etiquetaEscrita->longEPC = (int) (_buffer[5] - 2);
    etiquetaEscrita->Datos_EPC =  (uint8_t*)malloc(etiquetaEscrita->longEPC * sizeof(uint8_t));
    for (int pos = 8; pos < 6 + _buffer[5]; pos++) etiquetaEscrita->Datos_EPC[pos-8] = _buffer[pos];

    eldato.bytes[0] = _buffer[4]; 
    eldato.bytes[1] = _buffer[3];
    etiquetaEscrita->longDatos = eldato.ui - _buffer[5] - 1;
    etiquetaEscrita->Datos =  (uint8_t*)malloc(etiquetaEscrita->longDatos * sizeof(uint8_t));
    for (int pos = 6 + _buffer[5]; pos < (6 + _buffer[5] + etiquetaEscrita->longDatos); pos++) etiquetaEscrita->Datos[pos-(6 + _buffer[5])] = _buffer[pos];

    return true;
  }
  return false;
}

/******************* 
  0x0C  setSelect(t_Parametros): Establece parámetros Select, antes de realizar lecturas, escrituras o bloqueos
          t_Parametros Parametros
**********************/
bool R200::setSelect(t_Objetivo objetivo, t_Accion accion, t_BankMemory banco, uint32_t inicioEnBits, uint8_t *mascara, unsigned int longitud, 
                    t_Truncate truncate)
{
  //int etiquetasLeidas = 0;
  uint8_t bytesRecibidos;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x0C;   // Comando para realizar un Pool, para ver etiquetas al alcance
  union 
  {
    unsigned int ui;
    uint8_t bytes[2];
  } eldato;
  eldato.ui = longitud + 7;
  _commandFrame[3] = eldato.bytes[1]; // Longitud del parametro y depende de la longitud de la mascara
  _commandFrame[4] = eldato.bytes[0];
  union {
        uint8_t selParam;  // Byte completo de Sel Param Target(3)Action(3)BancoMemoria(2)
        struct {
            uint8_t mem_bank : 2;  // Bits 1-0: MemBank (00-RFU 01-EPC 10-TID 11-User)
            uint8_t action   : 3;  // Acciones sobre el tag 000-Match(incluir) 001-NoMatch(no incluir) 010-Reservado
            t_Objetivo target   : 3;  // Inventario objetivo 000-S0 001-S1 010-S2 011-S3 100-SL 101-RFU 101-RFU 111-RFU
        } campo;
    } sel;
  sel.campo.target = objetivo;
  sel.campo.action = accion;
  sel.campo.mem_bank = banco;
  _commandFrame[5] = sel.selParam;
  union 
  {
    uint32_t valor;
    uint8_t bytes[4];
  } dWord;
  dWord.valor = inicioEnBits;
  _commandFrame[6]= dWord.bytes[3];
  _commandFrame[7]= dWord.bytes[2];
  _commandFrame[8]= dWord.bytes[1];
  _commandFrame[9]= dWord.bytes[0];
  _commandFrame[10]=  longitud * 8; // Se pone en bits
  _commandFrame[11]=  truncate ;

  for(int pos=0; pos<longitud; pos++) _commandFrame[pos+12] = mascara[pos];
  _commandFrame[12+longitud] = crc(_commandFrame,12+longitud);
  _commandFrame[13+longitud] = R200_FrameFinal;

  Serial.println("Lanzamos comando ");
  bytesRecibidos = enviaComando(_commandFrame, 14+longitud);
  //if (bytesRecibidos > 0)
  
  return true;
}
/******************* 
  0x0B  getSelect(): Recoge parametros de Select.
          Retorna t_Parametros con los parametros de select configurados.
**********************/
t_Parametros R200::getSelect(void)
{
  t_Parametros retorno;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x0B;   // Comando 
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x00; // ParamLen LSB 
  _commandFrame[5] = crc(_commandFrame,5);
  _commandFrame[6] = R200_FrameFinal;

  if(enviaComando(_commandFrame, 7)> 0)  
  {
    retorno.sel.selParam = _buffer[5];
    retorno.ptr[0] = _buffer[6];
    retorno.ptr[1] = _buffer[7];
    retorno.ptr[2] = _buffer[8];
    retorno.ptr[3] = _buffer[9];
    retorno.MaskLen = _buffer[10];
    retorno.Truncate = _buffer[11];
    for (int pos = 0; pos < retorno.MaskLen/8; pos++) retorno.Mask[pos]= _buffer[pos+12];; 
  }
  return retorno;
}
/******************* 
  0x12  modoSelect(uint8_t): Ponemos el modo Select.
          uint8_t modo del select: 0x00 siempre select, 0x01 se cancela select 
          y 0x02(se pone al definir select) no envio en pooling si en lecturas o escrituras.
          Retorna true si todo correcto
**********************/
bool R200::modoSelect(uint8_t modo)
{
  t_Parametros retorno;
  _commandFrame[0] = R200_FrameCabecera;
  _commandFrame[1] = R200_TipoComando;
  _commandFrame[2] = 0x12;   // Comando 
  _commandFrame[3] = 0x00; // ParamLen MSB
  _commandFrame[4] = 0x01; // ParamLen LSB 
  _commandFrame[5] = modo;
  _commandFrame[6] = crc(_commandFrame,6);
  _commandFrame[7] = R200_FrameFinal;

  enviaComando(_commandFrame, 8);
  return true;
}

/********************************************* Privadas ********************************************************************/
/*************
  crc(uint8_t*, uint16_t ):  Calcula el checksum de una trama 
      uint8_t* array con la trama
      uint16_t posición desde donde empieza a calcular el CRC
      retorna uint8_t con el CRC
******************/
uint8_t R200::crc(uint8_t *frame, uint16_t poscrc)
{
  uint16_t check = 0;
  for(uint16_t i=2; i < poscrc; i++) 
  {
    check += frame[i];
  }
  // retornamos solo LSB
  return (check & 0xff);
}
  
/*************
  enviaComando(uint8_t*, uint16_t): envia una trama con el comando y retorna la longitud de la respuesta.
                                    Se actualiza _err con el error (retorno 0) y _buffer con los datos de retorno.
      uint8_t* Trama a enviar
      uint16_t longitud de la trama
      retorna uint8_t longitud de los datos retornados.
******************/
int R200::enviaComando(uint8_t* frame, uint16_t longitud)
{
  limpiaBuffer();  // Limpia antes de enviar comando
  _error = ERR_NINGUNO;  // Inicialmente sin error

  // debugFrame('>', frame, longitud); // Visible la trama a envias
  _serial->write(frame, longitud);  // Envia la trama al dispositivo
  delay(500);  // Pequeña espera para que le de tiempo a dar respuesta

  unsigned long inicio = millis();
  uint8_t bytesRecibidos = 0;
  
  for (int i = 0; i < RX_BUFFER_LENGTH; i++) { _buffer[i] = 0; } // Limpiamos buffer donde estara la respuesta
  while ((millis() - inicio) < 1000) // Si tarda mas de un segundo en llegar la respuesta salimos
  {
    while (_serial->available()) 
    {
      uint8_t b = _serial->read(); // Leemos byte
      if(bytesRecibidos > RX_BUFFER_LENGTH - 1) // Evitamos pasar el buffer, si se pasa deberemos aumentar buffer
      {
        Serial.print("Error: Max Buffer Longitud excedida!");
        limpiaBuffer();  // Limpia buffer
        _error = ERR_BUFFER;  // Marcamos error de buffer
        return 0;
      }
      else { _buffer[bytesRecibidos] = b;}  // Se va rellenando el buffer 

      bytesRecibidos++;
      
      if (b == R200_FrameFinal) { break; }  // Se llego al final de la trama terminamos
    }
  }

  // Verificamos si la trama es correcta o si es una respuesta de error
  // debugFrame('<', _buffer, bytesRecibidos); // Visible la trama a envias
  if (bytesRecibidos > 1 && _buffer[0] == R200_FrameCabecera && _buffer[bytesRecibidos - 1] == R200_FrameFinal) // Trama Correta
  { 
    if (_buffer[1]==R200_TipoRespuesta && _buffer[2] == CMD_ERROR)   // La respuesta es un error que esta en [5]
    {
      _error = (t_CodError) _buffer[5];
      return 0;
    }
    return bytesRecibidos; 
  }
  _error = ERR_TRAMA; // La trama es incorrecta no empieza con su cabecera y termina con su final.
  
  return 0;
}

/**************************
  limpiaBuffer(void): Limpia el buffer del puerto serie.
******************/
void R200::limpiaBuffer(void)
{
  while(_serial->available()) { _serial->read(); }
}