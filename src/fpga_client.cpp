#include <iostream>
// #include <netinet/in.h>
// #include <netdb.h>
// #include <unistd.h>
#include <string>
// #include <sys/socket.h>
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/udp.h"
#include "lwipopts.h"
#include <cstring>
#include "platform.h"

using namespace std;

class Client {
    public:
        int socketFD;
        struct sockaddr_in serverAddress;
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

            memset(&serverAddress, 0, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(portNumber);
            inet_aton(serverIP.c_str(), &(serverAddress.sin_addr));

            for (int i = 0; i < 102; i++){
                frame[i] = 1;
            }

            socketFD = 0;
            wasInitialized = false;
        }

        int initSocket(){
            if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                //cout << "Error: Socket could not be initialized." << endl;
                return -1;
            }
            
            wasInitialized = true;

            //cout << "Socket successfully initialized." << endl;

            return 0;
        }

        int closeSocket(){
            if (!wasInitialized){
                //cout << "Error: Socket is not open." << endl;
                return -1;
            }
            close(socketFD);

            wasInitialized = false;
            return 0;
        }

        int sendMessage(string message, int bufferSize){
            size_t bytesSent;

            if (!wasInitialized){
                //cout << "Error: Socket was not initialized." << endl;
                return -1;
            }
                

            if ((bytesSent = sendto(socketFD, (const char *)message.c_str(), bufferSize, 0, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0){
                //cout << "Error: Could not send message." << endl;
                return -1;
            }

            if (verbose){
                // cout << "Message sent successfully:" << endl;        
                // cout << "Bytes sent: " << bytesSent << endl;
                // cout << "Address: " << addressName << endl;
                // cout << "Port: " << portNumber << endl;
                // cout << "Message: " << message << endl;
            }

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
        //cout << "Error: Server IP address requried. E.g. ./client 127.0.0.1 -v" << endl;
    }

    if (argc > 2 && (argv[2][0] == '-' && argv[2][1] == 'v')){
        verbose = true;
    }

    // Main program - send one test message
    Client fpga_client(argv[1],1864, verbose);

    fpga_client.initSocket();

    //cout << "Sending test messages at 100 kHz." << endl;

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