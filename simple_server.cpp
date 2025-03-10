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

int main(int argc, char *argv[]){

    // Create a UDP socket in the IPv4 domain, using the IP protocol
    int my_socket;

    if ((my_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        cout << "Error: Socket could not be created." << endl;
        exit(1);
    }

    // Bind the socket to the server's address
    static struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    //inet_aton("127.0.0.1", &(server_address.sin_addr));
    static unsigned short port = 1864;
    server_address.sin_port = htons(port);

    inet_aton("10.91.145.22", &(server_address.sin_addr));

    // string ip_address = "130.15.85.123";

    // server_address.sin_addr.s_addr = inet_aton(ip_address.c_str(),&server_address.sin_addr);
    // cout << "Server address: " << server_address.sin_addr.s_addr << endl;

    if ((::bind(my_socket, (const struct sockaddr *)&server_address,sizeof(server_address))) < 0){
        cout << "Error: Socket could not be bound with the following parameters:" << endl;
        cout << "Family: " << server_address.sin_family << endl;
        cout << "Port: " << server_address.sin_port << endl;
        cout << "Zero: " << server_address.sin_zero << endl;
        cout << "Address: " << server_address.sin_addr.s_addr << endl;
        exit(1);
    }
    cout << "Socket bound with the following parameters:" << endl;
    cout << "Family: " << server_address.sin_family << endl;
    cout << "Port: " << server_address.sin_port << endl;
    cout << "Zero: " << server_address.sin_zero << endl;
    cout << "Address: " << server_address.sin_addr.s_addr << endl;
    cout << "Server address converted: " << inet_ntoa(server_address.sin_addr) << endl;    

    char incomingMessage[12];

    struct sockaddr src_addr;
    socklen_t src_len;

    cout << "Waiting for message" << endl;
    // Receive incoming message
    if ((recvfrom(my_socket,incomingMessage,12,0,&src_addr,&src_len)) < 0){
        cout << "Error: Message receive error." << endl;
        exit(1);
    }

    cout << "Message received!" << endl;
    cout << incomingMessage << endl;

    // Send a reply back
    incomingMessage[0] = 'R';
    incomingMessage[1] = 'E';
    incomingMessage[2] = 'P';
    incomingMessage[3] = 'L';
    incomingMessage[4] = 'Y';
    incomingMessage[5] = '\0';

    if ((sendto(my_socket, incomingMessage, 10, 0, &src_addr,sizeof(src_addr))) < 0){
        cout << "Error: Error sending the reply." << endl;
    }

    // Close the socket
    close(my_socket);
}