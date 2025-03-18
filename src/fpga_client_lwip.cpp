#include "lwip/udp.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

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
                cout << "Error: Socket was already initialized." << endl;
                return -1;
            }
            
            lwip_init();

            pcb = udp_new();

            if (!pcb){
                cout << "Error: UDP PCB could not be created." << endl;
                return -1;
            }
            
            wasInitialized = true;

            cout << "UDP PCB successfully initialized." << endl;

            return 0;
        }

        int closeSocket(){
            if (!wasInitialized){
                cout << "Error: Socket is not open." << endl;
                return -1;
            }

            udp_remove(pcb);

            wasInitialized = false;
            return 0;
        }

        int sendMessage(string message, int bufferSize){
            
            if (!wasInitialized){
                cout << "Error: Socket was not initialized." << endl;
                return -1;
            }

            struct pbuf *p;

            p = pbuf_alloc(PBUF_TRANSPORT, bufferSize, PBUF_RAM);

            if (p == NULL){
                cout << "Error: Failed to allocate pbuf" << endl;
                return -1;
            }

            memcpy(p->payload, message, bufferSize);

            err_t err = udp_sendto(pcb, p, &serverAddress, portNumber);

            if (err != ERR_OK){
                cout << "Error: UDP packet could not be sent." << endl;
                return -1;
            }

            if (verbose){
                cout << "Message sent successfully:" << endl;        
                cout << "Address: " << addressName << endl;
                cout << "Port: " << portNumber << endl;
                cout << "Message: " << message << endl;
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



int main(int argc, char *argv[]){
    bool verbose = false; // do I want lots of output messages in the console?

    if (argc < 2){ // Requires 1 command line parameter, server IP address
        cout << "Error: Server IP address requried. E.g. ./client 127.0.0.1 -v" << endl;
    }

    if (argc > 2 && (argv[2][0] == '-' && argv[2][1] == 'v')){
        verbose = true;
    }

    // Main program - send one test message
    Client fpga_client(argv[1],1864, verbose); // port number specified here

    fpga_client.initSocket();

    cout << "Sending test messages at 100 kHz." << endl;

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
