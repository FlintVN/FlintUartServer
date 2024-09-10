using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace FlintUartServer {
    internal class FlintTcp : FlintProtocol {
        public event DataReceived DataReceived;

        private int port;
        private IPAddress address;
        private TcpListener listener;
        private Socket client;
        private Thread rxThread;

        public FlintTcp(int port, IPAddress address) {
            this.port = port;
            this.address = address;
            listener = null;
            client = null;
            rxThread = null;
        }

        public bool Connect() {
            listener = new TcpListener(address, port);
            listener.Start();

            rxThread = new Thread(() => {
                while(true) {
                    client = listener.AcceptSocket();
                    while(true) {
                        byte[] data = new byte[20480];
                        int rxSize = client.Receive(data);
                        if(rxSize > 0 && DataReceived != null)
                            DataReceived(this, data, rxSize);
                        else {
                            lock(this) {
                                client.Close();
                                client = null;
                            }
                            break;
                        }
                    }
                }
            });
            rxThread.IsBackground  = true;
            rxThread.Start();

            return true;
        }

        public bool Send(byte[] data, int offset, int length) {
            lock(this) {
                if(client == null)
                    return false;
                return client.Send(data, offset, length, SocketFlags.None) == data.Length;
            }
        }

        public bool IsConnected() {
            if(listener == null || rxThread == null || !rxThread.IsAlive)
                return false;
            return true;
        }

        public void Disconnect() {
            if(rxThread != null && rxThread.IsAlive)
                rxThread.Abort();
            if(client != null) {
                client.Close();
                client = null;
            }
            if(listener != null) {
                listener.Stop();
                listener = null;
            }
        }
    }
}
