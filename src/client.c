#include <locale.h> // Lib para edi??o de arquivos
#include <stdio.h> // Lib para print e scan
#include <string.h> // Lib para funcionalidades com string
#include <stdlib.h> // Lib para aloca??o de mem?ria e outros macros
#include <sys/types.h> // Lib com types usados para chamadas de sistema
#include <winsock2.h> // Lib com structs necess?rias para sockets
#include <WS2tcpip.h> // Lib com struct hostent

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "Portuguese_Brazil"); //Habilita acentos nas palavras
    if(argc == 4 && strncmp(argv[1], "client", 6) == 0){
        WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { //Inicia o winsock baseado na dll e se não der certo retorna erro
                printf("Erro: %d\n", WSAGetLastError());
                return 1;
            }

            int sockfd, portno, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;
            char buffer[255];

            portno = atoi(argv[3]);
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) error("Erro ao abrir o socket");

            server = gethostbyname(argv[2]);
            if(server == NULL){
                fprintf(stderr, "Erro, host n?o encontrado");
            }

            memset((char *) &serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            memmove((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
            serv_addr.sin_port = htons(portno);
            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error ("Conex?o falhou");
            
            while (1){
                printf("********** Bem vindo ao menu de op??es do usu?rio **********\n\n");
                printf("Digite READ para ler os registros salvos.\n");
                printf("Digite DELETE (c?digo) para deletar o registro salvo.\n");
                printf("Digite SAIR para finalizar a execu??o e salvar as altera??es.\n\n");

                memset(buffer, 0, 255);
                fgets(buffer, 255, stdin);
                if(strncmp(buffer, "READ", 4) == 0){
                    n = send(sockfd, buffer, strlen(buffer), 0);
                    if (n < 0) error("Erro para escrever ");
                    while(1){
                        memset(buffer, 0, 255);
                        n = recv(sockfd, buffer, sizeof(buffer), 0);
                        if (n < 0) error("Erro para ler. ");
                        if (strcmp(buffer, "fim") == 0){ 
                            break;
                        }
                        else if(strcmp(buffer, "1") == 0){
                            printf("\nN?o foi poss?vel ler os registros salvos pois a lista est? vazia.\n");
                            break;
                        }
                        else{
                            printf("Server: %s", buffer);
                        }
                    }
                }
                else if (strncmp(buffer, "DELETE", 6) == 0){
                    n = send(sockfd, buffer, strlen(buffer), 0);
                    if (n < 0) error("Erro para escrever ");
                    memset(buffer, 0, 255);
                    recv(sockfd, buffer, sizeof(buffer),0);
                    printf(buffer);
                }
                else if (strncmp(buffer, "SAIR", 4) == 0){
                    send(sockfd, buffer, sizeof(buffer), 0);
                    break;
                }
                else{
                    printf("Digite um valor v?lido!\n\n");
                }
            }
            closesocket(sockfd);
            WSACleanup();
    }
   
    else{
        printf("Digite (programa) client (0.0.0.0) (0000) para executar a vers?o com funcionalidades de cliente.\n");
        exit(1);
    }
    return 0;
}