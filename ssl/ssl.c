#include "ssl.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

void initialize_ssl(){
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}