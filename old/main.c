#include <ctype.h>
#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

int socket_desc;
char *host;

static char* skip(char *s, char c) {
	while(*s != c && *s != '\0')
		s++;
	if(*s != '\0')
		*s++ = '\0';
	return s;
}

static void trim(char *s) {
	char *e;

	for (e = s + strlen(s); e > s && isspace((unsigned char)*(e - 1)); e--)
		;
	*e = '\0';
}

static void sout(char *fmt, ...) {
	char message[512];

	va_list ap;

	va_start(ap, fmt);
	vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);
	if( send(socket_desc , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
	}
}

static void parsesrv(char *cmd) {
        char *usr, *par, *txt;

        usr = host;
        if(!cmd || !*cmd)
                return;
        if(cmd[0] == ':') {
                usr = cmd + 1;
                cmd = skip(usr, ' ');
                if(cmd[0] == '\0')
                        return;
                skip(usr, '!');
        }
        skip(cmd, '\r');
        par = skip(cmd, ' ');
        txt = skip(par, ':');
        trim(par);
        if(!strcmp("PONG", cmd))
                return;
        if(!strcmp("PING", cmd))
                sout("PONG %s", txt);
}

int main(int argc , char *argv[])
{
	struct sockaddr_in server;
	char server_reply[2000];

	host = argv[1];

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr(host);
	server.sin_family = AF_INET;
	server.sin_port = htons( 6667 );

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected\n");

	/*
	//Send some data
	message = "CAP LS 30\r\n";
	if( send(socket_desc , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("CAP Send\n");
	*/

	/*
	message = "NICK anAnnoyingNerd\r\n";
	if( send(socket_desc , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	*/
	sout("NICK %s\r\n", argv[2]);
	puts("NICK Send\n");

	/*
	message = "USER anAnnoyingNerd 0 * :Zane\r\n";
        if( send(socket_desc , message , strlen(message) , 0) < 0)
        {
                puts("Send failed");
                return 1;
        }
	*/
	sout("USER %s 0 * :Zane\r\n", argv[2]);
        puts("USER Send\n");

	/*
	message = "CAP END\r\n";
        if( send(socket_desc , message , strlen(message) , 0) < 0)
        {
                puts("Send failed");
                return 1;
        }
        puts("CAP Send\n");
	*/

	//Receive a reply from the server
	while(1) {
		if( recv(socket_desc, server_reply , 2000 , 0) < 0)
		{
			puts("recv failed");
		}
		puts("Reply received\n");
		puts(server_reply);
		parsesrv(server_reply);
		/*
		if (strncmp(server_reply, "PING", 4) == 0) {
			char *colon_pos = strchr(server_reply, ':');
			if (colon_pos == NULL) {
				printf("Invalid PING format\n");
        			return 1;
			}

    			// Extract the argument from after the colon
    			char *ping_argument = colon_pos + 1;

			char pong_response[100];
			snprintf(pong_response, sizeof(pong_response), "PONG :%s\r\n", ping_argument);
			printf("%s", pong_response);
			if( send(socket_desc , message , strlen(message) , 0) < 0)
        		{
                		puts("Send failed");
                		return 1;
        		}
        		puts("PONG Send\n");
		}
		*/
	}
	return 0;
}
