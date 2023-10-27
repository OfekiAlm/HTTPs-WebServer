#ifndef HTTPS_WEBSERVER_SSL_H
#define HTTPS_WEBSERVER_SSL_H
#include <openssl/ssl.h>
#include <openssl/err.h>

void initialize_ssl();
SSL_CTX* create_context();
void configure_context(SSL_CTX *ctx);

#endif //HTTPS_WEBSERVER_SSL_H
