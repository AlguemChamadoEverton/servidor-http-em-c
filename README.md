# Cadastro em C com servidor TCP e HTTP

Sistema de cadastro de pessoas escrito em **C puro**, sem bibliotecas externas além da API de sockets do Windows. Os registros ficam numa **lista encadeada** em memória e são salvos em um arquivo CSV.

O mesmo executável funciona de três formas: menu no terminal, servidor TCP para um cliente próprio, e servidor HTTP que responde direto no navegador.

Projeto de estudo, feito para entender na prática como funcionam sockets, o protocolo HTTP e alocação dinâmica de memória, sem o apoio de nenhum framework.

## Funcionalidades

- Cadastrar, listar e remover registros (código, nome, telefone, e-mail e endereço)
- Validação de código duplicado
- Persistência em arquivo CSV, carregado na inicialização e salvo ao sair
- Servidor TCP com protocolo de texto próprio
- Exclusão de registro pelo navegador, por uma rota HTTP

## Requisitos

- Windows (o código usa Winsock: `winsock2.h` e `windows.h`)
- GCC (MinGW) ou outro compilador C

## Como compilar

```bash
gcc src/servidor.c -o servidor.exe -lws2_32
gcc src/client.c   -o client.exe   -lws2_32
```

A flag `-lws2_32` linka a biblioteca de sockets do Windows.

## Como usar

O arquivo `file.csv` precisa estar na mesma pasta do executável. Ele já vem com dados de exemplo (fictícios).

### 1. Menu no terminal

```bash
servidor.exe
```

Abre o menu interativo:

```
1. Adicionar novo registro
2. Listar todos os registros
3. Remover registro por chave
0. Sair do programa
```

As alterações são gravadas no CSV ao sair.

### 2. Servidor TCP com o cliente próprio

Em um terminal:

```bash
servidor.exe server
```

O servidor fica escutando na porta **9000**.

Em outro terminal:

```bash
client.exe client localhost 9000
```

Comandos aceitos pelo cliente:

| Comando | O que faz |
|---|---|
| `READ` | Lista todos os registros |
| `DELETE <código>` | Remove o registro com aquele código |
| `SAIR` | Encerra a conexão e salva as alterações |

### 3. Servidor HTTP

```bash
servidor.exe http
```

O servidor fica escutando na porta **8080**. Depois abra o navegador em `http://localhost:8080`. A página lista os registros. A rota de exclusão remove um registro pelo código e grava a alteração no CSV.

## Estrutura

```
.
├── src/
│   ├── servidor.c   # menu, lista encadeada, CSV, servidor TCP e HTTP
│   └── client.c     # cliente TCP de linha de comando
├── file.csv         # base de dados (dados de exemplo)
└── README.md
```

Os registros são guardados nesta estrutura:

```c
typedef struct Cadastro {
    int codigo;
    char nome[70];
    char telefone[70];
    char email[70];
    char endereco[70];
    struct Cadastro *proximo;
} Cadastro;
```

## Limitações conhecidas

Este é um projeto de aprendizado, e estas limitações são conhecidas:

- Roda apenas no Windows, porque usa Winsock, `system("cls")` e `Sleep`
- O servidor atende **um cliente por vez** (`listen` com fila de 1, sem threads)
- Não há autenticação nem validação de entrada no modo HTTP
- Os campos têm tamanho fixo de 70 caracteres
- O CSV é regravado por inteiro a cada salvamento
## Sobre

Escrito por [Everton Rodrigues](https://github.com/AlguemChamadoEverton) como projeto de estudo de redes e estruturas de dados em C.
