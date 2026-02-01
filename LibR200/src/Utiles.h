/***********************************************************
 *  Autor: Fco. Javier Rodriguez Navarro
 *  WEB: www.pinguytaz.net
 *  Descripción: funciones que normalmente necesitaremos
 **********************************************************/
#ifndef UTILES
#define UTILES

void impErrores(uint8_t error)
{
  const char* error_msgs[] = {"0x00 OK", "0x01 Memory Overrun", "0x02 Memory Locked", "0x03 Memory Overrun",
                              "0x04 Low Power", "0x05 Reservado", "0x06 Reservado", "0x07 Error generico (CRC/timeout)"  };
  
  if (error >= 0xA0 && error <= 0xBF)   // Errores EPCGen2
  {
    error = error & 0x0F; // Convertimos a codigo propio de EPCGen2
    if(error > 7) Serial.println(String(error,HEX) + " Error EPCGen2 no codificado");
    printf("Error: %s\n", error_msgs[error]);
  }
  else
  {
    switch(error) 
    {
      // Errores del lector
      case ERR_FALLO_LECTURA:
        Serial.println("Fallo lectura de Tag");
        break;
      case ERR_ACCESO_FALLIDO:
        Serial.println("Error de acceso en etiqueta");
        break;
      case ERR_FALLO_ESCRITURA:
        Serial.println("Fallo escritura en etiqueta");
        break;
      case ERR_FALLO_INVENTARIO:
        Serial.println("No hay etiquetas al alcance");
        break;
      case ERR_COMANDO:
        Serial.println("Error de comando");
        break;
      case ERR_FALLO_FHSS:
        Serial.println("Error frecuencias Hopping");
        break;
      default:
        Serial.print("Error desconocido: 0x");
        Serial.println(error, HEX);
        break;
    }
  }
}

void printHexBytes(uint8_t *value, uint16_t len)
{
  for(int i=0; i<len; i++){
    Serial.print(value[i] < 0x10 ? "0" : "");
    Serial.print(value[i], HEX);
    Serial.print(" ");
  }
}
void debugFrame(char pre, uint8_t* frame, uint16_t longitud)
{
  Serial.print(pre);   // Normalmente indicara peticion '>' o recepción '<'
  printHexBytes(frame, longitud); 
  Serial.println("");
}

#endif
