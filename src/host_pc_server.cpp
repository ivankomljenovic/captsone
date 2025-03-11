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
#include <thread>
#include <mutex>

#include "host_pc_server.h"

using namespace std;
using namespace string_literals;

mutex frame_lock; // protects frame
int32_t frame[102];

void threadServer(int socketFD, int bufferSize){
    char message[bufferSize];
    struct sockaddr sourceAddress;
    socklen_t sourceLength;

    // Initialize some variables
    int i = 0;
    size_t pos = 0;
    string entry = "";
    string msg = "";
    int lastSeq = -1;


    while (true){
        if ((recvfrom(socketFD, message, sizeof(message), 0, &sourceAddress, &sourceLength)) < 0){
            cout << "Error: Error receiving the message." << endl;

            cout << "Shutting down the server." << endl;
            return;
        }

        //cout << "Received message: " << message << endl;
        msg = string(message);
        {
            lock_guard<mutex> g(frame_lock);

            // Parse through the message
            while (msg.find(',') != string::npos){
                //cout << i << endl;
                pos = msg.find(',');
                entry = msg.substr(0, pos);
                //cout << entry << " " << pos << endl;
                //cout << msg << endl;
                msg.erase(0, pos + 1);

                if (i == 0){
                    // this entry is the frame sequence number
                    if (stoi(entry) != 1 && stoi(entry) <= lastSeq){ // already past this packet
                        cout << "Received out of order packet. Skipping packet " << entry << endl;
                        break; // this might lead to a bug if breaks from both while loops
                    }else{
                        lastSeq = stoi(entry);
                    }
                }else{
                    frame[i-1] = stoi(entry);
                }

                i++;
            }
        }
    }
}


Server::Server(string serverIP, int portNumber, int bufferSize, bool v){
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

int Server::initSocket(){
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

int Server::closeSocket(){
    if (!wasInitialized){
        cout << "Error: Socket is not open." << endl;
        return -1;
    }
    close(socketFD);

    wasInitialized = false;
    return 0;
}

int Server::sendMessage(string message){
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

thread Server::startServer (){
    thread serverThread(threadServer, socketFD, bufferSize);

    return serverThread;
}

void readFrame(){ // please get a lock on frame_lock before using this function
    cout << "Current Frame: " << endl;

    for (int i = 0; i < 102; i++){
        cout << frame[i] << " ";
    }
    cout << endl;
}



int main(int argc, char *argv[]){
    bool verbose = false;

    if (argc < 2){
        cout << "Error: Requires the server IP address. E.g. ./server 127.0.0.1 -v" << endl;
        return -1;
    }

    if (argc > 2 && (argv[2][0] == '-' && argv[2][1] == 'v'))
        verbose = true;


    // Initialize the server
    Server hostPC(argv[1], 1864, 100000, verbose);
    hostPC.initSocket();
    cout << "Starting server." << endl;
    thread serverThread = hostPC.startServer();


    // main loop
    while (true){
        // Read the frame every 1 second
        sleep(1);
        {
            lock_guard<mutex> g(frame_lock);

            readFrame();
        }
    }


    serverThread.join();
    hostPC.closeSocket();
}