#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

static char DB_HOST[100];
static char DB_USER[100];
static char DB_PASS[100];
static char DB_NAME[100];
static unsigned int DB_PORT;

int server_socket;
int client_socket;

MYSQL *connect_db() {
    MYSQL *conn;
    int retries = 5;
    int delay = 5;  // seconds

    for (int i = 0; i < retries; i++) {
        conn = mysql_init(NULL);
        if (conn == NULL) {
            fprintf(stderr, "mysql_init() failed\n");
            return NULL;
        }

        if (mysql_real_connect(conn, "mysql", "testuser", "secret", "testdb", 3306, NULL, 0) != NULL) {
            printf("Connected to MySQL\n");
            return conn;
        }

        fprintf(stderr, "mysql_real_connect() failed (Attempt %d/%d): %s\n", i + 1, retries, mysql_error(conn));
        mysql_close(conn);
        sleep(delay);
    }

    return NULL;
}

void* updater_thread(void* arg) {
    while (1) {
        MYSQL *conn = connect_db();
        const char *insert = 
            "INSERT INTO users_points (user, points, datetime_stamp) "
            "VALUES ('UserC', 1, NOW()) "
            "ON DUPLICATE KEY UPDATE "
            "points = points + 1, "
            "datetime_stamp = NOW()";
        if (mysql_query(conn, insert)) {
            fprintf(stderr, "Insert/Update failed: %s\n", mysql_error(conn));
        }
        mysql_close(conn);
        sleep(5);
    }
    return NULL;
}

int get_points() {
    int points = -1;
    MYSQL *conn = connect_db();
    const char *query = "SELECT points FROM users_points WHERE user='UserC'";
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return points;
    }
    MYSQL_RES *res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) points = atoi(row[0]);
        mysql_free_result(res);
    }
    mysql_close(conn);
    return points;
}

int main() {
    char* env_host = getenv("DB_HOST");
    char* env_user = getenv("DB_USER");
    char* env_pass = getenv("DB_PASS");
    char* env_name = getenv("DB_NAME");
    char* env_port = getenv("DB_PORT");

    if(env_host) strcpy(DB_HOST, env_host);
    if(env_user) strcpy(DB_USER, env_user);
    if(env_pass) strcpy(DB_PASS, env_pass);
    if(env_name) strcpy(DB_NAME, env_name);
    if(env_port) DB_PORT = atoi(env_port);
    if(DB_PORT == 0) DB_PORT = 3306;

    pthread_t tid;
    pthread_create(&tid, NULL, updater_thread, NULL);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(2001);

    if (bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind() failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen() failed");
        close(server_socket);
        exit(1);
    }

    // Server startup timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] C Server started on port 2001\n",
           t->tm_hour, t->tm_min, t->tm_sec);

    while (1) {
        now = time(NULL);
        t = localtime(&now);
        printf("[%02d:%02d:%02d] Waiting for connections...\n",
               t->tm_hour, t->tm_min, t->tm_sec);

        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("accept() failed");
            continue;
        }

        now = time(NULL);
        t = localtime(&now);
        printf("[%02d:%02d:%02d] New client connected\n",
               t->tm_hour, t->tm_min, t->tm_sec);

        char buffer[1024];
        int n = read(client_socket, buffer, 1023);
        if (n > 0) {
            buffer[n] = '\0';
            if (strncmp(buffer, "GET", 3) == 0) {
                now = time(NULL);
                t = localtime(&now);
                printf("[%02d:%02d:%02d] Processing GET request\n",
                       t->tm_hour, t->tm_min, t->tm_sec);
                
                int pts = get_points();
                char response[256];
                sprintf(response, "Points for UserC: %d\n", pts);
                write(client_socket, response, strlen(response));
            } else {
                now = time(NULL);
                t = localtime(&now);
                printf("[%02d:%02d:%02d] Received unknown command\n",
                       t->tm_hour, t->tm_min, t->tm_sec);
                char *msg = "Unknown command\n";
                write(client_socket, msg, strlen(msg));
            }
        }

        close(client_socket);
        now = time(NULL);
        t = localtime(&now);
        printf("[%02d:%02d:%02d] Client disconnected\n",
               t->tm_hour, t->tm_min, t->tm_sec);
    }

    close(server_socket);
    return 0;
}
