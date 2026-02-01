/***********************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *  Descripción: Cabecera LibR200
 **********************************************************/
#ifndef LIBR200
#define LIBR200

#include <HardwareSerial.h>
#include <tipos_LibR200.h>   // Definimos tipos de datos


#define RX_BUFFER_LENGTH 256

class R200
{
   private:
      HardwareSerial *_serial;
      int _baud;
      uint8_t _RxPin;
      uint8_t _TxPin;
      uint8_t _buffer[RX_BUFFER_LENGTH] = {0};
      uint8_t _commandFrame[30] = {0};
      t_CodError _error = ERR_NINGUNO;

      uint8_t crc(uint8_t*,uint16_t);
      int enviaComando(uint8_t*, uint16_t);
      void limpiaBuffer(void);

   public:
      R200(HardwareSerial*, int baud = 115200, uint8_t RxPin = 16, uint8_t TxPin = 17);
      bool iniciaR200();
      t_CodError get_Error();  // Nos da el codigo del ultimo error producido

      bool hw_info(t_Hardware*);
      t_Region get_Region(void);
      bool set_Region(t_Region);
      float get_Canal(void);
      bool set_Canal(uint8_t);
      int get_Potencia(void);
      bool set_Potencia(int);
      t_Firmware get_Firmware(void);
      bool set_Firmware(t_Sel, t_Session, t_Target, uint8_t);
      bool hopping(bool);
      bool insertCanales(uint8_t , uint8_t*);
      bool transmisionContinua(bool);
      t_Demodulador get_Demodulador(void);
      bool set_Demodulador(t_Demodulador);

      int simplePool(t_Tag* , int );
      bool setSelect(t_Objetivo, t_Accion, t_BankMemory, uint32_t, uint8_t*, unsigned int, t_Truncate);
      t_Parametros getSelect(void);
      bool modoSelect(uint8_t);
      bool leeTAG(uint8_t*, t_BankMemory , unsigned int, unsigned int, t_DatosTag*);
      bool escribeTAG(uint8_t*, t_BankMemory , unsigned int, unsigned int, uint8_t*, t_DatosTag*);
};
#endif
