#ifndef ERROR_H
#define ERROR_H

#define SUCCESS_STR "success.\n"
#define FAIL_STR "fail.\n"
#define READ_ERROR_STR "read error.\n"
#define WRITE_ERROR_STR "write error.\n"
#define XML_COPY_ERROR_STR "XML copy error.\n"
#define XML_ADD_CHILD_ERROR_STR "XML add child error.\n"
#define XML_READ_FILE_ERROR_STR "XML read file error.\n"
#define XML_TO_STRING_ERROR_STR "XML to string conversion error.\n"
#define UNKNOWN_ERROR_STR "unknown error code - %d.\n"

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
    NP_XML_TO_STRING_ERROR
} NPResult;

// Checks if an error occurred and prints it out.
void printErr(NPResult result, const char *func);

#endif
