/*
 * ssl_platom.h
 *
 *  Created on: Sep 7, 2015
 *      Author: liuhan
 */

#ifndef SSL_PLATOM_H_
#define SSL_PLATOM_H_

#include "ssl/ssl_os_port.h"
#include "ssl/ssl_ssl.h"
#include "ssl/ssl_tls1.h"

#include "ssl/ssl_bigint_impl.h"
#include "ssl/ssl_crypto.h"

typedef void * (*ssl_func_type_t)(void);
typedef void * (*bio_func_type_t)(void);

typedef struct
{
    ssl_func_type_t ssl_func_type;
} PLATOM_CTX;

#define PLATOM_CTX_ATTR  ((PLATOM_CTX *)ssl_ctx->bonus_attr)

/*encapsulation the structure based on the espressif platform*/
struct	_MD_CTX
{
    unsigned char cksum[16];    /* checksum of the data block */
    unsigned char state[48];    /* intermediate digest state */
    unsigned char buffer[16];   /* data block being processed */
    int left;                   /* amount of data in buffer */
};

typedef struct _MD_CTX 			EVP_MD_CTX;
typedef unsigned char			EVP_MD;
typedef struct _x509_ctx 		X509;
typedef struct _x509_ctx 		X509_STORE_CTX;
//typedef struct _SSL 			SSL;
//typedef struct _SSL_CTX 		SSL_CTX;

#ifdef OPENSSL_BIGNUM
typedef bigint	BIGNUM;
typedef BI_CTX	BN_CTX;
typedef BI_CTX	BN_MONT_CTX;
typedef uint64 BN_ULONG;
#endif

#define ESP_SSL_ERROR_NONE					0
#define ESP_SSL_ERROR_WANT_WRITE			1
#define ESP_SSL_ERROR_WANT_READ				2
#define ESP_SSL_ERROR_WANT_X509_LOOKUP		3
#define ESP_SSL_ERROR_SYSCALL				4
#define ESP_SSL_ERROR_ZERO_RETURN			5
#define ESP_SSL_ERROR_SSL					6
#define ESP_SSL_FILETYPE_PEM				10
#define ESP_SSL_VERIFY_PEER					11
#define ESP_EVP_MAX_MD_SIZE					6
#define ESP_SSL_VERIFY_FAIL_IF_NO_PEER_CERT	4

#define ESP_SSL_MODE_ENABLE_PARTIAL_WRITE	5		
#define ESP_SSL_VERIFY_NONE					12		
#define ESP_SSL_ERROR_WANT_CONNECT			13		
#define ESP_SSL_ERROR_WANT_ACCEPT			14		

/*
 * Enable a subset of espressif platom ssl compatible functions. We don't aim to be 100%
 * compatible - just to be able to do basic ports etc.
 *
 * Only really tested on mini_httpd, so I'm not too sure how extensive this
 * port is.
 */

/******************************************************************************
 * FunctionName : esp_EVP_DigestInit
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_EVP_DigestInit(MD5_CTX *ctx, uint8 *out);


/******************************************************************************
 * FunctionName : esp_EVP_DigestUpdate
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_EVP_DigestUpdate(MD5_CTX *ctx, const uint8_t *input, int ilen);


/******************************************************************************
 * FunctionName : esp_EVP_DigestFinal
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_EVP_DigestFinal(MD5_CTX *ctx, uint8_t *output, uint16* olen);


/******************************************************************************
 * FunctionName : esp_EVP_sha1
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char * esp_EVP_sha1(void);

/******************************************************************************
 * FunctionName : esp_EVP_cleanup
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char * esp_EVP_cleanup(void);

/******************************************************************************
 * FunctionName : esp_base64_encode
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_base64_encode(uint8 *dst, size_t dlen, size_t *olen,
    const uint8_t *src, size_t slen);

/******************************************************************************
 * FunctionName : esp_SSLv23_server_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_SSLv23_server_method(void);

/******************************************************************************
 * FunctionName : esp_SSLv3_server_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_SSLv3_server_method(void);

/******************************************************************************
 * FunctionName : esp_TLSv1_server_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_TLSv1_server_method(void);

/******************************************************************************
 * FunctionName : esp_TLSv1_1_server_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_TLSv1_1_server_method(void);

/******************************************************************************
 * FunctionName : esp_TLSv1_1_client_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_TLSv1_1_client_method(void);

/******************************************************************************
 * FunctionName : esp_SSLv23_client_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_SSLv23_client_method(void);

/******************************************************************************
 * FunctionName : esp_SSLv3_client_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_SSLv3_client_method(void);

/******************************************************************************
 * FunctionName : esp_TLSv1_client_method
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_TLSv1_client_method(void);

/******************************************************************************
 * FunctionName : esp_ssl_CTX_new
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
SSL_CTX * esp_ssl_CTX_new(ssl_func_type_t meth);

/******************************************************************************
 * FunctionName : esp_ssl_CTX_set_option
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_set_option(SSL_CTX *ssl_ctx, uint32_t options);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ssl_CTX_free(SSL_CTX *ssl_ctx);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
SSL * esp_ssl_new(SSL_CTX *ssl_ctx);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_set_fd(SSL *s, int fd);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_accept(SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_connect(SSL *ssl);


/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ssl_free(SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_read(SSL *ssl, void *buf, int num);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_write(SSL *ssl, const void *buf, int num);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_use_certificate_file(SSL_CTX *ssl_ctx, const char *file, int type);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_use_certificate_auth_file(SSL_CTX *ssl_ctx, const char *file, int type);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_use_PrivateKey_file(SSL_CTX *ssl_ctx, const char *file, int type);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_CTX_use_certificate_ASN1(SSL_CTX *ssl_ctx, int len, const uint8_t *d);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_CTX_set_session_id_context(SSL_CTX *ctx, const unsigned char *sid_ctx,
    unsigned int sid_ctx_len);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_use_certificate_chain_file(SSL_CTX *ssl_ctx, const char *file);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_shutdown(SSL *ssl);

/*** get/set session ***/
/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
SSL_SESSION * SSL_get1_session(SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_set_session(SSL *ssl, SSL_SESSION *session);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_SESSION_free(SSL_SESSION *session);
/*** end get/set session ***/

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
long SSL_CTX_ctrl(SSL_CTX *ctx, int cmd, long larg, void *parg);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ssl_CTX_set_verify(SSL_CTX *ctx, int mode,
    int (*verify_callback)(int, void *));

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ssl_CTX_set_verify_depth(SSL_CTX *ctx, int depth);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,
    const char *CApath);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * SSL_load_client_CA_file(const char *file);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_CTX_set_client_CA_list(SSL_CTX *ssl_ctx, void *file);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSLv23_method(void);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_CTX_set_default_passwd_cb(SSL_CTX *ctx, void *cb);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_CTX_set_default_passwd_cb_userdata(SSL_CTX *ctx, void *u);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_peek(SSL *ssl, void *buf, int num);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_set_bio(SSL *ssl, void *rbio, void *wbio);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
long esp_ssl_get_verify_result(const SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_state(SSL *ssl);

/** end of could do better list */
/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void * esp_ssl_get_peer_certificate(const SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_clear(SSL *ssl);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_check_private_key(const SSL_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int SSL_CTX_set_cipher_list(SSL *s, const char *str);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_get_error(const SSL *ssl, int ret);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_CTX_set_options(SSL_CTX *ssl_ctx, int option);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_library_init(void );

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void SSL_load_error_strings(void );

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ssl_CTX_set_default_verify_paths(SSL_CTX *ssl_ctx);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
X509_CTX * esp_X509_store_ctx_get_current_cert(X509_CTX * store);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_X509_free(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_X509_NAME_oneline(X509_CTX * x509_CTX);


/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char* esp_X509_get_issuer_name(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char* esp_X509_get_subject_name(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_X509_STORE_CTX_get_error_depth(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char* esp_X509_STORE_CTX_get_error(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
char* esp_X509_verify_cert_error_string(X509_CTX * x509_CTX);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_ERR_get_error(void);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ERR_error_string_n(uint32 error, char* out, uint32 olen);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ERR_error_string(uint32 error, char* out);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_ERR_free_strings(void);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_CRYPTO_cleanup_all_ex_data(void);

/******************************************************************************
 * FunctionName : esp_ssl_set_fd
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
const char* esp_ERR_strerror(uint32 error);

void esp_ssl_sleep(uint16 mseconds);

int esp_ssl_CTX_set_client_cert_cb();

int esp_ssl_CTX_set_mode();

int esp_ssl_pending();

bool esp_ssl_fragment_length_negotiation(SSL* ssl, int fragmet_level);

//////////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSSL_BIGNUM
/******************************************************************************
 * FunctionName : esp_BN_new
 * Description : creat the struct BIGNUM 
 * Parameters  : none
 * Returns   : the struct BIGNUM 
*******************************************************************************/
BIGNUM *esp_BN_new(void);

/******************************************************************************
 * FunctionName : esp_BN_free
 * Description : 释放一个BIGNUM
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_BN_free(BIGNUM *a);

/******************************************************************************
 * FunctionName : esp_BN_clear_free
 * Description : 将BN_free和BN_clear综合，要不就赋值0，要不就释放空间。
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_BN_clear_free(BIGNUM *a);

/******************************************************************************
 * FunctionName : esp_BN_bin2bn
 * Description : 赋二进制值s到ret中，返回ret
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
BIGNUM *esp_BN_bin2bn(const unsigned char *s,int len,BIGNUM *ret);

/******************************************************************************
 * FunctionName : esp_BN_set_word
 * Description : 设置a为w
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_set_word(BIGNUM *a, BN_ULONG w);

/******************************************************************************
 * FunctionName : esp_BN_bn2bin
 * Description : 取a为二进制到to中，返回字符串长度
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_bn2bin(const BIGNUM *a, unsigned char *to);
/******************************************************************************
 * FunctionName : esp_BN_bn2hex
 * Description : 取a的16进制值，返回一个字符串的指针。
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

char *esp_BN_bn2hex(const BIGNUM *a);
/******************************************************************************
 * FunctionName : esp_BN_num_bits
 * Description : 返回a的二进制位数
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

int esp_BN_num_bits(const BIGNUM *a);
/******************************************************************************
 * FunctionName : esp_BN_cmp
 * Description : 判断a与b是否相等
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

int esp_BN_cmp(const BIGNUM *a, const BIGNUM *b);
/******************************************************************************
 * FunctionName : esp_BN_lshift
 * Description : a左移n位，结果存于r
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

int esp_BN_lshift(BIGNUM *r, const BIGNUM *a, int n);

/******************************************************************************
 * FunctionName : esp_BN_copy
 * Description : 将b复制给a,正确返回a，错误返回NULL
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

BIGNUM *esp_BN_copy(BIGNUM *a, const BIGNUM *b);
/******************************************************************************
 * FunctionName : esp_BN_add
 * Description : 计算a与b的和，值储存在r中。如果成功，返回1；否则，返回0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/

int esp_BN_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

/******************************************************************************
 * FunctionName : esp_BN_add_word
 * Description : 大数a加上w，值储存在a中，返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_add_word(BIGNUM *a, BN_ULONG w);
/******************************************************************************
 * FunctionName : esp_BN_sub
 * Description : 计算a与b的差，值储存在r中。返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);

/******************************************************************************
 * FunctionName : esp_BN_sub_word
 * Description : 大数a减去w，值储存在a中，返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_sub_word(BIGNUM *a, BN_ULONG w);

/******************************************************************************
 * FunctionName : esp_BN_mul
 * Description : 计算a与b的积，值储存在r中。返回1或者0。ctx为一个上下文相关的结构，可以用BN_CTX_new与BN_CTX_free来创建与释放它
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_BN_mul_word
 * Description : 大数a乘以w，值储存在a中，返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_mul_word(BIGNUM *a, BN_ULONG w);

/******************************************************************************
 * FunctionName : esp_BN_div
 * Description : 计算m与d的商，值储存在dv中，余数储存在rem中。返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_div(BIGNUM *dv, BIGNUM *rem, const BIGNUM *m, const BIGNUM *d,BN_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_BN_div_word
 * Description : 大数a除以w，值储存在a中，返回余数
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
BN_ULONG esp_BN_div_word(BIGNUM *a, BN_ULONG w);

/******************************************************************************
 * FunctionName : esp_BN_mod
 * Description : 计算m与d的模，值储存在rem中。返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_mod(BIGNUM *rem, const BIGNUM *m, const BIGNUM *d,BN_CTX *ctx); 

/******************************************************************************
 * FunctionName : esp_BN_mod_word
 * Description : 大数a模w，返回余数
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
BN_ULONG esp_BN_mod_word(const BIGNUM *a, BN_ULONG w);

/******************************************************************************
 * FunctionName : esp_BN_mod_mul
 * Description : 计算a与b的积，再模m，值储存在r中。返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_mod_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b,	const BIGNUM *m, BN_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_BN_mod_exp
 * Description : 计算a的p次方，再模m，值储存在r中。返回1或者0
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_mod_exp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p,	const BIGNUM *m,BN_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_BN_is_prime
 * Description : 判断是否为素数，返回0表示成功，1表示错误概率小于0.25，-1表示错误
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_is_prime(const BIGNUM *p,int nchecks,void (*callback)(int,int,void *),BN_CTX *ctx,void *cb_arg);

/******************************************************************************
 * FunctionName : esp_BN_CTX_new
 * Description : creat the struct BN_CTX
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
BN_CTX *esp_BN_CTX_new(void);

/******************************************************************************
 * FunctionName : esp_BN_CTX_free
 * Description : free the struct BN_CTX
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_BN_CTX_free(BN_CTX *c);

/*
* 下面函数采用蒙哥马利算法进行模幂计算，可以提高效率，
* 主要应用于在同一模下进行多次幂运算 
*/
/******************************************************************************
 * FunctionName : esp_BN_MONT_CTX_new
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
BN_MONT_CTX *esp_BN_MONT_CTX_new(void );

/******************************************************************************
 * FunctionName : esp_BN_MONT_CTX_set
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int esp_BN_MONT_CTX_set(BN_MONT_CTX *mont,const BIGNUM *mod,BN_CTX *ctx);

/******************************************************************************
 * FunctionName : esp_BN_MONT_CTX_free
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
void esp_BN_MONT_CTX_free(BN_MONT_CTX *mont);

/******************************************************************************
 * FunctionName : esp_BN_mod_exp_mont
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_mod_exp_mont(BIGNUM *r, const BIGNUM *a, const BIGNUM *p,const BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *m_ctx);

/******************************************************************************
 * FunctionName : esp_BN_mod_exp_mont_word
 * Description : sent data for client or server
 * Parameters  : s -- espconn to set for client or server
 *        fd -- data to send
 *        length -- length of data to send
 * Returns   : none
*******************************************************************************/
int	esp_BN_mod_exp_mont_word(BIGNUM *r, BN_ULONG a, const BIGNUM *p,const BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *m_ctx);
#endif

#endif /* SSL_PLATOM_H_ */
