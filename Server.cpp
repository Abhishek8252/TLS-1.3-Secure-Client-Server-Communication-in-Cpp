//2. TLS_AES_Comm server.cpp
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <unistd.h>
#include <arpa/inet.h>

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    exit(1);
}

void decrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX **ctx, unsigned char *key, unsigned char *iv)
{
    if (!(*ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if (1 != EVP_DecryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
        handleErrors();
}

int decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ct_len,
            unsigned char *tag, unsigned char *plaintext, int *pt_len)
{
    int len;
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ct_len))
        handleErrors();
    *pt_len = len;

    // Set expected tag before finalizing
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    // Final_ex returns 1 only if the tag matches
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return -1;

    *pt_len += len;
    return 1;
}

int main()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    std::cout << "Server listening on 8080...\n";

    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);

        if (SSL_accept(ssl) > 0)
        {
            std::cout << "\n[New Session Started]\n";

            // Loop to receive multiple messages from the same client
            while (true)
            {
                unsigned char buffer[1024] = {0};
                unsigned char tag[16] = {0};

                int ct_bytes = SSL_read(ssl, buffer, sizeof(buffer));
                if (ct_bytes <= 0)
                    break; // Client disconnected or error

                SSL_read(ssl, tag, 16);

                unsigned char key[] = "01234567890123456789012345678901";
                unsigned char iv[] = "012345678901";
                unsigned char plaintext[1024] = {0};

                int pt_len = 0;
                EVP_CIPHER_CTX *dec_ctx;

                decrypt_EVP_aes_256_gcm_init(&dec_ctx, key, iv);
                if (decrypt(dec_ctx, buffer, ct_bytes, tag, plaintext, &pt_len) > 0)
                {
                    plaintext[pt_len] = '\0';
                    std::cout << "Encrypted message: " << buffer << std::endl;
                    std::cout << "Decrypted Message: " << plaintext << "\n"
                              << std::endl;
                }

                EVP_CIPHER_CTX_free(dec_ctx);
            }
        }
        std::cout << "[Client Disconnected]\n";
        SSL_free(ssl);
        close(client_fd);
    }
    SSL_CTX_free(ctx);
    return 0;
}
