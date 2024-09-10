
#include <iostream>
#include "windows_tcp.h"
#include "windows_uart.h"

using namespace std;

FlintTcp *tcp = NULL;
FlintUart *uart = NULL;

static void cleanup(void) {
    if(tcp) {
        tcp->~FlintTcp();
        delete tcp;
    }
    if(uart) {
        uart->~FlintUart();
        delete uart;
    }
}

static BOOL WINAPI consoleHandler(DWORD signal) {
    if(signal == CTRL_C_EVENT) {
        cleanup();
        exit(0);
    }
    return TRUE;
}

static void tcpRxHandler(FlintTcp *tcp, uint8_t *data, uint32_t size) {
    if(uart) {
        if(!uart->sendData(data, size))
            std::cout << "Error: Could not send data to " << uart->port << std::endl;
    }
}

static void uartRxHandler(FlintUart *uart, uint8_t *data, uint32_t size) {
    if(tcp)
        tcp->sendData(data, size);
}

static void checkUartConnectTask(FlintUart *uart) {
    while(1) {
        Sleep(100);
        if(!uart->isConnect()) {
            std::cout << uart->port;
            std::cout << " is disconnected" << std::endl;
            cleanup();
            exit(0);
            return;
        }
    }
}

int main(int argc, char *argv[]) {
    if(!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        std::cout << "Error: Could not set control handler" << std::endl;
        return 1;
    }
    const char *port = 0;
    if(argc > 1)
        port = argv[1];
    else {
        std::cout << "Error: Please specify COM port" << std::endl;
        return 1;
    }
    uart = new FlintUart(port, 921600, uartRxHandler);
    if(uart->connect()) {
        tcp = new FlintTcp(9620, "127.0.0.1", tcpRxHandler);
        Sleep(10);
        std::cout << "FlintJVM debug server is started" << std::endl;
    }
    else
        std::cout << "Can't connect to " << port << std::endl;
    checkUartConnectTask(uart);
    return 1;
}
