#include <stdio.h>
#include "error.h"

void printErr(NPResult result, const char *func) {
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
    } else {
        printf(UNKNOWN_ERROR_STR, result);
    }
}
