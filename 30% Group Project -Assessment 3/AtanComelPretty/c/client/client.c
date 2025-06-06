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
    server_addr.sin_port = htons(1025);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Send message
    sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive response from the server
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    printf("Server: %s\n", buffer);

    close(sockfd);
}

int main() {
    char option;

    printf("Choose an option:\n");
    printf("1. Update points for Atan_Comel\n");
    printf("2. Get points for Atan_Comel\n");
    printf("Enter option: ");
    scanf(" %c", &option);

    if (option == '1') {
        send_request("UPDATE:Atan_Comel"); // Update points for Atan_Comel with the correct format
    } else if (option == '2') {
        send_request("GET_POINTS:Atan_Comel"); // Get points for Atan_Comel
    } else {
        printf("Error: Invalid option. Please choose 1 or 2.\n");
    }

    return 0;
}
