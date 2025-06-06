#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

void send_request(const char *message) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1027);  // Match server port
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);  // Server address (localhost)

    // Send the request message
    sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive response from the server
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    buffer[sizeof(buffer) - 1] = '\0';  // Ensure buffer is null-terminated
    printf("Server 2: %s\n", buffer);

    close(sockfd);
}

int main() {
    char option;

    printf("Choose an option:\n");
    printf("1. Update points for Atan_Comel2\n");
    printf("2. Get points for Atan_Comel2\n");
    printf("Enter option: ");
    scanf(" %c", &option);

    if (option == '1') {
        // Request to update points
        send_request("UPDATE:Atan_Comel2");
    } else if (option == '2') {
        // Request to get points
        send_request("GET_POINTS:Atan_Comel2");
    } else {
        printf("Error: Invalid option entered. Please choose 1 or 2.\n");
    }

    return 0;
}
