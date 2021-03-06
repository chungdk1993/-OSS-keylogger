#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define GET 100
#define SEND 101
#define FAIL_SEND 102
#define EXIT_CMD 900
#define EXIT_PROCESS 901
#define MAX 1024


void error_handling(char *message);



int main(int argc, char *argv[])
{
	int server_sock;
	int client_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	char message[] = "Hello!";
	char resp[MAX];

	FILE *fp;


	char file[MAX]; // string from fopen
	char readmsg[MAX];
	char *tmp;
	char *split;
	int len;
	int n, i = 0;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	server_sock = socket(PF_INET, SOCK_STREAM, 0); /* create server socket */
	if (server_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) /* assign the address to server */
		error_handling("bind() error");
	for(;;)
	{
	if (listen(server_sock, 5) == -1)  /* enter the status of waiting listen mode */
		error_handling("listen() error");



		clnt_addr_size = sizeof(clnt_addr);
		client_sock = accept(server_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size); /* accept connection */

		if (client_sock == -1)
			error_handling("accept() error");

		for (;;) // execute CMD
		{
			len = 0;
			tmp = readmsg;
			while ((n = read(client_sock, tmp, 1))>0) // Get the CMD
			{
				if (*tmp == '\r') continue;
				if (*tmp == '\0') break;
				if (*tmp == '\n') break;

				if (len == MAX) break;

				tmp++;
				len++;
			}

			readmsg[len] = '\0';
			if (strcmp(readmsg, "GET") == 0)
			{
				printf("\nSend Message>>\n");
				fp = fopen("test_log", "r"); // file open
				while (!feof(fp)) // To EOF fp
				{
					write(client_sock, "OK", 3); // send msg
					read(client_sock, resp, sizeof(resp) - 1); // read respone
					if (strcmp(resp, "SEND") == 0)
					{

						fgets(file, MAX, fp);
						split = file;
						if (strlen(split) == 0)
							write(client_sock, "EOF", 4); 
						write(client_sock, file, strlen(file) + 1);
						memset(file, '\0', sizeof(file));
					}
				}
				fclose(fp);
			}
			else if(strcmp(readmsg, "END")==0)
				break;

		}
		close(client_sock);
	}
		//return 0;
}



void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}
