using System.IO.Ports;

namespace FlintJVMUartServer {
    internal class WindowsUart : Protocol {
        public event DataReceived DataReceived;

        private SerialPort serialPort;
        private string port;
        private int baurdRate;

        public WindowsUart(string port, int baurdRate) {
            this.serialPort = null;
            this.port = port;
            this.baurdRate = baurdRate;
        }

        public string Port {
            get => port;
        }

        public bool Connect() {
            if(serialPort == null || !serialPort.IsOpen) {
                try {
                    serialPort = new SerialPort();
                    serialPort.PortName = port;
                    serialPort.BaudRate = baurdRate;
                    serialPort.Parity = Parity.None;
                    serialPort.DataBits = 8;
                    serialPort.StopBits = StopBits.One;
                    serialPort.DataReceived += SerialPort_DataReceived;
                    serialPort.Open();
                    return serialPort.IsOpen;
                }
                catch {
                    return false;
                }
            }
            return true;
        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e) {
            byte[] data = new byte[serialPort.BytesToRead];
            serialPort.Read(data, 0, data.Length);
            if(DataReceived != null)
                DataReceived(this, data, data.Length);
        }

        public bool Send(byte[] data, int offset, int length) {
            if(serialPort == null || serialPort.IsOpen) {
                try {
                    serialPort.Write(data, offset, length);
                    return true;
                }
                catch {
                    return false;
                }
            }
            return false;
        }

        public bool IsConnected() {
            if(serialPort == null)
                return false;
            return serialPort.IsOpen;
        }

        public void Disconnect() {
            if(serialPort == null || serialPort.IsOpen)
                serialPort.Close();
        }
    }
}
