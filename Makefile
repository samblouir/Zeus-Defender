CC = gcc
XML_LIBS = `xml2-config --libs`
CFLAGS = `xml2-config --cflags` -DDEBUG -Wall -g
SRC_DIR = ./src
INC_DIR = ./inc
SRCS = $(wildcard $(SRC_DIR)/*.c)

all: $(SRCS)
	$(CC) $(CFLAGS) -I$(INC_DIR) -o pitcher $^ $(XML_LIBS)
	@echo "Finished making pitcher."
