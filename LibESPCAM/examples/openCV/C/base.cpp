/********************************************************************************
*  Fco. Javier Rodriguez Navarro 
*  https://www.pinguytaz.net
*
*  Programa basico de visualización de camara.
*
*  Historico:
*     - Junio 2025    V1: Creación 
********************************************************************************/
#include <opencv2/opencv.hpp>

#include <iostream>

int main(int argc, char** argv) {
    // definimos la URL donde emite la camara
    std::string url = "http://192.168.50.1:8081/stream";
    cv::VideoCapture cap(url,cv::CAP_ANY);
    cv::namedWindow("Video desde URL", cv::WINDOW_GUI_NORMAL | cv::WINDOW_AUTOSIZE);

    if (!cap.isOpened()) {
        std::cerr << "Error: No se pudo abrir el stream de video desde la URL: " << url << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        bool ret = cap.read(frame);  // Leer un frame
        if (!ret || frame.empty()) {
            std::cerr << "Error: No se pudo leer frame o el stream terminó." << std::endl;
            break;
        }

        cv::imshow("Video desde URL", frame);

        // Salir si se presiona la tecla 'q'
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}

