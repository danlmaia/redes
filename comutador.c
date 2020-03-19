#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static const int BUFSIZE = 512;
struct addrinfo *addr;
struct sockaddr_storage their_addr;
int sockServC;
socklen_t addr_len;

//Socket para comunicação do comutador com o destino
int SetupUDPClientSocket(const char *host, const char *service){
	
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;

	struct addrinfo *servAddr;
	int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);
	if (rtnVal != 0){
		perror("getaddrinfo() falhou\n");
		exit(EXIT_FAILURE);
	}

	//socket para comunicação do comutador com o destino
	int sock = -1;
	for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock < 0)
			continue;
		if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
			break;

		close(sock);
		sock = -1;
	}
	freeaddrinfo(servAddr);
	return sock;
	
}

//socket para comunicação do comutador com a origem
int SetupUDPServerSocket(const char *service)
{
    // Constrói a struct de endereço do servidor
    struct addrinfo addrCriteria;                   // Critério para casamento de endereço
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Limpa a struct
    addrCriteria.ai_family = AF_UNSPEC;             // Qualquer família de endereços
    addrCriteria.ai_flags = AI_PASSIVE;             // Aceita em qualquer endereço/porta
    addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;         // Somente sockets de fluxo

    struct addrinfo *servAddr; // Lista de endereços servidor
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
    {
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }

    //socket para comunicação do comutador com a origem
    int servSock = -1;
    for (addr = servAddr; addr != NULL; addr = addr->ai_next)
    {
        // Cria um socket TCP
        servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_socktype);
        if (servSock < 0)
            continue; // Criação do socket falhou; tenta o próximo endereço

        // Associa ao endereço local e configura o socket para escutar
        if (bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) /*&& (listen(servSock, MAXPENDING) == 0))*/
        {
            break;
        }

        close(servSock);
        servSock = -1;
    }

    // Libera memória da lista de endereços alocada por getaddrinfo()
    freeaddrinfo(servAddr);

    return servSock;
}

void receber(){
	char *msg;
	int len = 3;
	addr_len = sizeof their_addr;

	int numbytes;
    if ((numbytes = recvfrom(sockServC, msg, sizeof(msg), 0,
            				(struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }
	printf("Antes: num1= %sd.\n", msg);
}

int main(int argc, char const *argv[]){
	if (argc < 2 || argc > 3){
		perror("Execução correta: ./origem <ip> [<porta>]\n");
		return EXIT_FAILURE;
	}

	const char *server = argv[1];
	const char *service = (argc == 3) ? argv[2] : "5001";

	//int sockClntC;
	//sockClntC = SetupUDPClientSocket(server, "5002");
	sockServC = SetupUDPServerSocket(service);
	receber();

	/*
	recvfrom(sockServC);
	sendTo(sockClntC);
	*/
}
