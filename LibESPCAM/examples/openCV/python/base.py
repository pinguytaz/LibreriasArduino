#!./venv/bin/python
# -*- coding: utf-8 -*-
################################################################################
#  Fco. Javier Rodriguez Navarro 
#  https://www.pinguytaz.net
#
#  base.py: Programa basico de visualización de camara.
#
#  Historico:
#     - Junio 2025    V1: Creación 
#
# Previo:
#    python3 -m venv venv    Genera entorno
#    source venv/bin/activate        Activación del entorno Virtual
#    pip3 install -r requirements.txt     Instalación de dependencias en el entorno activado
################################################################################
import sys            # Para recoger los parámetros
import os
import cv2

def main(argv):
        # Definir la URL donde emite la camara.
	url='http://192.168.50.1:8081/stream'
	#cap = cv2.VideoCapture(url,cv2.CAP_ANY) # Crear objeto para capturar video
	cap = cv2.VideoCapture(url,cv2.CAP_ANY) # Crear objeto para capturar video
	cap.open(url,cv2.CAP_ANY) # Crear objeto para capturar video

	print(cap.getBackendName())
	print(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
	print(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
	
	winName = 'Camara BASE IP'    # Nombre de la ventana
	cv2.namedWindow(winName, cv2.WINDOW_GUI_NORMAL | cv2.WINDOW_AUTOSIZE)

	while cap.isOpened():
		ret, frame = cap.read()
		if not ret:
			print("Error en frame, reintentando...")
			break
		cv2.imshow(winName, frame)
    
		tecla = cv2.waitKey(1)
		if tecla == 27:  # ESC
			break
	cap.release()
	cv2.destroyAllWindows()

#############    Funciones

#############    Clases

############     Metodos



################# Lanzamiento de la funcion principal ##########################
if __name__ == "__main__":
    main(sys.argv)
