
#include "windows_tcp.h"
#include "flint_debugger.h"

WindowsTcp::WindowsTcp(uint16_t port, const char *address, void (*rxCallback)(WindowsTcp *, uint8_t *, uint32_t)) {
    WSADATA wsaData;
    SOCKADDR_IN serverAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    server = socket(AF_INET, SOCK_STREAM, 0);
    client = 0;
    semaphore = CreateSemaphore(NULL, 1, 1, NULL);

    serverAddr.sin_addr.s_addr = inet_addr(address);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    ::bind(server, reinterpret_cast<SOCKADDR *>(&serverAddr), sizeof(serverAddr));
    listen(server, 0);

    this->rxCallback = rxCallback;

    hThread = CreateThread(
        NULL,                                       /* Default security attributes */
        0x200000,                                   /* Default stack size */
        (LPTHREAD_START_ROUTINE)receiveTask,        /* Thread function */
        this,                                       /* Parameter to thread function */
        0,                                          /* Default creation flags */
        NULL
    );
}

bool WindowsTcp::sendData(uint8_t *data, uint32_t length) {
    WaitForSingleObject(semaphore, INFINITE);
    if(!client)
        return false;
    bool ret = send(client, (const char *)data, length, 0) == length;
    ReleaseSemaphore(semaphore, 1, NULL);
    return ret;
}

void WindowsTcp::receiveTask(WindowsTcp *tcp) {
    char rxBuff[1024];
    SOCKADDR_IN clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while(1) {
        WaitForSingleObject(tcp->semaphore, INFINITE);
        tcp->client = accept(tcp->server, reinterpret_cast<SOCKADDR *>(&clientAddr), &clientAddrSize);
        ReleaseSemaphore(tcp->semaphore, 1, NULL);
        if(tcp->client != INVALID_SOCKET) {
            while(1) {
                int rxSize = recv(tcp->client, rxBuff, sizeof(rxBuff), 0);
                if(rxSize > 0 && tcp->rxCallback)
                    tcp->rxCallback(tcp, (uint8_t *)rxBuff, rxSize);
                else {
                    WaitForSingleObject(tcp->semaphore, INFINITE);
                    tcp->client = 0;
                    ReleaseSemaphore(tcp->semaphore, 1, NULL);
                    break;
                }
            }
        }
    }
}

WindowsTcp::~WindowsTcp(void) {
    if(hThread)
        TerminateThread(hThread, 0);
    if(semaphore)
        CloseHandle(semaphore);
}
