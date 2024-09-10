
#ifndef __WINDOWS_TCP_H
#define __WINDOWS_TCP_H

#include <future>
#include <winsock2.h>

class FlintTcp {
private:
    HANDLE hThread;
    SOCKET server;
    SOCKET client;
    HANDLE semaphore;
    void (*rxCallback)(FlintTcp *, uint8_t *, uint32_t);
public:
    FlintTcp(uint16_t port, const char *address, void (*rxCallback)(FlintTcp *, uint8_t *, uint32_t) = 0);

    bool sendData(uint8_t *data, uint32_t length);

    ~FlintTcp(void);
private:
    FlintTcp(const FlintTcp &) = delete;
    void operator=(const FlintTcp &) = delete;

    static void receiveTask(FlintTcp *tcp);
};

#endif /* __WINDOWS_TCP_H */
