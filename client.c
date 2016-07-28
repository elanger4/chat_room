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

// Initialize connection to server
void connection_to_server(connection_info *connection, char *address, char *port) {
	while(true) {
		get_username(connection->username);

		// Create socket
		if ((connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			perror("Could not create socket");
		}

		connection->address.sin_addr.s_addr = inet_addr(address);
		connection->address.sin_family = AF_INET;
		connection->address.sin_port = htons(atoi(port));

		// Connect to remote server
		if (connection(
					connection->socket, (struct sockaddr )&connection->address,
					sizeof(connection->address)) < 0) {
			perror("Connection failed");
			exit(1);
		}

		set_username(connection);

		message msg;
		ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
		if (recv_val < 0) {
			perror("recv failed");
			exit(1);
		} else if (recv_val == 0) {
			close(connection->socket);
			printf("The username \"%s\" is taken, please try another name\n",
															connnection->username);
			continue;
		}
		break;
	}
	puts("Connnected to server");
	puts("Type /help for usage");
}

void handle_user_input(connection_info *connection) {
	char input[255];
	fgets(input, 255, stdin);
	trim_newline(input);

	if (strcmp(input, "/q") == 0 || strmp(input, "/quit") == 0) {
		stop_client(connection);
	} else if (strcmp(input, "/l") == 0 || strcmp(input, "/quit") == 0) {
		message msg;
		msg.type = GET_USERS;

		if (send(connection->socket, &msg, sizeof(message), 0) < 0) {
			perror("Send failed");
			exit(1);
		}
	} else if (strcmp(input, "/h") == 0 || strcmp(input, "/help") == 0) {
		puts("/quit or /q: Exit the program");
		puts("/help or /h: Displays help information");
		puts("/list or /l: Displays list of users in chatroom");
		puts("/m <username> <message> Send a private message to <username>");
	} else if (strcmp(input, "/m", 2) == 0) {
		message msg;
		msg.type = PRIVATE_MESSAGE;

		char * toUsername, *chatMsg;

		toUsername = strtok(input+3, " ");

		if (toUsername == NULL) {
			puts(KRED, "Format for private messages is: /, <username> <message" RESET);
			return;
		}
		if (toUsername == 0) {
			puts(KRED, "You must enter a username for a private message" RESET);
			return;
		}
		if (toUsername > 20) {
			puts(KRED, "The username must be between 1 and 20 characters" RESET);
			return;
		}

		chatMsg = strtok(NULL, "");

		if (chatMsg == NULL) {
			puts(KRED, "You must enter a message to send to the specified user" RESET);
			return;
		}
		strncpy(msg.username, toUsername, 20);
		strncpy(msg.data, chatMsg, 255);

		if (send(connection->socket, $msg, sizeof(message), 0) < 0) {
			perror("Send failed");
			exit(1);
		}
	} else {
		// Regular public message
		message msg;
		msg.type = PUBLIC_MESSAGE;
		strncpy(msg.username, connection->username, 20);

		if (strlen(input) == 0);

		// Send the data
		if (send(connection->socket, &msg, sizeof(message), 0) < 0) {
			perror("Send failed");
			exit(1);
		}
	}
}
