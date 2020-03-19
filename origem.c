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
int clntSocket;

int SetupUDPClientSocket(const char *host, const char *service){
	struct addrinfo *servAddr;
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_socktype = SOCK_DGRAM;
	addrCriteria.ai_protocol = IPPROTO_UDP;

	
	int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);
	if (rtnVal != 0){
		perror("getaddrinfo() falhou\n");
		exit(EXIT_FAILURE);
	}

	int sock = -1;
	for (addr = servAddr; addr != NULL; addr = addr->ai_next)
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

int meuSocket(bool destino){
	int sockClient;
	if (destino == false){
		sockClient = SetupUDPClientSocket("host", "5001");

	}
}

void HandleTCPServer()
{
    char *line = "SYN";
    size_t len = 3;
    int numBytesSent, numBytesRcvd;

        if ((numBytesSent = sendto(clntSocket, line, sizeof(char *), 0,
        	 addr->ai_addr, addr->ai_addrlen)) == -1)
    	{
        	perror("client: sendto");
        	exit(1);
    	}

        if (numBytesSent < 0)
        {
            perror("send() falhou\n");
            exit(EXIT_FAILURE);
        }

}

void meuConnect(int clntSocket){
	printf("Pedindo conexão");
	HandleTCPServer(clntSocket);
	printf("Conexão estabelecida");

}

int main(int argc, char const *argv[]){
	if (argc < 2 || argc > 3){
		perror("Execução correta: ./origem <ip> [<porta>]\n");
		return EXIT_FAILURE;
	}

	const char *server = argv[1];
	const char *service = (argc == 3) ? argv[2] : "5001";
	
	int socketOrigem;
	socketOrigem = meuSocket(false);
	//meuConnect(socketOrigem);
	
	
}
