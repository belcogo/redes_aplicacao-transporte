#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#define ECHOMAX 255

int main(void) {
    int sock;
    /* Estrutura: familia + endereco IP + porta */
    struct sockaddr_in server, client;
    int sock_size = sizeof(server);
    
    /* Cria o socket para enviar e receber datagramas
     parametros(familia, tipo, protocolo) */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("ERRO na Criacao do Socket!\n");
    else
        printf("Esperando Mensagens...\n");
    
    char linha[ECHOMAX];
    char aux[ECHOMAX] = " - reenviado!!";
    char aux_exit[ECHOMAX];
    
    /* Construcao da estrutura do endereco local
     Preenchendo a estrutura socket server (familia, IP, porta)
     A funcao bzero eh usada para colocar zeros na estrutura server */
    bzero((char *)&server, sock_size);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* endereco IP local */
    server.sin_port = htons(6000); /* porta local  */
    
    /* Bind para o endereco local
     parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
    if(-1 != bind(sock, (struct sockaddr *)&server, sock_size))
        do{
            /* Recebe mensagem do endereco remoto
             parametros(descritor socket, dados, tamanho dos dados, flag, estrutura do socket, endereco do tamanho da estrutura) */
            recvfrom(sock, linha, ECHOMAX, 0, (struct sockaddr *)&client, &sock_size);
            printf("Porta:%i Endereço:%s\n",ntohs(client.sin_port),inet_ntoa(client.sin_addr));
            strcpy(aux_exit, linha);
            printf("%s\n", linha);
            strcat(linha, aux);
            /* Envia mensagem para o endereco remoto
             parametros(descritor socket, dados, tamanho dos dados, flag, estrutura do socket, tamanho da estrutura) */
            sendto(sock, linha, ECHOMAX, 0, (struct sockaddr *)&client, sock_size);
        }while(strcmp(aux_exit, "exit"));
    else puts("Porta ocupada");
    close(sock);
    return 0;
}
