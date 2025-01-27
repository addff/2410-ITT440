#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>  // Added for timestamps

int main() {
    char *server_name = "c_server";
    int server_port = 2001;

    struct hostent *he = gethostbyname(server_name);
    if (!he) {
        fprintf(stderr, "Could not resolve host: %s\n", server_name);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
    server_addr.sin_port = htons(server_port);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        close(sockfd);
        return 1;
    }

    // Get current time for request timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] Sending GET request\n", 
           t->tm_hour, t->tm_min, t->tm_sec);

    char *msg = "GET";
    write(sockfd, msg, strlen(msg));

    char buffer[1024];
    int n = read(sockfd, buffer, 1023);
    if (n > 0) {
        buffer[n] = '\0';
        // Get response time
        now = time(NULL);
        t = localtime(&now);
        printf("[%02d:%02d:%02d] Server response: %s",
               t->tm_hour, t->tm_min, t->tm_sec, buffer);
    }

    close(sockfd);
    return 0;
}
