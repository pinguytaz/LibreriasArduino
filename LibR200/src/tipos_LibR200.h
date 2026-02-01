/***********************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *  Descripción: Tipos, constantes, enum, etc que necesitaremos en nuestra libreria
 **********************************************************/
#ifndef TIPOS_LIBR200_H
#define TIPOS_LIBR200_H

// Definicion de constantes
#define R200_FrameCabecera 0xAA
#define R200_FrameFinal 0xDD
#define R200_TipoComando 0x00
#define R200_TipoRespuesta 0x01
#define CMD_ERROR 0xFF
#define R200_TipoNotificacion 0x02

// Tipo estructura con la información del Hardware
typedef struct
{
  String vHW;
  String vSW;
  String fabricante;
} t_Hardware;

// definicion de regiones
enum t_Region 
{
  Desconocido = 0x00,
  China_900 = 0x01,
  US = 0x02,
  EU = 0x03,
  China_800 = 0x04,
  Korea = 0x06,
} ;

enum t_Sel 
{
  ALL = 0b00,
  NSL = 0b10,
  SL = 0b11,
  
} ;
enum t_Session // Sesiones etiquetas en la que trabaja
{
  S0 = 0b00,
  S1 = 0b01,
  S2 = 0b10,
  S3 = 0b11,
  
} ;

enum t_Target 
{
  A = 0b00,
  B = 0b01,
} ;

typedef union {
    uint8_t dato[2];          // Dos bytes de los que se compone los parametros de firmware
    struct {
        uint8_t reservado : 3;  // No usados
        uint8_t Q         : 4;  // Algoritmo anticolisión probabilidad de colision/latencia Q=4 son 16 ranuras
        t_Target Target   : 1;  // Objetivo del flag (A/B 0/1), permite barrer población en multiples pasadas sin reconsultar las mismas etiquetas
        
        t_Session Session : 2;  // Control anticolisión, silenciamiento etiqueta (S0,S1,S2,S3)
        t_Sel Sel         : 2;  // Banco de población logica (00/01 ALL) interroga a todos (10 -SL) seleccion TAG SL=0 (11 SL) Selección tag con SL=1
        uint8_t TRext     : 1;  // Presencia del tono siempre sera 1
        uint8_t M         : 2;  // Indica la modulación y siempre sera 00 que es M=1, ya que solo soporta modulación 1
        uint8_t DR        : 1;  // Es la tasa de división de datos. Siempre sera 0 que es 8, el 1 seria 64/3
    } campo;
} t_Firmware;

// Estructura para información de una targeta
typedef struct
{
  uint8_t RSSI;
  uint8_t PC_msb;
  uint8_t PC_lsb;
  int LongEPC;
  uint8_t *Datos_EPC;
  uint8_t CRC_msb;
  uint8_t CRC_lsb;
} t_Tag;

/*enum t_BankMemory 
{
    BANK_RFU = 0x00,      // Reservada
    BANK_EPC = 0x01,
    BANK_TID = 0x02,
    BANK_User = 0x03,
};*/

enum t_BankMemory 
{
    BANK_RFU = 0b000,    // Reservado para password, etc  
    BANK_EPC = 0b001,    // Define producto y lo cambiamos
    BANK_TID = 0b010,    // TID El identificador unico
    BANK_User = 0b011,   // Banco usuario.
};

typedef struct
{
  uint16_t PC;
  int longEPC;
  uint8_t* Datos_EPC;
  t_BankMemory bancoMemoria;
  int longDatos;
  uint8_t* Datos;
} t_DatosTag;

enum t_Objetivo 
{
  obSL = 0b000,   // Singular afecta por contenido banco memoria (EPC=1)
  obS0 = 0b001,   // Sesion 0
  obS1 = 0b010,   // Sesion 1
  obS2 = 0b011,   // Sesion 2
  obS3 = 0b100,   // Sesion 3
} ;

enum t_Accion 
{
  Coincide = 0b000,     // Coincidencia
  NoCoincide = 0b001,   // Selecciona la no coincidentes
  Inactivo = 0b010,   // Todas seleccionadas Select incativo
  Inactivo2 = 0b011,   // Todas a no seleccionadas Select inavtivo 
  Toggle = 0b100,   // Van cambiando de seleccionada a no, poco uso
} ;

enum t_Truncate
{
  NoTrunca = 0x00,
  Trunca = 0x80,
} ;

typedef struct {
    union {
        uint8_t selParam;  // Byte completo de Sel Param Target(3)Action(3)BancoMemoria(2)
        struct {
            uint8_t mem_bank  : 2;  // Bits 1-0: MemBank (00-RFU 01-EPC 10-TID 11-User)
            uint8_t action    : 3;  // Acciones sobre el tag 000-Match(incluir) 001-NoMatch(no incluir) 010-Reservado
            t_Objetivo target : 3;  // Inventario objetivo 000-S0 001-S1 010-S2 011-S3 100-SL 101-RFU 101-RFU 111-RFU
        } campo;
    } sel;
    uint32_t ptr[4];          
    uint8_t MaskLen;      // Longitud máscara en bits  asi 30 son 96 bits que seran 12 bytes
    uint8_t Truncate;      // 0x00 deshabilitada, 0x80 habilita
    uint8_t Mask[32];      // Máscara 32 es la mascara más grande que puede haber 256 bits
} t_Parametros;


/* Codigos de Mixer Gain. Ajusta cuanto amplifica la señal RF antes de pasar a la FI
   1-5 Muy bajo pueden perderse etiquetas lejanas y mal orientadas
   12-15 Muy alto puede saturar con etiquetas muy cercanas 
   MG_9 
*/
enum t_mixerGain 
{
  MG_0 = 0x00,
  MG_3 = 0x01,
  MG_6 = 0x02,
  MG_9 = 0x03,
  MG_12 = 0x04,
  MG_15 = 0x05,
  MG_16 = 0x06,
};

/* Codigos IF_G (ganancia del amplificador de frecuencia intermedia) mide cuántos decibelios amplifica la señal en la etapa IF. 
   Valores típicos  12 dB hasta 40 dB. Se usa para aumentar la señal antes de su procesamiento digital.
   IFG_36 con MG_9
*/
enum t_IFAMP
{
  IFG_12 = 0x00,
  IFG_18 = 0x01,
  IFG_21 = 0x02,
  IFG_24 = 0x03,
  IFG_27 = 0x04,
  IFG_30 = 0x05,
  IFG_36 = 0x06,
  IFG_40 = 0x07,
};
/* Thrd  umbral de decisión digital "Threshold" para decidir si un bit es un 0 o 1 en la señal demodulada.
	0x01B0=432 Alto y hace al lector exigente
	0x100=256  bajo hace que el lector escuche mejor las etiquetas de señal debil (OJO posibles falsas lectras)
	 0x0180 o 0x01A0 bien para MG_9 y IFG_36
*/

// Estructura datos demodulador
typedef union
{
  uint8_t dato[4];
  struct 
  {
    t_mixerGain Mixer_G : 8;
    t_IFAMP IF_G : 8;
    uint16_t Thrd : 16;   
  } campo;
} t_Demodulador;

// Codigos de error
enum t_CodError 
{
    ERR_NINGUNO = 0x00,
    ERR_FALLO_LECTURA = 0x09,
    ERR_FALLO_ESCRITURA = 0x10,
    ERR_FALLO_INVENTARIO = 0x15,
    ERR_ACCESO_FALLIDO = 0x16,
    ERR_COMANDO = 0x17,
    ERR_FALLO_FHSS = 0x20,

    ERR_TRAMA = 0xFD,
    ERR_BUFFER = 0xFE,
    ERR_DESCONOCIDO = 0xFF,

    ERR_LOCK_FAIL = 0x13,
    ERR_KILL_FAIL = 0x12,
};


#endif
