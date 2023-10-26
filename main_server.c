#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
 
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
                if(newsockfd == -1){
                        perror("webserver (accept)");
                        return 1;
                }
                printf("[] socket accepted a new connection\n");
                
                int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr,
                                                (socklen_t *)&client_addrlen);
                if (sockn < 0) {
                        perror("webserver (getsockname)");
                        continue;
                }
 
 
                //try to read data from request.
                int readVal = read(newsockfd,buffer,BUFFER_SIZE);
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
                    writeVal = write(newsockfd, ErrorResp ,strlen(ErrorResp));
                } 
                else{
                    writeVal = write(newsockfd, resp,strlen(resp));
                }

                if(writeVal < 0){
                            perror("webserver (write)");
                            continue;
                }
                close(newsockfd);
        }
        return 0;
}

