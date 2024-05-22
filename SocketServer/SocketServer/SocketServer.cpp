#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {

    setlocale(LC_ALL, "Russian");

    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ConnectSocket = INVALID_SOCKET;
    char recvBuffer[512];

    const char* sendBuffer = "Hello from server";

    // Инициализация Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup не удалось с результатом: " << result << endl;
        return 1;
    }

    // Настройка подсказок для информации об адресе
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;         // IPv4
    hints.ai_socktype = SOCK_STREAM;   // Потоковый сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP;   // Протокол TCP
    hints.ai_flags = AI_PASSIVE;       // Пассивный сокет для связывания

    // Разрешение адреса сервера и порта
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo не удалось с ошибкой: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Создание сокета для сервера для прослушивания клиентских соединений
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Создание сокета не удалось" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Связывание сокета с адресом и портом, указанным в addrResult
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Связывание не удалось, ошибка: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Прослушивание входящих запросов на соединение на связанном сокете
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "Прослушивание не удалось, ошибка: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Принятие запроса на соединение от клиента
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Сокет прослушивания больше не нужен
    closesocket(ListenSocket);

    // Основной цикл для приема данных от клиента и отправки ответов
    do {
        // Очистка буфера приема
        ZeroMemory(recvBuffer, 512);

        // Прием данных от клиента
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            // Данные были получены
            cout << "Получено " << result << " байт" << endl;
            cout << "Полученные данные: " << recvBuffer << endl;

            // Отправка ответа обратно клиенту
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Отправка не удалась, ошибка: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            // Соединение было закрыто клиентом
            cout << "Соединение закрывается" << endl;
        }
        else {
            // Произошла ошибка при приеме
            cout << "Прием не удался, ошибка: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    // Завершение соединения, так как больше не будет отправляться данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Завершение не удалось, ошибка: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Очистка и закрытие подключенного сокета
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
