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


void meuListen(){
    printf("Iniciando escuta");
}

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

int SetupUDPServerSocket(const char *service)
{
    // Constrói a struct de endereço do servidor
    struct addrinfo addrCriteria;                   // Critério para casamento de endereço
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Limpa a struct
    addrCriteria.ai_family = AF_UNSPEC;             // Qualquer família de endereços
    addrCriteria.ai_flags = AI_PASSIVE;             // Aceita em qualquer endereço/porta
    addrCriteria.ai_socktype = SOCK_DGRAM;          // Somente sockets de fluxo
    addrCriteria.ai_protocol = IPPROTO_UDP;         

    struct addrinfo *servAddr; // Lista de endereços servidor
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
    {
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }

    //socket para comunicação do comutador com a origem
    int servSock = -1;
    for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next)
    {
        // Cria um socket UDP
        servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (servSock < 0)
            continue; // Criação do socket falhou; tenta o próximo endereço

        // Associa ao endereço local e configura o socket para escutar
        if (bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0)
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

void HandleTCPClient(int clntSocket)
{
    char buffer[BUFSIZE];
    char *line = "SYNACK";
    size_t len = 0;
    ssize_t read = 0;
    ssize_t numBytesSent, numBytesRcvd;

    while (true)
    {
        numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
        if (numBytesRcvd < 0)
        {
            perror("recv() falhou\n");
            exit(EXIT_FAILURE);
        }

        //fputs("Comutador: ", stdout);
        //fputs(buffer, stdout);
        //fputc('\n', stdout);

        //fputs("Destino: ", stdout);
        read = getline(&line, &len, stdin);
        numBytesSent = send(clntSocket, line, len, 0);
        if (numBytesSent < 0)
        {
            perror("send() falhou\n");
            exit(EXIT_FAILURE);
        }else{
            break;
        }
        
    }
}

int meuSocket(bool destino){
	int sockClient;
    int sockServer;
	if (destino == false){
		sockClient = SetupUDPClientSocket("127.0.0.1", "5001");
        return sockClient;
    }
    
    sockServer = SetupUDPServerSocket("5002");
    return sockServer;

}

void meuAccept(int clntSocket){
    printf("Esperando conexão");
    HandleTCPClient(clntSocket);
    printf("Conexão aceita");

}



int main(int argc, char const *argv[]){
	if (argc < 2 || argc > 3){
		perror("Execução correta: ./origem <ip> [<porta>]\n");
		return EXIT_FAILURE;
	}

	const char *server = argv[1];
	const char *service = (argc == 3) ? argv[2] : "5001";

    //SOCKET PARA COMUNICAÇÃO DO DESTINO COM O COMUTADOR 
    int socketDest;
    socketDest = meuSocket(true);
    meuListen();
    meuAccept(socketDest);

}
