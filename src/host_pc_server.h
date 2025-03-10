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
        mutex frameLock; // protects frame
        int32_t frame[102];

        Server(string serverIP, int portNumber, int bufferSize, bool v);

        int initSocket();

        int closeSocket();

        int sendMessage(string message);

        thread startServer ();
};