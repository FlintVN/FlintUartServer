using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FlintJVMUartServer {
    delegate void DataReceived(Protocol sender, byte[] data, int length);
    internal interface Protocol {
        event DataReceived DataReceived;

        bool Connect();
        bool Send(byte[] data, int offset, int length);
        bool IsConnected();
        void Disconnect();
    }
}
