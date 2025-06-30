#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define GREEN     "\033[32m"
#define RED       "\033[31m"
#define YELLOW    "\033[33m"
#define BOLD      "\033[1m"
#define UNDERLINE "\033[4m"
#define RESET     "\033[0m"

#define APP_VERSION "v0.1 Alpha"

int scan_port(char* target_ip, int port, char* banner, size_t banner_size) {
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

    if (connect(sock, (struct sockaddr*)&target, sizeof(target)) != 0) {
        close(sock);
        return 0;
    }

    ssize_t len = recv(sock, banner, banner_size - 1, 0);
    if (len > 0) {
        banner[len] = '\0'; 
    } else {
        banner[0] = '\0';
    }

    close(sock);
    return 1;
}

char* port_controller(int port);

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("- - - - - - -\n");
        printf("Copyright 2025 " BOLD "Pounter" RESET ".\n" UNDERLINE "VERSION\t" RESET YELLOW " [%s]\n" RESET, APP_VERSION);
        printf("- - - - - - -\n");
        exit(EXIT_SUCCESS);
    } else if (argc < 2) {
        printf(BOLD "\nUsage:" RESET " %s <" RESET GREEN "Target IP" RESET "> [<" GREEN "Start Port" RESET "> <" GREEN "End Port" RESET ">] [<" GREEN "-y" RESET ">]\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *target_ip = argv[1];
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, target_ip, &(sa.sin_addr)) != 1) {
        fprintf(stderr, RED BOLD "[ERR]" RESET " Invalid " UNDERLINE "IP address" RESET "! Please try again.\n");
        exit(EXIT_FAILURE);
    }

    int start_port = 1;
    int end_port = 65535;
    int show_closed = 0;

    if (argc >= 4) {
        start_port = atoi(argv[2]);
        end_port = atoi(argv[3]);

        if (argc >= 5 && strcmp(argv[4], "-y") == 0) {
            show_closed = 1;
        }
    } else if (argc == 3) {
        if (strcmp(argv[2], "-y") == 0) {
            show_closed = 1;
        }
    }

    if (start_port < 1 || end_port > 65535 || start_port > end_port) {
        fprintf(stderr, RED BOLD "[ERR]" RESET " Invalid " UNDERLINE "port range" RESET "! Please try again.\n\tMIN=" RED "1, MAX=" RED "65535\n" RESET);
        exit(EXIT_FAILURE);
    }

    printf(YELLOW BOLD "[SM]" RESET " Port: " GREEN "%d" RESET "-" GREEN "%d\n" RESET, start_port, end_port);

    for (int port = start_port; port <= end_port; port++) {
        char banner[1024];
        if (scan_port(target_ip, port, banner, sizeof(banner))) {
            if (strlen(banner) == 0) {
                snprintf(banner, sizeof(banner), "%s", port_controller(port));
            }
            printf(GREEN "[OP]---" RESET "%d\t%s\n", port, banner);
        } else if (show_closed) {
            printf(RED "[CL]---" RESET "%d\n", port);
        }
    }

    printf(YELLOW BOLD "\n[SM]" RESET " Scanning Completed.\n");
    return 0;
}

char* port_controller(int port) {
    switch (port) {
        case 21:  return "FTP";
        case 22:  return "SSH";
        case 23:  return "Telnet";
        case 25:  return "SMTP";
        case 53:  return "DNS";
        case 80:  return "HTTP";
        case 110: return "POP3";
        case 143: return "IMAP";
        case 443: return "HTTPS";
        case 3306: return "MySQL";
        case 3389: return "RDP";
        case 5900: return "VNC";
        case 8080: return "HTTP Alt";
        default:  return "Other";
    }
}
