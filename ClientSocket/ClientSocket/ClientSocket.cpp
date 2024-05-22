#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {

    setlocale(LC_ALL, "Russian");

    WSADATA wsaData; // Структура для информации о Windows Sockets
    ADDRINFO hints; // Структура для хранения информации об адресе
    ADDRINFO* addrResult; // Указатель на структуру с результатом функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к серверу
    char recvBuffer[512]; // Буфер для хранения полученных данных

    const char* sendBuffer1 = "Hello from client 1"; // Первое сообщение для отправки
    const char* sendBuffer2 = "Hello from client 2"; // Второе сообщение для отправки

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось с результатом: " << result << endl;
        return 1;
    }

    // Настройка подсказок для информации об адресе
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получение информации об адресе сервера и порте
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для подключения к серверу
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Невозможно подключиться к серверу" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Отправка первого сообщения
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Отправлено: " << result << " байт" << endl;

    // Отправка второго сообщения
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Отправка не удалась, ошибка: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Отправлено: " << result << " байт" << endl;

    // Завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Завершение отправки не удалось, ошибка: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Прием данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // Очистка буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных
        if (result > 0) {
            cout << "Получено " << result << " байт" << endl;
            cout << "Полученные данные: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Соединение закрыто" << endl;
        }
        else {
            cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    // Закрытие сокета
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult); // Освобождение памяти, выделенной функцией getaddrinfo
    WSACleanup(); // Очистка ресурсов Winsock
    return 0;
}