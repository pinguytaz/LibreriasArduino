# LibR200 (Libreria clase gestion lector R200)  ![Logo](LogoLibR200_Peque.jpg)  [![license](https://www.pinguytaz.net/IMG_GITHUB/gplv3-with-text-84x42.png)](https://github.com/pinguytaz/Arduino-ESP32/blob/master/LICENSE)


__Libreria LibR200__
Probada con el lector M100 26dBm V1.0 (UHF EPC C1G2/ISO 18000-6C, 840–960 MHz) V.Software: V2.3.5 Fabricante: MagicRf
  
Libreria para la gestion y programación del lector RFID-UHF R200  
  
  
El autor __NO asume ninguna responsabilidad__ por la forma en que elija utilizar cualquiera de los ejecutables/código fuente de cualquier archivo provistos. El autor y cualquier persona afiliada no serán responsables de ninguna pérdida o daño en relación con el uso de esta libreria o parte de su código.  
  
Usted ACEPTA USARLO BAJO SU PROPIO RIESGO.  

##Comandos pendientes de implementar
   Algunos comandos no se han implementado, por no encontrarles todavia uso interesante, así la clase ocupa menos memoria, y por su peligrosidad y poco uso.  
   De todas formas la implementación de nuevos comandos es sencilla con las base de la clase creada.  
  
- Multipolling 0x27 y 0x28 ya que con el simple pooling lo podemos realizar nosotros y asi ocupamos menos memoria en la clase.  
- Look Almacenamiento 0x82 y Kill Tag 0x65. Por su peligrosidad en especial Kill que las deja fuera de uso y solo seria interesante en programa producción de por ejemplo un comercio.  
- Parametros de selección 0x0C, 0x0B y 0x12
- Control IO 0x 1A  
- Sleep 0x17 y 0x1D  
- Propios de NXP e impinj  
  
  
##Uso de LibR200
Libreria que  facilita la programación del lector "R200", lo primero que deberemos de hacer es crear la clase 
R200 Lector(&Puerto);  
y despues inicializarla "Lector.iniciaR200()"  

  

**Métodos LibR200**  
  
|  Funcion|Descripción                                                                                                              |
|---------------------------------------------------|-------------------------------------------------------------------------------|
|***R200(HardwareSerial*, int baud = 115200, uint8_t RxPin = 16, uint8_t TxPin = 17)**   | Constructor    |
|**bool iniciaR200()**  |   Inicializa la clase  |  
|**t_CodError get_Error()**  | Nos da el codigo del ultimo error producido   |  
|***bool hw_info(t_Hardware*)** |   Información de la placa|  
|**t_Region get_Region(void)  bool set_Region(t_Region)** |   Obtiene y programa la región de uso |  
|**float get_Canal(void) bool set_Canal(uint8_t)** |  obtiene y pone canal  |  
|**int get_Potencia(void) bool set_Potencia(int)**   | Obtiene y define la potencia |  
|**t_Firmware get_Firmware(void)  bool set_Firmware(t_Sel, t_Session, t_Target, uint8_t)** | configuración y obtención de parametros de firmware |  
|**t_Demodulador get_Demodulador(void)  bool set_Demodulador(t_Demodulador)**  |  Define y obtiene configuración del demodulador  |  
|**bool hopping(bool)**  | Activa o desactiva hopping   |  
|***bool insertCanales(uint8_t , uint8_t*)** |   Inserta canales de hopping |  
|**bool transmisionContinua(bool)**  |  Activa o desactiva la transmisión continua |  
|**int simplePool(t_Tag * , int )**  | Lectura simple de los TAGs a su alcance  |  
|**bool leeTAG(uint8_t *, t_BankMemory , unsigned int, unsigned int, t_DatosTag*)** |  Lee datos de un TAG |  
|***bool escribeTAG(uint8_t *, t_BankMemory , unsigned int, unsigned int, uint8_t*, t_DatosTag*)**   |  Escribe un TAG |  

<BR>  
Tenemos varios ejemplos de su uso:  
  
- **VerificaLector** Nos da el firmware y datos configurados  
- **LecturaTAG** Lectura de un TAG  
- **EscrituraTAG** Escribe datos en un TAG.  
- **BuscaEtiqueta** Localizas etiquetas en el ambito.  

  
<BR>  
__Website__: https://www.pinguytaz.net
