namespace FlintUartServer {
    delegate void DataReceived(FlintProtocol sender, byte[] data, int length);
    internal interface FlintProtocol {
        event DataReceived DataReceived;

        bool Connect();
        bool Send(byte[] data, int offset, int length);
        bool IsConnected();
        void Disconnect();
    }
}
