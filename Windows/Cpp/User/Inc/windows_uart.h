
#ifndef __WINDOWS_UART_H
#define __WINDOWS_UART_H

#include <stdint.h>
#include <windows.h>

class FlintUart {
public:
    const char * const port;
private:
    HANDLE hThread;
    HANDLE hSerial;
    uint32_t baudRate;
    void (*rxCallback)(FlintUart *, uint8_t *, uint32_t);
    char portPath[16];
public:
    FlintUart(const char *port, uint32_t baudRate, void (*rxCallback)(FlintUart *, uint8_t *, uint32_t) = 0);

    bool connect(void);
    bool isConnect(void) const;
    bool sendData(uint8_t *data, uint32_t size) const;

    ~FlintUart(void);
private:
    FlintUart(const FlintUart &) = delete;
    void operator=(const FlintUart &) = delete;

    bool isDataAvailable(void);
    uint32_t readData(uint8_t *buff, uint32_t size) const;
    static void receiveTask(FlintUart *uart);
};

#endif /* __WINDOWS_UART_H */
