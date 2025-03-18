#include "lwip/udp.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include <sleep.h>

#include "platform.h"
#include "xil_printf.h"

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

class Client {
    public:
        ip_addr_t serverAddress;
        struct udp_pcb *pcb;
        
        bool wasInitialized;
        string addressName;
        int portNumber;
        bool verbose;
        int32_t frame[102];
        int lastSeq = 0;

        Client(string serverIP, int portNumber, bool v){
            addressName = serverIP;
            this->portNumber = portNumber;
            verbose = v;

            // memset(&serverAddress, 0, sizeof(serverAddress));
            // serverAddress.sin_family = AF_INET;
            // serverAddress.sin_port = htons(portNumber);
            ipaddr_aton(serverIP.c_str(), &serverAddress);

            for (int i = 0; i < 102; i++){
                frame[i] = 1;
            }

            wasInitialized = false;
        }

        int initSocket(){
            if (wasInitialized){
                xil_printf("Error: Socket was already initialized.\r\n");
                return -1;
            }
            
            lwip_init();

            pcb = udp_new();

            if (!pcb){
                xil_printf("Error: UDP PCB could not be created.\r\n");
                return -1;
            }
            
            wasInitialized = true;

            xil_printf("UDP PCB successfully initialized.\r\n");

            return 0;
        }

        int closeSocket(){
            if (!wasInitialized){
                xil_printf("Error: Socket is not open.\r\n");
                return -1;
            }

            udp_remove(pcb);

            wasInitialized = false;
            return 0;
        }

        int sendMessage(string message, int bufferSize){
            
            if (!wasInitialized){
                xil_printf("Error: Socket was not initialized.\r\n");
                return -1;
            }

            struct pbuf *p;

            p = pbuf_alloc(PBUF_TRANSPORT, sizeof(message.c_str()), PBUF_RAM);

            if (p == NULL){
                xil_printf("Error: Failed to allocate pbuf\r\n");
                return -1;
            }

            memcpy(p->payload, message.c_str(), sizeof(message.c_str()));

            err_t err = udp_sendto(pcb, p, &serverAddress, portNumber);

            if (err != ERR_OK){
                xil_printf("Error: UDP packet could not be sent.\r\n");
                return -1;
            }

            if (verbose){
                xil_printf("Message sent successfully:\r\n");     
                xil_printf("Address: %s\r\n", addressName);
                xil_printf("Port: %d\r\n", portNumber);
                xil_printf("Message: %s\r\n", message);
            }

            pbuf_free(p);

            return 0;
        }

        int sendFrame(){
            string msg = "";

            msg = msg + to_string(++lastSeq);
            
            for (int i = 0; i < 102; i++){
                msg = msg + ',' + to_string(frame[i]);
            }

            msg = msg + ',';

            //cout << msg << endl;

            return sendMessage(msg, 2048);
        }
};



int main(){
    // Main program - send one test message
    Client fpga_client("127.0.0.1",1864, true); // port number specified here

    fpga_client.initSocket();

    xil_printf("Sending test messages at 100 kHz.\r\n");

    int count = 0; // variable for testing purposes

    while (true){ // send a new microphone frame at 100 kHz rate
        usleep(10); // should get to 10 here

        // update the frame with microphone data, for now populate with dummy data
        for (int i = 0; i < 102; i++){
            fpga_client.frame[i] = i + count;
        }

        fpga_client.sendFrame();

        count++;
    }

    fpga_client.closeSocket();
}
