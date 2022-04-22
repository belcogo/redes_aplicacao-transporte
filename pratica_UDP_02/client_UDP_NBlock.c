#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>

#define ECHOMAX 255

int main(int argc, char **argv) {
    int sock;
    /* Estrutura: familia + endereco IP + porta */
    struct sockaddr_in client, server;
    
    /* Criando Socket */
    if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        printf("Socket Falhou!!!\n");
    
    int sock_size = sizeof(client);
    int value = 1; // valor a ser alterado no ioctl
    int try, ok, retrys = 5;
    char linha[ECHOMAX];
    char aux_exit[ECHOMAX];

 
    /* Consistencia */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server IP> \n", argv[0]);
        exit (1);
    }
    
    /* Socket não bloqueante FIONBIO ("File I/O NonBlocking I/O")
     ioctl permite configurar um descritor aberto.
     Três parâmetros: o descritor, um identificador da característica, novo valor */
    ioctl(sock, FIONBIO, &value);
    
    /* Construindo a estrutura de endereco local ----> Se não criar o SO criará
     A funcao bzero eh usada para colocar zeros na estrutura client */
    bzero((char *)&client, sock_size);
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY); /* endereco IP local */
    client.sin_port = htons(0); /* porta local (0=auto assign) */
    bind(sock, (struct sockaddr *)&client, sock_size);
    
    /* Construindo a estrutura de endereco do destino
     A funcao bzero eh usada para colocar zeros na estrutura server */
    bzero((char *)&server, sock_size);
    server.sin_family=AF_INET;
    /* endereco IP de destino */
    server.sin_addr.s_addr=inet_addr(argv[1]); /* host local */
    server.sin_port=htons(6000); /* porta do servidor */
    
    do {
        //gets(linha);
       fgets (linha, ECHOMAX, stdin);
       linha[strcspn(linha, "\n")] = 0;
        try = retrys;
        ok = 0;
        /* Testa se recebeu por 5 tentativas */
        while(try && !ok) {
            sendto(sock, linha, ECHOMAX, 0, (struct sockaddr *)&server, sock_size);
            strcpy(aux_exit, linha);
            sleep(1);
            if(-1 != recvfrom(sock, linha, ECHOMAX, 0,(struct sockaddr *)&server, &sock_size)) ok = 1;
            try--;
        }
        if(ok) puts(linha);
        else puts("O servidor não retornou");
        
        memset(linha, '\0', ECHOMAX);
    }
    while(strcmp(aux_exit,"exit"));
    close(sock);
    return 0;
}
