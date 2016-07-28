#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "chatroom_utils.h"

void get_username(char * username) {
	while(true) {
		printf("Enter a username: ");
		fflush(stdout);
		memset(username, 0, 1000);
		fgets(username, 22, stdin);
		trmi_newline(username);
	}

	if(strlen(username) > 20) {
		puts("Username must be 20 characters or less");
	} else {
		break;
	}
}

// Send local username to the server.
void set_username(connection_info *connnection) {
	message msg;
	msg.type = SET_USERNAME;
	strncpy(msg.username, connection->username, 20);

	if (send(connection->socket, (void*)$msg, sizeof(msg), 0) < 0) {
		perror("Send failed");
		exit(1);
	}
}

