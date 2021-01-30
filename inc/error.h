#ifndef ERROR_H
#define ERROR_H

#define SUCCESS_STR "Success.\n"
#define FAIL_STR "Fail.\n"
#define READ_ERROR_STR "Read error.\n"
#define WRITE_ERROR_STR "Write error.\n"
#define XML_COPY_ERROR_STR "XML copy error.\n"
#define XML_ADD_CHILD_ERROR_STR "XML add child error.\n"
#define XML_READ_FILE_ERROR_STR "XML read file error.\n"
#define XML_TO_STRING_ERROR_STR "XML to string conversion error.\n"
#define XML_NEW_ROOT_CREATION_ERROR_STR "XML new root creation error.\n"
#define XML_DOC_CREATION_ERROR_STR "XML document creation error.\n"
#define SOCKET_CREATION_ERROR_STR "Socket creation error.\n"
#define SOCKET_BIND_ERROR_STR "Socket bind error.\n"
#define SOCKET_IP_INVALID_ERROR_STR "Invalid IP address for server.\n"
#define SOCKET_CONNECTION_ERROR_STR "Socket connection error.\n"
#define SOCKET_SEND_MSG_ERROR_STR "Error while sending message to server.\n"
#define SOCKET_RECV_MSG_ERROR_STR "Error while receiving message from client.\n"
#define UNKNOWN_ERROR_STR "Unknown error code - %d.\n"

// Types of errors
typedef enum  NPResult
{
    NP_SUCCESS = 0,
    NP_FAIL,
    NP_READ_ERROR,
    NP_WRITE_ERROR,
    NP_XML_COPY_ERROR,
    NP_XML_ADD_CHILD_ERROR,
    NP_XML_READ_FILE_ERROR,
    NP_XML_TO_STRING_ERROR,
    NP_XML_NEW_ROOT_CREATION_ERROR,
    NP_XML_DOC_CREATION_ERROR,
    NP_SOCKET_CREATION_ERROR,
    NP_SOCKET_BIND_ERROR,
    NP_SOCKET_IP_INVALID_ERROR,
    NP_SOCKET_CONNECTION_ERROR,
    NP_SOCKET_SEND_MSG_ERROR,
    NP_SOCKET_RECV_MSG_ERROR
} NPResult;

// Checks if an error occurred and prints it out.
void print_err(NPResult result, const char *func);

#endif
