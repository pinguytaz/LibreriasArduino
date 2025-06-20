#!./venv/bin/python
# -*- coding: utf-8 -*-
################################################################################
#  Fco. Javier Rodriguez Navarro 
#  https://www.pinguytaz.net
#
#  Camara.py: Programa que nos permite visualizar imagenes de un ESP-CAM y
#             solicitar comandos a la camara.
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
import requests
from datetime import datetime

def main(argv):
	url='http://192.168.50.1:8081/stream'
	rotar=90   # 0, 90, 180, 270
	ficheroVideo = ""    # Vacio es que no graba
	
	cap = cv2.VideoCapture(url) # Crear objeto para capturar video
	cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)  # Reducir buffer para video en streaming
	
	nombreWin = 'Camara IP'    # Nombre de la ventana
	cv2.namedWindow(nombreWin, cv2.WINDOW_GUI_NORMAL | cv2.WINDOW_AUTOSIZE)

	while cap.isOpened():
		ret, frame = cap.read()
		
		if not ret:
			print("Error en frame, reintentando...")
			cap.release()
			cap = cv2.VideoCapture(url)  # Reconexión automática
			continue
    
		# Aplicar rotación según ángulo actual
		if rotar == 90:
			frame = cv2.rotate(frame, cv2.ROTATE_90_CLOCKWISE)
		elif rotar == 180:
			frame = cv2.rotate(frame, cv2.ROTATE_180)
		elif rotar == 270:
			frame = cv2.rotate(frame, cv2.ROTATE_90_COUNTERCLOCKWISE)

		# Si existe fichero de video es que graba
		if ficheroVideo != "":
			if frame.shape[1] != anchoFrame or frame.shape[0] != altoFrame:
				framev = cv2.resize(frame, (anchoFrame, altoFrame))
			else: framev = frame
			video.write(framev)
		
		cv2.imshow(nombreWin, frame)
    
		# Gestion de teclas para lanzar comandos
		###tecla = cv2.waitKey(10) & 0xFF  # Aumentar tiempo de espera para reducir CPU
		tecla = cv2.waitKey(10) & 0xFF
		match tecla:
			case 27:       # ESC
				break    # Salimos del programa
			case 102:       # f el Flash
				enviaComando("flash")
			case 99:       # c Realiza una captura y la graba en un fichero
				ahora = datetime.now()
				nombreFichero = ahora.strftime("Captura_%d%m%Y-%H%M%S.jpg")
				cv2.imwrite(nombreFichero, frame)  # Guarda el frame como JPEG
			case 114:       # r Rota imagen
				rotar = (rotar + 90) % 360  # Ciclo 0->90->180->270->0
			case 118:       # v Comienza a grabar el video y lo para
				if ficheroVideo == "" :
					ahora = datetime.now()
					ficheroVideo = ahora.strftime("Video_%d%m%Y-%H%M%S.mp4")
					# Obtener tamaño de frame
					#anchoFrame = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
					#altoFrame = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
					altoFrame, anchoFrame = frame.shape[:2]
					fps = int(cap.get(cv2.CAP_PROP_FPS))
					
					# Definimos codec y crear el objeto VideoWriter
					fourcc = cv2.VideoWriter_fourcc(*'MP4V')  # o 'MJPG', 'MP4V', XVID.
					video = cv2.VideoWriter(ficheroVideo, fourcc, fps, (anchoFrame, altoFrame), True)
				else:
					video.release()
					ficheroVideo = ""	
			

			case _:        # Cualquier otro valor, para debug en realidad no deberia hacer nada
				if tecla <255:
					print("ERROR TECLA")
					print(tecla)
	cap.release()
	if ficheroVideo != "": video.release()
	cv2.destroyAllWindows()

#############    Funciones
def enviaComando(comando):
	# URL para comandos de la cammara
	comandos='http://192.168.50.1:8080/'
	comando = comandos + comando
	#print(comando)
	respuesta = requests.get(comando)
	print('Código de estado:', respuesta.status_code)
	print('Respuesta:', respuesta.text)
	
#############    Clases

############     Metodos


################# Lanzamiento de la funcion principal ##########################
if __name__ == "__main__":
    main(sys.argv)
