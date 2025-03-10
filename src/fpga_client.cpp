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

        Client(string serverIP, int portNumber){
            addressName = serverIP;
            this->portNumber = portNumber;

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

            cout << "Message sent successfully:" << endl;        
            cout << "Bytes sent: " << bytesSent << endl;
            cout << "Address: " << addressName << endl;
            cout << "Port: " << portNumber << endl;
            cout << "Message: " << message << endl;

            return 0;
        }
};



int main(int argc, char *argv[]){
    
    if (argc != 2){ // Requires 1 command line parameter, server IP address
        cout << "Error: Server IP address requried. E.g. ./client 127.0.0.1" << endl;
    }

    // Main program - send one test message
    Client fpga_client(argv[1],1864);

    fpga_client.initSocket();

    fpga_client.sendMessage("Hello World");

    fpga_client.closeSocket();
}