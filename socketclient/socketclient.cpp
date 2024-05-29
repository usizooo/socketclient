#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>         
#include <iostream>          
#include <WinSock2.h>        // Включение заголовков для работы с Windows Sockets
#include <WS2tcpip.h>        // Включение заголовков для работы с TCP/IP

using namespace std;         

int main() {
    WSADATA wsaData;         // Структура для хранения информации о Windows Sockets
    ADDRINFO hints;          // Структура для хранения информации о настройках сокета
    ADDRINFO* addrResult;    // Указатель на структуру, которая будет содержать результат вызова getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET;  // Сокет для соединения с сервером, инициализируется как невалидный
    char recvBuffer[512];    // Буфер для приема данных, размером 512 байт

    const char* sendBuffer1 = "Hello from client 1";  // Первое сообщение, которое клиент отправляет серверу
    const char* sendBuffer2 = "Hello from client 2";  // Второе сообщение, которое клиент отправляет серверу

    // Инициализация Winsock, использование версии 2.2
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        // Если инициализация не удалась, вывод сообщения об ошибке и завершение программы
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // Очистка структуры hints нулями
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;         // Указываем, что будем использовать IPv4
    hints.ai_socktype = SOCK_STREAM;   // Указываем, что будем использовать потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP;   // Указываем, что будем использовать протокол TCP

    // Получение адресной информации для подключения к серверу на порту 666
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        // Если получение адресной информации не удалось, вывод сообщения об ошибке и завершение программы
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }

    // Создание сокета для подключения к серверу
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        // Если создание сокета не удалось, вывод сообщения об ошибке и завершение программы
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        // Если подключение не удалось, вывод сообщения об ошибке и завершение программы
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);  // Закрытие сокета
        ConnectSocket = INVALID_SOCKET;  // Обнуление сокета
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }
    // Отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        // Если отправка не удалась, вывод сообщения об ошибке и завершение программы
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);  // Закрытие сокета
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;  // Вывод количества отправленных байт

    // Отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        // Если отправка не удалась, вывод сообщения об ошибке и завершение программы
        cout << "Send failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);  // Закрытие сокета
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;  // Вывод количества отправленных байт

    // Завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        // Если завершение отправки данных не удалось, вывод сообщения об ошибке и завершение программы
        cout << "Shutdown failed, error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);  // Закрытие сокета
        freeaddrinfo(addrResult);   // Освобождение памяти, выделенной для addrResult
        WSACleanup();               // Завершение работы с Winsock
        return 1;
    }

    // Цикл для приема данных от сервера
    do {
        ZeroMemory(recvBuffer, 512);  // Очистка буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0);  // Получение данных от сервера
        if (result > 0) {
            // Если данные успешно получены, выводим количество полученных байт и сами данные
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            // Если сервер закрыл соединение, выводим соответствующее сообщение
            cout << "Connection closed" << endl;
        }
        else {
            // Если прием данных не удался, вывод сообщения об ошибке
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Закрытие сокета и очистка ресурсов
    closesocket(ConnectSocket);  // Закрытие сокета
    freeaddrinfo(addrResult);    // Освобождение памяти, выделенной для addrResult
    WSACleanup();                // Завершение работы с Winsock
    return 0;                    // Завершение программы
}