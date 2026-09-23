#include <locale.h> // Lib para edição de arquivos
#include <stdio.h> // Lib para print e scan
#include <string.h> // Lib para funções com string
#include <stdlib.h> // Lib para alocação de memória e outros macros
#include <sys/types.h> // Lib com types usados para chamadas de sistema
#include <winsock2.h> // Lib com structs necessárias para sockets
#include <WS2tcpip.h> // Lib com struct hostent
#include <ctype.h> // Lib com função isdigit
#include <windows.h> //Lib para sleep

#define t 70

typedef struct Cadastro{
    int codigo;
    char nome[t];
    char telefone[t];
    char email[t];
    char endereco[t];
    struct Cadastro *proximo;
} Cadastro; //Struct para armazenar um cadastro simples

int opcao = 0;
Cadastro *inicio = NULL;
Cadastro *tmp;
FILE *banco;

void error(const char *msg){
    perror(msg);
    exit(1);
}

int verificaDuplicata(int codigo){ //Função que verifica se um valor já existe na lista
    tmp = inicio;

    while(tmp != NULL){ //Enquanto o item atual não for nulo continua
        if(tmp->codigo == codigo){//Se o código do item atual for igual ao inteiro passado como parâmetro retorna 1(True)
            return 1;
        }
    tmp = tmp->proximo;//Faz com que a variável do item atual receba o ponteiro do próximo item da lista
    }
    return 0; //Se não encontrar o inteiro passado como parâmetro retorna 0(False)
}

Cadastro *novoRegistro(){//Função que faz o registro de novos itens na lista
    int codigo;
    char nome[t];
    char telefone[t];
    char email[t];
    char endereco[t];
    //Declarando variáveis locais para facilitar o armazenamento nos ponteiros

    void lerString(char *destino, const char *mensagem){
        printf("%s", mensagem);
        fgets(destino, t, stdin);
        destino[strcspn(destino, "\n")] = '\0'; //Strcspn retorna o índice equivalente ao caractere passado no segundo parâmetro e substitui por \0 que signfica final da string
    } //Declara uma função local para ler a string e salvar para evitar que o código fique repetitivo

    do {
        printf("Digite o código de cadastro da pessoa:\n");
        scanf("%d", &codigo);
        while (getchar() != '\n'); //Limpa o buffer

    if (verificaDuplicata(codigo)) {
        system("cls"); //Limpa o terminal
        printf("Esse código já foi cadastrado, digite novamente.\n");
    }
    } while (verificaDuplicata(codigo)); //Enquanto o código cadastrado já existir, executa o DO acima

    lerString(nome, "Digite seu nome:\n");
    lerString(telefone, "Digite seu telefone:\n");
    lerString(email, "Digite seu email:\n");
    lerString(endereco, "Digite seu endereço:\n");
    //Chama a função de ler string que armazena o input nas variáveis locais


    Cadastro *result = malloc(sizeof(Cadastro)); //Faz uma pré alocação de memória para um dado do tipo cadastro
    result->codigo = codigo;
    strcpy(result->nome, nome);
    strcpy(result->telefone, telefone);
    strcpy(result->email, email);
    strcpy(result->endereco, endereco);
    result->proximo = NULL; 
    
    return result;
}

void listarRegistro(){ //Função para listar os itens salvos
    int i = 0;
    tmp = inicio;

    if(tmp == NULL){ //Se a lista não tiver nenhum registro retorna erro
        printf("Por favor, acrescente um registro antes de poder listar. \n");
        return;
    }

    while(tmp != NULL){
        i++;
        printf("Pessoa número %d:\n\n", i);
        printf("Código: %d\n", tmp->codigo);
        printf("Nome: %s\n", tmp->nome);
        printf("Telefone: %s\n", tmp->telefone);
        printf("Email: %s\n", tmp->email);
        printf("Endereço: %s\n\n", tmp->endereco);
        tmp = tmp->proximo;
    }
}

void listarRead(int newsockfd){ //Função para listar os itens salvos
    int i = 0, n;
    tmp = inicio;

    if(tmp == NULL){ //Se a lista não tiver nenhum registro retorna erro
        n = send(newsockfd, "1", 50, 0);
        if (n < 0) error("Erro ao enviar");
        return;
    }

    while(tmp != NULL){
        i++;
        char output[300];
        memset(output, 0, 300);
        sprintf(output,"Pessoa número %d:\n\n"
        "Código: %d\n"
        "Nome: %s\n"
        "Telefone: %s\n"
        "Email: %s\n"
        "Endereço: %s\n\n",
        i, tmp->codigo, tmp->nome, tmp->telefone, tmp->email, tmp->endereco);
        n = send(newsockfd, output, strlen(output), 0);
        if (n < 0) error("Erro ao enviar");
        tmp = tmp->proximo;
        Sleep(1);
    }
}

const char *readHTMl(int newsockfd){ //Função para listar os itens salvos
    int i = 0, n;
    tmp = inicio;
    static char output[20000] = "", lista[20000] = "";
    char temp[300];

    if(tmp == NULL){ //Se a lista não tiver nenhum registro retorna erro
        if (n < 0) error("Erro ao enviar");
        return "A lista está vazia";
    }

    while(tmp != NULL){
        i++;
        memset(temp, 0, 300);
        sprintf(temp,"<ul><li>Pessoa número: %d "
        "<ul><li> Código: %d </li>"
        "<li> Nome: %s </li>"
        "<li> Telefone: %s </li>"
        "<li> Email: %s </li>"
        "<li> Endereço: %s </li> </ul> </ul>",
        i, tmp->codigo, tmp->nome, tmp->telefone, tmp->email, tmp->endereco);
        strcat(lista,temp);
        if (n < 0) error("Erro ao enviar");
        tmp = tmp->proximo;
    }
    strcpy(output,lista);
    memset(lista, 0, 1000);
    return output;
}

void salvarRegistros(){ //Função para salvar a lista encadeada no arquivo
    banco = fopen("file.csv", "w");
    int i = 0;
    tmp = inicio;

    if(tmp == NULL){
        printf("Execução finalizada sem nenhum dado. \n");
        return;
    }

    while(tmp != NULL){
        i++;
        fprintf(banco, "%d,%s,%s,%s,%s\n", 
                    tmp->codigo,
                    tmp->nome,
                    tmp->telefone,
                    tmp->email,
                    tmp->endereco);
        tmp = tmp->proximo;
    }
    printf("Execução finalizada, modificações salvas\n");
    fclose(banco);
}

void removeRegistro(){
    system("cls");
    int codigo;
    tmp = inicio;
    Cadastro *anterior = NULL;

    printf("Digite o código da pessoa que deseja remover:\n");
    scanf("%d", &codigo);

    while(tmp != NULL){
        if(tmp->codigo == codigo){
            if(anterior == NULL){
                inicio = tmp->proximo;
            }
            else{
                anterior->proximo = tmp->proximo;
            }
            free(tmp);
            return;
        }
        anterior = tmp;
        tmp = tmp->proximo;
    }
    printf("O código digitado não está na lista\n\n");
}

void menu(int *mopcao){
    system("cls");
    while(1){
        printf("***** Bem vindo(a) ao sistema de gerenciamento de registros de pessoas! *****\n\n");
        printf("Qual das seguintes opções você deseja?\n\n");
        printf("1. Adicionar novo registro\n");
        printf("2. Listar todos os registros\n");
        printf("3. Remover registro por chave\n");
        printf("0. Sair do programa\n\n");
        scanf("%d", mopcao);
        while (getchar() != '\n'); // Limpa o buffer de entrada
        
        int _opcao = *mopcao;

        switch(_opcao){
            case 0:
                return;
            case 1:
                system("cls");
                tmp = novoRegistro();
                tmp->proximo = inicio;
                inicio = tmp;
                break;
            case 2:
                system("cls");
                listarRegistro();
                break;
            case 3:
                system("cls");
                removeRegistro();
                break;
            default:
                printf("Opção inválida! Digite novamente.\n\n");  
        }
    }
}

void lerBanco(FILE *banco) {
    char linha[t];
    while (fgets(linha, t, banco)) {
        Cadastro *novo = malloc(sizeof(Cadastro));
        if (!novo) {
            printf("Erro ao alocar memória.\n");
            exit(1);
        }

        if (sscanf(linha, "%d,%70[^,],%70[^,],%70[^,],%70[^\n]",
                   &novo->codigo,
                   novo->nome,
                   novo->telefone,
                   novo->email,
                   novo->endereco) == 5) {

            novo->proximo = inicio;
            inicio = novo;
        } else {
            free(novo);
        }
    }
}

void clientDelete(int newsockfd, int codigo){
    tmp = inicio;
    Cadastro *anterior = NULL;

    while(tmp != NULL){
        if(tmp->codigo == codigo){
            if(anterior == NULL){
                inicio = tmp->proximo;
            }
            else{
                anterior->proximo = tmp->proximo;
            }
            free(tmp);
            int n = send(newsockfd, "Dado removido com sucesso\n\n",strlen("Dado removido com sucesso\n\n"),0);
            if (n < 0) error("Erro para enviar ");
            return;
        }
        anterior = tmp;
        tmp = tmp->proximo;
    }
    send(newsockfd, "O código digitado não está na lista\n\n",strlen("O código digitado não está na lista\n\n"),0);
}

int deleteHTML(int codigo){
    tmp = inicio;
    Cadastro *anterior = NULL;

    while(tmp != NULL){
        if(tmp->codigo == codigo){
            if(anterior == NULL){
                inicio = tmp->proximo;
            }
            else{
                anterior->proximo = tmp->proximo;
            }
            free(tmp);
            return 1;
        }
        anterior = tmp;
        tmp = tmp->proximo;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Portuguese_Brazil"); //Habilita acentos nas palavras
    banco = fopen("file.csv", "r"); //Salva um ponteiro para o arquivo na variável
    if (banco) lerBanco(banco); //Se for salvo com sucesso chama a função para ler o banco
    fclose(banco); //Fecha o banco

    if(argc == 1) menu(&opcao); //Se não foi passado nenhum argumento na execução do programa executa o menu

    else if(argc == 2 && (strncmp(argv[1], "http", 4) == 0 || strncmp(argv[1], "server", 6) == 0)){
        //Se foi passado um argumento na execução do programa e esse argumento foi html ou server executa
        char buffer[255];
        WSADATA wsa; //Inicia o WSADATA necessário para que o socket funcione
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){ //Inicia o winsock baseado na dll e se não der certo retorna erro
            printf("Erro: %d\n", WSAGetLastError());
            return 1;
        }
        struct sockaddr_in serv_addr, cli_addr; //Inicia estruturas para o socket
        int sockfd, newsockfd, portno, n, clilen; //Inicia variáveis para o socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0); //Inicia o socket e declara um file descriptor
        if(sockfd < 0) error("Error ao abrir o socket. ");//Se o socket não pode ser iniciado retorna erro
        
        memset((char *) &serv_addr, 0, sizeof(serv_addr)); //Limpa lixo de memória da variável serv_addr
        serv_addr.sin_family = AF_INET; // Declara a conexão como TCP
        serv_addr.sin_addr.s_addr = INADDR_ANY; // Declara o ip como qualquer IP local disponível na máquina
        
        if(strncmp(argv[1], "http", 4) == 0){
            portno = 8080; //Declara a porta que ficará aberta
            serv_addr.sin_port = htons(portno); //Vincula a porta à conexão

            if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("Erro no bind"); //Faz o bind, o segundo parametro tem um cast para sockaddr

            listen(sockfd, 1); //Faz o listen com até 1 conexões simultâneas
            clilen = sizeof(cli_addr); //Define a váriável com o tamanho em bytes da struct cliaddr
            
            while(1){//Loop para manter a execução do programa
                char response[20000];
                memset(response, 0, sizeof(response)); //Limpa o lixo de memória da variável response
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //Aceita conexões
                if(newsockfd < 0) error("Erro ao aceitar");//Se a conexão não foi aceita retorna erro
                n = recv(newsockfd, response, sizeof(response), 0);//Recebe o que foi mandado pelo navegador
                if (n < 0) error("Erro para enviar ");

                if(strncmp(response, "GET / HTTP/1.1", 14) == 0){// Se o que foi mandado pelo navegador começar com
                    char html_body[20000];
                    sprintf(html_body, "<html><body><h1> %s </h1></body></html>", readHTMl(newsockfd));//Faz o body do HTML com base no retorno da função readHTML
                    memset(response, 0, sizeof(response)); 
                    snprintf(response, sizeof(response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: %ld\r\n"
                    "\r\n"
                    "%s",
                    strlen(html_body), html_body); //Cria a string que será enviada para o navegador
                    n = send(newsockfd, response, sizeof(response), 0);
                    if (n < 0) error("Erro para enviar "); //Envia a string
                    Sleep(2000); //Aguarda 2 segundos para enviar
                }

                else if(strncmp(response, "GET /delete", 11) == 0){//Se o que foi mandado pelo navegador começar com
                    int achouDigito = 0;
                    char *str = response;
                    while(*str){//Loop para percorrer a string
                        if (isdigit(*str)){//Se o caractere atual da string for dígito
                            achouDigito = deleteHTML(atoi(str)); //Declara a variável achou digito com o retorno da função deleteHTML
                            salvarRegistros(); //Salva as edições no arquivo
                            break; //Quebra o loop
                        }
                        str++; //Se o caractere atual não for dígito continua percorrendo a string
                    }

                    if(achouDigito == 0){//Se um dígito não foi encontrado
                        tmp = inicio; //A variável recebe o primeiro valor da lista encadeada
                        if(tmp != NULL){ //Se o primeiro valor da lista encadeada não for nulo executa
                            sprintf(response,
                            "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "<html><body><h1>ERRO 404</h1><h1>Código não encontrado, digite delete/cod com um código válido da lista</h1></body></html>"
                        );//Salva a resposta que será enviada de código não encontrado para o navegador
                            n = send(newsockfd, response, strlen(response), 0); //Envia a resposta
                            if (n < 0) error("Erro para enviar ");//Se não for enviada retorna erro
                            Sleep(2000); //Aguarda 2 segundos para enviar
                        }
                        else{//Se o primeiro valor da lista encadeada for nulo executa
                            sprintf(response,
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "\r\n"
                            "<html><body><h1>A lista está vazia</h1></body></html>"
                        );//Cria uma string da resposta que será enviada ao navegador
                            n = send(newsockfd, response, sizeof(response), 0);//Envia a resposta
                            if (n < 0) error("Erro para enviar ");//Se não for enviada retorna erro
                            Sleep(2000); //Aguarda 2 segundos para enviar
                        }
                    }
                    else if(achouDigito == 1){
                        char html_body[1000];
                        sprintf(html_body, "<html><body><h1> %s </h1></body></html>", readHTMl(newsockfd));
                        memset(response,0,sizeof(response));
                        memset(response, 0, 1000);
                        snprintf(response, sizeof(response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %ld\r\n"
                        "\r\n"
                        "%s",
                        strlen(html_body), html_body);
                        send(newsockfd, response, sizeof(response), 0);
                        if (n < 0) error("Erro para enviar ");
                        Sleep(2000); //Aguarda dois segundos para enviar
                    }
                }
                else{
                        sprintf(response,
                        "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/html\r\n"
                        "\r\n"
                        "<html><body><h1> ERRO 404 </h1> <h1> Request não encontrado favor utilizar "
                        "http://Ip para a página inicial ou http://Ip/delete/cod para deletar um registro.</h1></body></html>"
                    );
                        n = send(newsockfd, response, sizeof(response), 0);
                        if (n < 0) error("Erro para enviar ");
                        Sleep(2000);
                }
                closesocket(newsockfd);
            }
        }
        
        else if(strncmp(argv[1], "server", 6) == 0){
            portno = 9000;
            serv_addr.sin_port = htons(portno); //Declara a porta da conexão

            if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("Binding Failed."); //Faz o bind, o segundo parametro tem um cast para sockaddr

            listen(sockfd, 1); //Faz o listen com até 1 conexões simultâneas
            clilen = sizeof(cli_addr);

            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if(newsockfd < 0) error("Error on Accept");


            while(1){
                memset(buffer, 0, 255);
                n = recv(newsockfd, buffer, 255, 0);
                if (n < 0) error("Error on reading. ");

                if (strncmp("READ", buffer, 4) == 0){
                    listarRead(newsockfd);
                    n = send(newsockfd, "fim", 3, 0);
                    if (n < 0) error("Erro ao enviar");
                }
                else if (strncmp("DELETE", buffer, 6) == 0){
                    int achouDigito = 0;
                    char *str = buffer;
                    while(*str){
                        if (isdigit(*str)){
                            clientDelete(newsockfd, atoi(str));
                            achouDigito = 1;
                            break;
                        }
                        str++;
                    }
                    if(achouDigito == 0){
                        n = send(newsockfd, "Código não encontrado, digite <DELETE (cod)>\n",strlen("Código não encontrado, digite <DELETE (cod)>\n"), 0);
                        if (n < 0) error("Erro para enviar ");
                    }
                }
                else if (strncmp("SAIR", buffer, 4) == 0){
                    closesocket(newsockfd);
                    salvarRegistros();
                    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                    if(newsockfd < 0) error("Error on Accept");
                }
                else if (strncmp("FINALIZAR", buffer, 9) == 0){
                    break;
                }
            }
        }
        closesocket(newsockfd);
        closesocket(sockfd);
        WSACleanup();
    }
    
    else{
        printf("\n\nUtilize um dos modelos abaixo: \n");
        printf("Digite (programa) para executar a versão com funcionalidades gerais.\n");
        printf("Digite (programa) html para executar a versão HTML.\n");
        printf("Digite (programa) server para executar a versão com funcionalidades de servidor.\n");
        exit(1);
    }
        
    salvarRegistros();

    return 0;
}
