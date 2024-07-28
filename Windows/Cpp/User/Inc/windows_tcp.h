
#ifndef __WINDOWS_TCP_H
#define __WINDOWS_TCP_H

#include <future>
#include <winsock2.h>

class WindowsTcp {
private:
    HANDLE hThread;
    SOCKET server;
    SOCKET client;
    HANDLE semaphore;
    void (*rxCallback)(WindowsTcp *, uint8_t *, uint32_t);
public:
    WindowsTcp(uint16_t port, const char *address, void (*rxCallback)(WindowsTcp *, uint8_t *, uint32_t) = 0);

    bool sendData(uint8_t *data, uint32_t length);

    ~WindowsTcp(void);
private:
    WindowsTcp(const WindowsTcp &) = delete;
    void operator=(const WindowsTcp &) = delete;

    static void receiveTask(WindowsTcp *tcp);
};

#endif /* __WINDOWS_TCP_H */
