#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

class Client {
    public:
        int socketFD;
        struct sockaddr_in serverAddress;
        bool wasInitialized;
        string addressName;
        int portNumber;
        bool verbose;

        Client(string serverIP, int portNumber, bool v){
            addressName = serverIP;
            this->portNumber = portNumber;
            verbose = v;

            memset(&serverAddress, 0, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(portNumber);
            inet_aton(serverIP.c_str(), &(serverAddress.sin_addr));

            socketFD = 0;
            wasInitialized = false;
        }

        int initSocket(){
            if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                cout << "Error: Socket could not be initialized." << endl;
                return -1;
            }
            
            wasInitialized = true;

            cout << "Socket successfully initialized." << endl;

            return 0;
        }

        int closeSocket(){
            if (!wasInitialized){
                cout << "Error: Socket is not open." << endl;
                return -1;
            }
            close(socketFD);

            wasInitialized = false;
            return 0;
        }

        int sendMessage(string message){
            size_t bytesSent;

            if (!wasInitialized){
                cout << "Error: Socket was not initialized." << endl;
                return -1;
            }
                

            if ((bytesSent = sendto(socketFD, (const char *)message.c_str(), sizeof(message), 0, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0){
                cout << "Error: Could not send message." << endl;
                return -1;
            }

            if (verbose){
                cout << "Message sent successfully:" << endl;        
                cout << "Bytes sent: " << bytesSent << endl;
                cout << "Address: " << addressName << endl;
                cout << "Port: " << portNumber << endl;
                cout << "Message: " << message << endl;
            }

            return 0;
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
    Client fpga_client(argv[1],1864, verbose);

    fpga_client.initSocket();

    // Build message to send

    // 102 mics with 32 bits each
    int mic_count = 102;
    int32_t mic_data[mic_count];

    for (int i = 0; i < mic_count; i++){
        mic_data[i] = 0xFFFFFFFF; // fill with ones
    }

    string my_message = "";

    for (int i = 0; i < mic_count; i++){
        my_message = my_message + to_string(mic_data[i]);
    }

    cout << "Sending test messages at 100 kHz." << endl;

    while (true){
        usleep(10);

        fpga_client.sendMessage(my_message);
    }

    fpga_client.closeSocket();
}