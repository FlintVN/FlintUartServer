
#include "windows_uart.h"

FlintUart::FlintUart(const char *port, uint32_t baudRate, void (*rxCallback)(FlintUart *, uint8_t *, uint32_t)) : port(port) {
    this->hThread = 0;
    this->hSerial = INVALID_HANDLE_VALUE;
    this->baudRate = baudRate;
    this->rxCallback = rxCallback;
    this->portPath[0] = '\\';
    this->portPath[1] = '\\';
    this->portPath[2] = '.';
    this->portPath[3] = '\\';
    uint8_t index = 0;
    while(port[index]) {
        this->portPath[index + 4] = port[index];
        index++;
    }
    this->portPath[index + 4] = 0;
}

bool FlintUart::connect(void) {
    if(hSerial != INVALID_HANDLE_VALUE && isConnect())
        return true;
    hSerial = CreateFile(
        portPath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if(hSerial == INVALID_HANDLE_VALUE)
        return false;
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if(!GetCommState(hSerial, &dcbSerialParams))
        return false;
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams))
        return false;

    double oneByteTime = (double)10 * 1000 / baudRate;
    DWORD timeoutMultiplier = (DWORD)oneByteTime;
    DWORD intervalTimeout = (DWORD)(oneByteTime * 10);

    timeoutMultiplier = (timeoutMultiplier < 1) ? 1 : timeoutMultiplier;
    intervalTimeout = (intervalTimeout < 1) ? 1 : intervalTimeout;

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = intervalTimeout;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = timeoutMultiplier;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = timeoutMultiplier;
    if(!SetCommTimeouts(hSerial, &timeouts))
        return false;

    hThread = CreateThread(
        NULL,                                       /* Default security attributes */
        0x200000,                                   /* Default stack size */
        (LPTHREAD_START_ROUTINE)receiveTask,        /* Thread function */
        this,                                       /* Parameter to thread function */
        0,                                          /* Default creation flags */
        NULL
    );

    return true;
}

bool FlintUart::isConnect(void) const {
    if(hSerial == INVALID_HANDLE_VALUE)
        return false;
    else {
        DWORD errors;
        COMSTAT status;
        if (!ClearCommError(hSerial, &errors, &status))
            return false;
        return true;
    }
}

bool FlintUart::isDataAvailable(void) {
    if(hSerial == INVALID_HANDLE_VALUE)
        return false;
    else {
        COMSTAT comstat;
        DWORD errors;

        if(ClearCommError(hSerial, &errors, &comstat))
            return comstat.cbInQue > 0;
        else
            return false;
    }
}

uint32_t FlintUart::readData(uint8_t *buff, uint32_t size) const {
    DWORD bytesRead = 0;
    if(hSerial == INVALID_HANDLE_VALUE)
        return 0;
    if(!ReadFile(hSerial, buff, size, &bytesRead, NULL))
        return 0;
    return bytesRead;
}

bool FlintUart::sendData(uint8_t *data, uint32_t size) const {
    DWORD bytesWrite = 0;
    if(hSerial == INVALID_HANDLE_VALUE)
        return false;
    if(!WriteFile(hSerial, data, size, &bytesWrite, NULL))
        return false;
    return bytesWrite == size;
}

void FlintUart::receiveTask(FlintUart *uart) {
    uint8_t rxBuff[1024 + 1];
    while(1) {
        if(uart->isDataAvailable()) {
            uint32_t rxSize = uart->readData(rxBuff, sizeof(rxBuff) - 1);
            if(rxSize > 0 && uart->rxCallback)
                uart->rxCallback(uart, rxBuff, rxSize);
        }
        Sleep(0);
    }
}

FlintUart::~FlintUart(void) {
    if(hThread)
        TerminateThread(hThread, 0);
    if(isConnect())
        CloseHandle(hSerial);
}
