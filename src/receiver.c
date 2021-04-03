#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "comms.h"
#include "error.h"

#define USAGE_STR           ("Usage: ./receiver <Pitcher's IP address> <PORT>\n")
#define GETOPT_OPTSTRING    (":f:h")
#define CS_XML_FILE         ("XML_FILE")
#define CS_PITCHER_ADDRESS  ("ADDRESS")
#define CS_PITCHER_PORT     ("PORT")


#define UNUSED(x) (unused(&(##x)))
void unused(void *x) {  }

typedef struct _config {
    char *xml_filename,
         *pitcher_addr;
    int pitcher_port;
} config;

int handle_var(config *conf, char *name, char *value) {
    int res = 1;
    if (strcmp(name, CS_XML_FILE) == 0) {
        conf->xml_filename = strdup(value);
    }
    else if (strcmp(name, CS_PITCHER_ADDRESS) == 0) {
        conf->pitcher_addr = strdup(value);
    }
    else if (strcmp(name, CS_PITCHER_PORT) == 0) {
        // TODO: check if this is good
        // conf->pitcher_port = value;
        sscanf(value, "%d", &(conf->pitcher_port));
    }
    else {
        res = 0; // TODO: better error handling
    }
    return res;
}

int config_file(char *filename, config *conf) {
    int res = 1;
    FILE *fp;
    char *line = NULL, *name, *value;
    size_t len;
    ssize_t read;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        goto end;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[read-1] == '\n') // TODO: trim newline better
            line[read-1] = '\0';
        if (line[0] != '#') {
            name = strtok(line, "=");
            value = strtok(NULL, "=");
            handle_var(conf, name, value);
        }
    }
end:
    free(line);
    fclose(fp);
    return res;
}

// Main function
int main(int argc, char **argv) {
    
    NPResult result = NP_SUCCESS;
    int fd = 0;
    // int port = 0;
    int opt = 0;
    char *filename = NULL;
    config conf = {};
    
    while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
        switch (opt)
        {
        case 'f':
            filename = optarg;
            break;
        case ':':
            printf("option needs a value\n");
            break;
        case 'h':
            printf(USAGE_STR);
        default:
            break;
        }
    }
    // goto end;

    if (filename != NULL) {
        config_file(filename, &conf);
        printf("config { %s, %s, %d }\n", conf.xml_filename, conf.pitcher_addr, conf.pitcher_port);
        return 0;
    }
    // Check arguments
    else if(argc != 3) {
        printf(USAGE_STR);
        result = NP_FAIL;
        goto end;
    }
    // sscanf(argv[2], "%d", &port);

    // Create a server for listening
    fd = create_server(conf.pitcher_addr, conf.pitcher_port);
    if(fd < 0) {
        result = NP_FAIL;
        goto end;
    }

    // Receive the XML data
    recv_xml(fd);

end:
    if(result != NP_SUCCESS) {
        print_err(result, "main()");
    }
    return result;
}
