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
#define PORT_NUMBER 8080

struct udp_pcb *pcb;
int32_t frame[FRAME_SIZE];
int lastSeq;
ip_addr_t * serverAddress;

static struct netif server_netif;
struct netif *echo_netif;

void print_app_header() {
	xil_printf("\n\r\n\r-----lwip UDP Transfer Mic Data------\n\r");
	xil_printf("UDP packets being sent to port %d\n\r", PORT_NUMBER);
}

int main() {
    ip_addr_t ipaddr, netmask, gw;

    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    echo_netif = &server_netif;

    init_platform();

    IP4_ADDR(&ipaddr, 192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    print_app_header();

    lwip_init();

    /* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\n\r");
        return -1;
    }

    netif_set_default(echo_netif);

    platform_enable_interrupts();

    netif_set_up(echo_netif);

    // Now start the application

    pcb = udp_new();
    if (!pcb) {
        xil_printf("Error: UDP PCB could not be created.\r\n");
        return -1;
    }

    ipaddr_aton("192.168.1.10", serverAddress);

    // Initialize frame with dummy data
    for (int i = 0; i < FRAME_SIZE; i++) {
        client->frame[i] = 1;
    }

    start_custom_application();
    
    return 0;
}


// Function to send a message
int sendMessage(const char* message, int bufferSize) {
    // xil_printf("Got to send message.\r\n");
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, bufferSize, PBUF_RAM);
    if (p == NULL) {
        xil_printf("Error: Failed to allocate pbuf\r\n");
        return -1;
    }

    memcpy(p->payload, message, strlen(message) + 1);
    
    err_t err = udp_sendto(pcb, p, &serverAddress, PORT_NUMBER);

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

    xil_printf("Message sent successfully:\r\n");
    xil_printf("Address: %s\r\n", "192.168.1.10");
    xil_printf("Port: %d\r\n", PORT_NUMBER);
    xil_printf("Message: %s\r\n", message);

    // xil_printf("About to leave send message.\r\n");

    pbuf_free(p);
    return 0;
}

// Function to send a frame
int sendFrame() {
    char msg[2048] = {0};  // message buffer with enough space

    snprintf(msg, sizeof(msg), "%d", ++lastSeq);

    for (int i = 0; i < FRAME_SIZE; i++) {
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), ",%d", frame[i]);
    }

    // xil_printf("About to call Client_sendMessage().\r\n");
    return sendMessage(msg, sizeof(msg));
}

// Main function to start the custom application
int start_custom_application() {

    // xil_printf("Sending test messages at 100 kHz.\r\n");
    xil_printf("Sending one message.\r\n");

    int count = 0;

    // while (1) {
    //     usleep(10);  // Sleep for 10 microseconds (100 kHz)

        // Update the frame with dummy data for testing
    //    for (int i = 0; i < FRAME_SIZE; i++) {
    //        fpga_client.frame[i] = i + count;
    //    }

    //    xil_printf("About to call Client_sendFrame().\r\n");
        // Send the frame
    //    Client_sendFrame(&fpga_client);

    //    count++;
    // }

    // xil_printf("About to call sendMessage()");
    sendMessage("hello world", sizeof("hello world"));

    return 0;
}