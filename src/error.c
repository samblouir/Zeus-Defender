#include <stdio.h>
#include "error.h"

// Prints out the correct error message.
void print_err(NPResult result, const char *func) {
    printf("%s: ", func);
    if(result == NP_SUCCESS) {
        printf(SUCCESS_STR);
    } else if(result == NP_FAIL) {
        printf(FAIL_STR);
    } else if(result == NP_READ_ERROR) {
        printf(READ_ERROR_STR);
    } else if(result == NP_WRITE_ERROR) {
        printf(WRITE_ERROR_STR);
    } else if(result == NP_XML_COPY_ERROR) {
        printf(XML_COPY_ERROR_STR);
    } else if(result == NP_XML_ADD_CHILD_ERROR) {
        printf(XML_ADD_CHILD_ERROR_STR);
    } else if(result == NP_XML_READ_FILE_ERROR) {
        printf(XML_READ_FILE_ERROR_STR);
    } else if(result == NP_XML_TO_STRING_ERROR) {
        printf(XML_TO_STRING_ERROR_STR);
    } else if(result == NP_XML_NEW_ROOT_CREATION_ERROR) {
        printf(XML_NEW_ROOT_CREATION_ERROR_STR);
    } else if(result == NP_XML_DOC_CREATION_ERROR) {
        printf(XML_DOC_CREATION_ERROR_STR);
    } else if(result == NP_XML_SCHEMA_ERROR){
        printf(XML_SCHEMA_ERROR_STR);
    } else if(result == NP_SOCKET_CREATION_ERROR) {
        printf(SOCKET_CREATION_ERROR_STR);
    } else if(result == NP_SOCKET_BIND_ERROR) {
        printf(SOCKET_BIND_ERROR_STR);
    } else if(result == NP_SOCKET_IP_INVALID_ERROR) {
        printf(SOCKET_IP_INVALID_ERROR_STR);
    } else if(result == NP_SOCKET_CONNECTION_ERROR) {
        printf(SOCKET_CONNECTION_ERROR_STR);
    } else if(result == NP_SOCKET_SEND_MSG_ERROR) {
        printf(SOCKET_SEND_MSG_ERROR_STR);
    } else if(result == NP_SOCKET_RECV_MSG_ERROR) {
        printf(SOCKET_RECV_MSG_ERROR_STR);
    } else if(result == NP_FILE_OPEN_ERROR) {
        printf(FILE_OPEN_ERROR_STR);
    } else {
        printf(UNKNOWN_ERROR_STR, result);
    }
}
