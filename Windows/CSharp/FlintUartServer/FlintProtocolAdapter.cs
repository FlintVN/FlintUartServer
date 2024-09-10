namespace FlintUartServer {
    internal class FlintProtocolAdapter {
        private FlintProtocol protocol1;
        private FlintProtocol protocol2;

        public FlintProtocolAdapter(FlintProtocol protocol1, FlintProtocol protocol2) {
            this.protocol1 = protocol1;
            this.protocol2 = protocol2;

            protocol1.DataReceived += Protocol1_OnDataReceived;
            protocol2.DataReceived += Protocol2_OnDataReceived;
        }

        private void Protocol1_OnDataReceived(FlintProtocol sender, byte[] data, int length) {
            protocol2.Send(data, 0, length);
        }

        private void Protocol2_OnDataReceived(FlintProtocol sender, byte[] data, int length) {
            protocol1.Send(data, 0, length);
        }

        public bool Connect() {
            if(protocol1.Connect())
                if(protocol2.Connect())
                    return true;
            return false;
        }

        public bool IsConnected() {
            return protocol1.IsConnected() && protocol2.IsConnected();
        }

        public void Disconnect() {
            protocol1.Disconnect();
            protocol2.Disconnect();
        }
    }
}
