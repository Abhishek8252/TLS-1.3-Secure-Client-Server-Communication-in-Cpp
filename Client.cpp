//2. TLS_AES_Comm client.cpp
#include <iostream>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <unistd.h>

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    exit(1);
}

void encrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX **ctx, unsigned char *key, unsigned char *iv)
{
    if (!(*ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if (1 != EVP_EncryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
        handleErrors();
}

void encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int pt_len,
             unsigned char *ciphertext, int *ct_len, unsigned char *tag)
{
    int len;
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, pt_len))
        handleErrors();
    *ct_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    *ct_len += len;

    // Retrieve the 16-byte tag
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();
}

int main()
{
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(8080)};
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) > 0)
    {
        std::cout << "Connected to Server. Type messages below (type 'exit' to quit):\n";

        std::string input;
        while (true)
        {
            std::cout << "> ";
            std::getline(std::cin, input);
            if (input == "exit")
                break;

            unsigned char key[] = "01234567890123456789012345678901";
            unsigned char iv[] = "012345678901";
            unsigned char ciphertext[1024] = {0};
            unsigned char tag[16] = {0};
            
            int ct_len = 0;
            EVP_CIPHER_CTX *enc_ctx;

            encrypt_EVP_aes_256_gcm_init(&enc_ctx, key, iv);
            encrypt(enc_ctx, (unsigned char *)input.c_str(), input.length(), ciphertext, &ct_len, tag);

            // Send Data
            SSL_write(ssl, ciphertext, ct_len);
            SSL_write(ssl, tag, 16);

            EVP_CIPHER_CTX_free(enc_ctx);
        }
    }
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
