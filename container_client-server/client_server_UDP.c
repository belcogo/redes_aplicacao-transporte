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

void sending(char linha[ECHOMAX], int PORT, int sock);
void receiving(int server_fd);
void *receive_thread(void *server_fd);

int main(int argc, char const *argv[])
{
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
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//Printed the server socket addr and port
	printf("Endereço IP: %s\n", inet_ntoa(address.sin_addr));
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
	char ch[ECHOMAX];
	pthread_t tid;
	pthread_create(&tid, NULL, &receive_thread, &server_fd); 
	do
	{
		printf("Escreva a mensagem para enviar\n");
		scanf(" %[^\n]", &ch);
		if (strcmp(ch, "exit") == 0) {
			break;
		}
		ch[strcspn(ch, "\n")] = 0;
		sending(ch, PORT, server_fd);
	} while (strcmp(ch, "") != 0);

	close(server_fd);
	return 0;
}

//Sending messages to port
void sending(char linha[ECHOMAX], int PORT, int sock)
{
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Erro na criação do socket \n");
		return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nNão foi posssível concluir a conexão\n");
		return;
	}

	printf("%s[PORTA:%d] diz: %s", name, PORT, linha);
	send(sock, linha, ECHOMAX, 0);
	printf("\nMensagem enviada\n");
	close(sock);
}

//Calling receiving every 2 seconds
void *receive_thread(void *server_fd)
{
	int s_fd = *((int *)server_fd);
	while (1)
	{
		sleep(2);
		receiving(s_fd);
	}
}

//Receiving messages on our port
void receiving(int server_fd)
{
	struct sockaddr_in address;
	char linha[ECHOMAX] = {0};
	int addrlen = sizeof(address);
	fd_set current_sockets, ready_sockets;

	//Initialize my current set
	FD_ZERO(&current_sockets);
	FD_SET(server_fd, &current_sockets);
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

		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{

				if (i == server_fd)
				{
					int client_socket;

					if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
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
					FD_CLR(i, &current_sockets);
				}
			}
		}

		if (k == (FD_SETSIZE * 2))
				break;
	}
}