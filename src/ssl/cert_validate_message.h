/*
 */

#ifndef SQUID_SSL_CERT_VALIDATE_MESSAGE_H
#define SQUID_SSL_CERT_VALIDATE_MESSAGE_H

#include "ssl/support.h"
#include "ssl/crtd_message.h"
#include <vector>

namespace Ssl
{

/**
 * This class is used to hold the required informations to build
 * a request message for the certificate validator helper
 */
class CertValidationRequest
{
public:
    SSL *ssl;
    Errors *errors; ///< The list of errors detected
    std::string domainName; ///< The server name
    CertValidationRequest() : ssl(NULL), errors(NULL) {}
};

/**
 * This class is used to store informations found in certificate validation
 * response messages read from certificate validator helper
 */
class CertValidationResponse
{
public:
    /**
     * This class used to hold error informations returned from
     * cert validator helper.
     */
    class  RecvdError
    {
    public:
        RecvdError(): id(0), error_no(SSL_ERROR_NONE), cert(NULL) {}
        RecvdError(const RecvdError &);
        RecvdError & operator = (const RecvdError &);
        void setCert(X509 *);  ///< Sets cert to the given certificate
        int id; ///<  The id of the error
        ssl_error_t error_no; ///< The OpenSSL error code
        std::string error_reason; ///< A string describing the error
        X509_Pointer cert; ///< The broken certificate
    };

    typedef std::vector<RecvdError> RecvdErrors;

    /// Search in errors list for the error item with id=errorId.
    /// If none found a new RecvdError item added with the given id;
    RecvdError &getError(int errorId);
    RecvdErrors errors; ///< The list of parsed errors
    HelperReply::Result_ resultCode; ///< The helper result code
};

/**
 * This class is responsible for composing or parsing messages destined to
 * or comming from a cert validator helper.
 * The messages format is:
 *   <response/request code> <whitespace> <body length> <whitespace> <key=value> ...\1
 */
class CertValidationMsg: public CrtdMessage
{
private:
    /**
     * This class used to hold the certId/cert pairs found
     * in cert validation messages.
     */
    class CertItem
    {
    public:
        std::string name; ///< The certificate Id to use
        X509_Pointer cert;       ///< A pointer to certificate
        CertItem(): cert(NULL) {}
        CertItem(const CertItem &);
        CertItem & operator = (const CertItem &);
        void setCert(X509 *); ///< Sets cert to the given certificate
    };

public:
    CertValidationMsg(MessageKind kind): CrtdMessage(kind) {}

    /// Build a request message for the cert validation helper
    /// using informations provided by vcert object
    void composeRequest(CertValidationRequest const &vcert);

    /// Parse a response message and fill the resp object with parsed informations
    bool parseResponse(CertValidationResponse &resp, STACK_OF(X509) *peerCerts, std::string &error);

    /// Search a CertItems list for the certificate with ID "name"
    X509 *getCertByName(std::vector<CertItem> const &, std::string const & name);

    /// String code for "cert_validate" messages
    static const std::string code_cert_validate;
    /// Parameter name for passing intended domain name
    static const std::string param_domain;
    /// Parameter name for passing SSL errors
    static const std::string param_error;
    /// Parameter name for passing SSL certificates
    static const std::string param_cert;
    /// Parameter name for passing the major SSL error
    static const std::string param_error_name;
    /// Parameter name for passing the error reason
    static const std::string param_error_reason;
    /// Parameter name for passing the error cert ID
    static const std::string param_error_cert;
};

}//namespace Ssl
#endif // SQUID_SSL_CERT_VALIDATE_MESSAGE_H
