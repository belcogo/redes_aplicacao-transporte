# Projeto do GA de RedesI: Aplicação e Transporte

### Pré-requisitos
1. Possuir docker instalado na máquina

### Instruções

1. Baixe o repositório
2. Acesse a pasta `container_client-server` pelo terminal
3. Execute o comando `docker build -t [nome_do_container] .`
4. Quando o processo for finalizado, execute o comando `docker run -d -t[nome_do_container]` (execute o run 3x para gerar 3 containers)
5. Depois recupere o nome do container criado (`docker ps` lista os containers que estão rodando)
6. Com o nome do container execute o comando `docker exec -it [nome_do_container] bash` para acessar o terminal do container. Faça isso para cada container rodando
7. execute o comando `./clientserver <Server1_IP> <Server2_IP>`. Faça isso para cada container rodando
8. Envie um comando como `ls`. Ele deve ser enviado para os outros terminais que estão rodando o programa clientserver e retornar o output do comando quando executado nesses terminais.

---------------------------------------------------------------------------------------------------------------------------------------------------------

**Objetivo**

Implementar uma comunicação peer2peer entre, no mínimo 3 peers.

**Enunciado**
Projetar e implementar um sistema de terminal distribuído com no mínimo três nodos
(peers). Cada peer da rede tem funções de servidor e cliente para utilizar comandos em
seus terminais. O objetivo é criar um software que permita a execução de comandos em
cada peer da rede. Por exemplo, quando for digitado o comando “ls” em um peer, esse
comando deve ser executado em todos os peers da rede e a visualização da resposta dos
terminais deve aparecer no peer que disparou o comando. A rede P2P pode ser estática,
isto é, as máquinas que irão pertencer a rede devem ser conhecidas.

**Aluna**
- Bel Cogo

**Professor**
- Cristiano Bonato Both

**Instituição**
- Universidade do Vale do Rio dos Sinos (UNISINOS)

____

### Códigos utilizados de referência
- Código da pasta `pratica_UDP_02` (disponibilizado em aula)
- https://github.com/um4ng-tiw/Peer-to-Peer-Socket-C/blob/main/peer.c

**OBS.:** Código em Progresso
