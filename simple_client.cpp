#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]){

    int my_socket;
    
    if ((my_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Error: Socket could not be created." << endl;
        exit(1);
    }

    // Find server address
    // string serv_name = "localhost";
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(1864);
    inet_aton("10.91.145.22", &(sa.sin_addr)); // provided by incus

    

    // struct addrinfo *addr_serv;
    // struct addrinfo hints;

    // hints.ai_family = AF_INET; // IPv4
    // hints.ai_socktype = SOCK_DGRAM; // UDP
    // hints.ai_protocol = 0; // any protocol
    // hints.ai_flags = 0; // no flags

    // int e; // store error result of getaddrinfo

    // if ((e = getaddrinfo(serv_name.c_str(), nullptr, nullptr, &addr_serv)) < 0){
    //     cout << "Error: Server address not found for name: " << serv_name.c_str() << endl;
    //     exit(1);
    // }

    // addrinfo * rover = addr_serv;
    // while (rover != nullptr){
    //     if ((((sockaddr_in*)rover->ai_addr)->sin_addr).s_addr != 0) {
    //         // First non-zero address
    //         cout << "Server address is: " << inet_ntoa(((sockaddr_in*)rover->ai_addr)->sin_addr) << endl;
    //         sa.sin_addr = ((sockaddr_in*)rover->ai_addr)->sin_addr;
    //         break;
    //     }
    //     rover = rover->ai_next;
    // }
    // freeaddrinfo(addr_serv);

    // struct sockaddr dest_address;
    // dest_address.sa_family = AF_INET;
    // dest_address.sa_data = inet_aton("0".c_str(), &dest_address.sa_data);

    // Bind not needed for UDP client

    // Send message to the server
    char myMessage[12];
    myMessage[0] = 'H';
    myMessage[1] = 'E';
    myMessage[2] = 'L';
    myMessage[3] = 'L';
    myMessage[4] = 'O';
    myMessage[5] = ' ';
    myMessage[6] = 'T';
    myMessage[7] = 'H';
    myMessage[8] = 'E';
    myMessage[9] = 'R';
    myMessage[10] = 'E';
    myMessage[11] = '\0';

    size_t byteCount;

    if ((byteCount = sendto(my_socket,(const char *)myMessage,12,0,(const struct sockaddr *)&sa,sizeof(sa))) < 0) {
        cout << "Error: Message not sent." << endl;
    }

    cout << "Sent " << byteCount << " bytes." << endl;
    cout << "Address: " << inet_ntoa(sa.sin_addr) << endl;
    cout << "Port: " << sa.sin_port << endl;
    cout << "Zero: " << sa.sin_zero << endl;
    cout << "Family: " << sa.sin_family << endl;

    cout << "Waiting for reply" << endl;

    if ((recvfrom(my_socket,myMessage,12,0,nullptr,nullptr)) < 0) {
        cout << "Error: message receive error." << endl;
    }
    cout << "Reply received" << endl;
    cout << myMessage << endl;

    // Close the socket
    close(my_socket);
}