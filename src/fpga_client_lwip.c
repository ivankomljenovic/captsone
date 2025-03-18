#include "lwip/udp.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"
#include <string.h>
#include <stdio.h>

#define FRAME_SIZE 102

// Client data structure
struct Client {
    ip_addr_t serverAddress;
    struct udp_pcb *pcb;
    int wasInitialized;
    char addressName[16];
    int portNumber;
    int verbose;
    int32_t frame[FRAME_SIZE];
    int lastSeq;
};

// Function to initialize the Client structure
int Client_init(struct Client* client, const char* serverIP, int portNumber, int verbose) {
    if (client->wasInitialized) {
        xil_printf("Error: Socket was already initialized.\r\n");
        return -1;
    }

    lwip_init();

    client->pcb = udp_new();
    if (!client->pcb) {
        xil_printf("Error: UDP PCB could not be created.\r\n");
        return -1;
    }

    ipaddr_aton(serverIP, &client->serverAddress);
    strncpy(client->addressName, serverIP, sizeof(client->addressName) - 1);
    client->addressName[sizeof(client->addressName) - 1] = '\0';
    client->portNumber = portNumber;
    client->verbose = verbose;

    // Initialize frame with dummy data
    for (int i = 0; i < FRAME_SIZE; i++) {
        client->frame[i] = 1;
    }

    client->wasInitialized = 1;
    xil_printf("UDP PCB successfully initialized.\r\n");
    return 0;
}

// Function to close the UDP socket
int Client_close(struct Client* client) {
    if (!client->wasInitialized) {
        xil_printf("Error: Socket is not open.\r\n");
        return -1;
    }

    udp_remove(client->pcb);
    client->wasInitialized = 0;
    return 0;
}

// Function to send a message
int Client_sendMessage(struct Client* client, const char* message, int bufferSize) {
    xil_printf("Got to send message.\r\n");

    if (!client->wasInitialized) {
        xil_printf("Error: Socket was not initialized.\r\n");
        return -1;
    }

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, bufferSize, PBUF_RAM);
    if (p == NULL) {
        xil_printf("Error: Failed to allocate pbuf\r\n");
        return -1;
    }

    memcpy(p->payload, message, strlen(message) + 1);
    
    err_t err = udp_sendto(client->pcb, p, &client->serverAddress, client->portNumber);

    if (err == ERR_MEM) {
        xil_printf("Error: Out of memory.\r\n");
        pbuf_free(p);
        return -1;
    } else if (err == ERR_RTE) {
        xil_printf("Error: Could not find route to destination address.\r\n");
        pbuf_free(p);
        return -1;
    } else if (err == ERR_VAL) {
        xil_printf("Error: No PCB or PCB is dual-stack.\r\n");
        pbuf_free(p);
        return -1;
    } else if (err != ERR_OK) {
        xil_printf("Error: Lower protocol error.\r\n");
        pbuf_free(p);
        return -1;
    }

    if (client->verbose) {
        xil_printf("Message sent successfully:\r\n");
        xil_printf("Address: %s\r\n", client->addressName);
        xil_printf("Port: %d\r\n", client->portNumber);
        xil_printf("Message: %s\r\n", message);
    }

     xil_printf("About to leave send message.\r\n");

    pbuf_free(p);
    return 0;
}

// Function to send a frame
int Client_sendFrame(struct Client* client) {
    char msg[2048] = {0};  // message buffer with enough space

    snprintf(msg, sizeof(msg), "%d", ++client->lastSeq);

    for (int i = 0; i < FRAME_SIZE; i++) {
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), ",%d", client->frame[i]);
    }

    xil_printf("About to call Client_sendMessage().\r\n");
    return Client_sendMessage(client, msg, sizeof(msg));
}

// Main function to start the custom application
int start_custom_application() {
    struct Client fpga_client;
    Client_init(&fpga_client, "192.168.1.100", 5001, 1);  // Initialize with server IP, port, and verbosity

    xil_printf("Sending test messages at 100 kHz.\r\n");

    int count = 0;

    while (1) {
        usleep(10);  // Sleep for 10 microseconds (100 kHz)

        // Update the frame with dummy data for testing
        for (int i = 0; i < FRAME_SIZE; i++) {
            fpga_client.frame[i] = i + count;
        }

        xil_printf("About to call Client_sendFrame().\r\n");
        // Send the frame
        Client_sendFrame(&fpga_client);

        count++;
    }

    Client_close(&fpga_client);
    return 0;
}