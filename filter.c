
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/socket"

//static xmlDocPtr schemaDocPtr = NULL;

int main(void){
    
    int fd, len, rc;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    char buf[2048];
//    char str[99999999]="";
    
    
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 2048);
    
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0){
        printf("SOCKET ERROR = %d", sock_errno());
        goto end;
 //       exit(1);
        
    }
    
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, "ZD", sizeof(server_sockaddr.sun_path)); 
    
    len = sizeof(server_sockaddr);
    unlink(SOCK_PATH);
    
    rc = bind(fd, (struct sockaddr *) &server_sockaddr, len);
    if (rc < 0){
        printf("BIND ERROR = %d", sock_errno());
        close(fd);
 //       exit(1);
    }
    FILE *fPtr;
    while(1){
        memset(buf, 0, 2048);
        bytes_rec = recv(fd, buf, 2048, 0);
        if (bytes_rec < 0){
            printf("RECV ERROR = %d", sock_errno());
            close(server_sock);
            break;
        } else {
            printf("DATA RECEIVED = %s\n", buf);
            
            fPtr = fopen("packet.xml", "a");
            fputs(buf, fPtr);
            
           // strcat(str,buf);
        }
    }
    
    //Creating xmlDoc for both the packet and xml schemea 
    xmlDocPtr packetDocPtr;
    xmlDocPtr schemaDocPtr;
    
    //xml filenames
    char * xmlFilename = "packet.xml";
    char * schemaFilename = "schema.xsd";
    
    //creating schema context for validating 
    xmlSchemaParserCtxtPtr schemaParserCtxt;
    xmlSchemaPtr schema;
    xmlSchemaValidCtxtPtr validCtxt;
    
    //error checking
    packetDocPtr = xmlReadFile(xmlFilename, NULL, XML_PARSE_NONET);
    if (!packetDocPtr){
        goto end;
    }
        
    schemaDocPtr = xmlReadFile(schemaFilename, NULL, XML_PARSE_NONET);
    if (!schemaDocPtr){
        goto end;
    }

    schemaParserCtxt = xmlSchemaNewDocParserCtxt(schemaDocPtr);
    if (!schemaParserCtxt){
        goto end;
    }

    schema = xmlSchemaParse(schemaParserCtxt);
    if (!schema){
        goto end;
    }

    validCtxt = xmlSchemaNewValidCtxt(schema);
    if(!validCtxt){
        goto end;
    }

    //XML validation
    xmlSchemaFreeParserCtxt(schemaParserCtxt);
    xmlFreeDoc(schemaDocPtr);

    int valid = 0;
    valid = xmlSchemaValidateDoc(validCtxt, packetDocPtr);
    if (valid == 0){
        printf("Valid Doc \n");
     }else{
       printf("Invalid Doc \n");
    }
    
    //freeing mem
    close(server_sock);
    xmlSchemaFree(schema);
    xmlFreeDoc(packetDocPtr);    
    fclose(fPtr);
    
end: 
    close(server_sock);
    printf("Error has occured\n");
    xmlSchemaFree(schema);
    xmlFreeDoc(packetDocPtr);    
    fclose(fPtr);
    return 0;
}
