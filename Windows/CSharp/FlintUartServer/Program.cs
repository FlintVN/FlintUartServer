using System;
using System.Net;
using System.Threading;

namespace FlintJVMUartServer {
    internal static class Program {
        [STAThread]
        private static void Main(string[] args) {
            if(args != null) {
                string serialPort = args[0];
                WindowsUart uart = new WindowsUart(serialPort, 921600);
                WindowsTcp tcp = new WindowsTcp(9620, IPAddress.Parse("127.0.0.1"));
                ProtocolAdapter adapter = new ProtocolAdapter(uart, tcp);
                if(adapter.Connect()) {
                    Thread.Sleep(10);
                    Console.WriteLine("FlintJVM debug server is started");
                    while(adapter.IsConnected())
                        Thread.Sleep(10);
                }
                else if(!uart.IsConnected())
                    Console.WriteLine("Can't connect to " + uart.Port);
                adapter.Disconnect();
            }
            else
                Console.WriteLine("Error: Please specify COM port");
        }
    }
}
