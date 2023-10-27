#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "ssl/ssl.h"

#define PORT 8080
#define BUFFER_SIZE 1024
 
int main(){
        char buffer[BUFFER_SIZE];
        char ErrorResp[] = "HTTP/1.0 404 Not Found\r\n"
                "Server:my-webserver-in-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html><h1>Not found Your request. probably /favicon.ico</h1></html>\r\n";
 
 
        char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server:my-webserver-in-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>Hello world!</html>\r\n";
 
        printf("Made by Ofek Almog - WebServer in C.\n");
        
        // create a socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1){
                perror("webserver (socket)");
                return 1;
        }
        printf("[] socket created successfully\n");
        
        struct sockaddr_in host_addr;
        int host_addrlen = sizeof(host_addr);

        initialize_ssl();
        SSL_CTX *ctx = create_context();
        configure_context(ctx);
        SSL *ssl = SSL_new(ctx);

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PORT);
        host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
        // Create client address
        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
 
        if(bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0){
                perror("webserver (bind)");
                return 1;
        }
        printf("[] socket successfully bound to address\n");
 
        if(listen(sockfd, SOMAXCONN) != 0){
                perror("webserver (listen)");
                return 1;
        }
        
        printf("[] socket listening for connections 128 (MAX CONNECTIONS)\n");
        for(;;){
            int newsockfd = accept(sockfd,(struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
            SSL_set_fd(ssl, newsockfd);
            if(newsockfd == -1){
                    perror("webserver (accept)");
                    return 1;
            }
            printf("[] socket accepted a new connection\n");

            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
            }

            int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr,
                                            (socklen_t *)&client_addrlen);
            if (sockn < 0) {
                    perror("webserver (getsockname)");
                    continue;
            }


            //try to read data from request.
            int readVal = SSL_read(ssl,buffer, sizeof(buffer));
            if(readVal < 0){
                    perror("webserver (read)");
                    continue;
            }

            //prints the user ip&port and request data.
            char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
            sscanf(buffer, "%s %s %s", method, uri, version);

            printf("[%s:%u]\nThis is the method %s\nVERSION: %s\nURI: %s\n",
                            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
                            method, version, uri);
            int writeVal = 0;
            if(strcmp(uri,"/favicon.ico") == 0){
                writeVal = SSL_write(ssl, ErrorResp ,strlen(ErrorResp));
            }
            else{
                writeVal = SSL_write(ssl, resp ,strlen(resp));
                //writeVal = serve_file(ssl,"s.html");
            }

            if(writeVal < 0){
                        perror("webserver (write)");
                        continue;
            }

            SSL_shutdown(ssl);
            SSL_free(ssl);

            close(newsockfd);


            //clean up

            SSL_CTX_free(ctx); //context
            EVP_cleanup(); //cleanup ssl lib

        }
        return 0;
}

int serve_file(SSL *ssl, const char *filename) {
    char buffer[1024];
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        // File not found, send 404
        SSL_write(ssl, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n", 47);
        SSL_write(ssl, "<html><body><h1>404 Not Found</h1></body></html>", 49);
        return -1;
    } else {
        // File found, send file content
        SSL_write(ssl, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n", 45);
        size_t in;
        while ((in = fread(buffer, 1, sizeof(buffer), file) > 0)) {
            SSL_write(ssl, file, in);
        }
        fclose(file);
    }
    return 0;
}

