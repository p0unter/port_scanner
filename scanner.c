#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> // timeout

#define GREEN     "\033[32m"
#define RED       "\033[31m"
#define YELLOW    "\033[33m"
#define BOLD      "\033[1m"
#define UNDERLINE "\033[4m"
#define RESET     "\033[0m"

#define APP_VERSION "v0.1 Alpha"

int scan_port(char* target_ip, int port) {
    int sock;
    struct sockaddr_in target;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(target_ip);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    int result = connect(sock, (struct sockaddr*)&target, sizeof(target));
    close(sock);
    return (result == 0) ? 1 : 0;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("- - - - - - -\n");
        printf("Copyright 2025 " BOLD "Pounter" RESET ".\n" UNDERLINE "VERSION\t" RESET YELLOW " [%s]\n" RESET, APP_VERSION);
        printf("- - - - - - -\n");
        exit(EXIT_SUCCESS);
    } else if (argc < 3) {
        printf(BOLD "\nUsage:" RESET " %s <" RESET GREEN "Target IP" RESET "> <" GREEN "Start Port (Default: 1)" RESET "> <" GREEN "End Port (Default: 65535)" RESET "> <" GREEN "Show close ports (Optional): -y" RESET ">\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *target_ip = argv[1];
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, target_ip, &(sa.sin_addr)) != 1) {
        fprintf(stderr, RED BOLD "[ERR]" RESET " Invalid " UNDERLINE "IP address" RESET "! Please try again.\n");
        exit(EXIT_FAILURE);
    }

    int start_port = 1, end_port = 65535;
    if (argc > 2 && argv[2][0] != '\0') {
        start_port = atoi(argv[2]);
    }
    if (argc > 3 && argv[3][0] != '\0') {
        end_port = atoi(argv[3]);
    }

    if (start_port < 1 || end_port > 65535 || start_port > end_port) {
        fprintf(stderr, RED BOLD "[ERR]" RESET " Invalid " UNDERLINE "port range" RESET "! Please try again.\n\tMIN=" RED "1, MAX=" RED "65535\n" RESET);
        exit(EXIT_FAILURE);
    }

    int show_closed = 0;
    if (argc > 4 && strcmp(argv[4], "-y") == 0) {
        show_closed = 1;
    }

    printf(YELLOW BOLD "[SM]" RESET " Port: " GREEN "%d" RESET "-" GREEN "%d\n" RESET, start_port, end_port);

    for (int port = start_port; port <= end_port; port++) {
        if (scan_port(target_ip, port)) {
            printf(GREEN "[OP]---" RESET "%d\n", port);
        } else if (show_closed) {
            printf(RED "[CL]---" RESET "%d\n", port);
        }
    }

    printf(YELLOW BOLD "\n[SM]" RESET " Scanning Completed.\n");
    return 0;
}
