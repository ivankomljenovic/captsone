#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <ostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <atomic>
#include <arpa/inet.h>
#include <cstring>

using namespace std;
using namespace string_literals;

class Server {
    public:
        int socketFD;
        struct sockaddr_in serverAddress;
        bool wasInitialized;
        string addressName;
        int portNumber;
        bool verbose;
        int bufferSize;

        Server(string serverIP, int portNumber, int bufferSize, bool v){
            addressName = serverIP;
            this->portNumber = portNumber;
            verbose = v;
            this->bufferSize = bufferSize;

            memset(&serverAddress, 0, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
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

            if ((::bind(socketFD, (const struct sockaddr *)&serverAddress, sizeof(serverAddress))) < 0){
                cout << "Error: Could not bind socket." << endl;
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

        int runServer (){
            char message[bufferSize];
            struct sockaddr sourceAddress;
            socklen_t sourceLength;


            while (true){
                if ((recvfrom(socketFD, message, sizeof(message), 0, &sourceAddress, &sourceLength)) < 0){
                    cout << "Error: Error receiving the message." << endl;

                    cout << "Shutting down the server." << endl;
                    return -1;
                }

                cout << "Received message: " << message << endl;
            }
        }
};

int main(int argc, char *argv[]){
    bool verbose = false;

    if (argc < 2){
        cout << "Error: Requires the server IP address. E.g. ./server 127.0.0.1 -v" << endl;
        return -1;
    }

    if (argc > 2 && (argv[2][0] == '-' && argv[2][1] == 'v'))
        verbose = true;

    Server hostPC(argv[1], 1864, 10000, verbose);

    hostPC.initSocket();

    cout << "Starting server." << endl;

    hostPC.runServer();

    hostPC.closeSocket();
}