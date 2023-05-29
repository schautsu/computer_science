/*
    Linguagens Formais e Autômatos (LFA)
    Aluno: Vinícius Alves Schautz (viniciusaschautz@gmail.com)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Protótipos de funções */
int escolhaPrincipal(void);

void inserirAFD(char *, char *, char*, char *, char [][10]);
void leiaDelta(char [][10], const char *, const char *);
void split(const char *, char *, const char);

void verificarPalavra(const char *, const char *, const char, const char *, char [][10]);
void leiaPalavra(char *, const char *);

bool isEstadoValido(const char, const char *);
bool isElementosValidos(const char *, const char *);

void pause(void);

int main()
{
    /* A: alfabeto; Q: estados; q0: estado inicial; F: estados de aceitação; Delta: transição de estados; W: palavra fornecida */
    char A[10], Q[10], q0, F[10], Delta[10][10];
    bool AFDisCreated = false;
    int option;
    do {
        option = escolhaPrincipal();

        switch (option) {
            case 1:
                inserirAFD(A, Q, &q0, F, Delta);
                AFDisCreated = true;
            break;
            case 2:
                if (AFDisCreated)
                    verificarPalavra(A, Q, q0, F, Delta);
                else {
                    printf("\n[ERRO] AFD ainda nao criado!\n");
                    pause();
                }
            break;
        }
    } while (option != 3);

    system("clear");

    return 0;
}

int escolhaPrincipal()
{
    int option;
    do {
        system("clear");
        printf("AUTOMATO FINITO DETERMINISTICO (AFD)\n\n[1] Inserir novo automato\n[2] Verificar palavra\n[3] Encerrar\n\n--> ");
    }
    while ((!scanf("%d", &option) && getchar()) || (option < 1 || option > 3));

    return option;
}

/* Leitura dos elementos do AFD a ser criado */
void inserirAFD(char *A, char *Q, char *q0, char *F, char Delta[][10])
{
    system("clear");
    getchar();

    char str[30];
    bool valid;

    puts("[ATENCAO] separados por virgula, sem espaços!\n");

    /* LEITURA: alfabeto A */
    printf("Entre com o ALFABETO: ");
    fgets(str, 30, stdin);
    split(str, A, ','); /* Retira vírgulas */

    /* LEITURA: estados Q */
    printf("Entre com os ESTADOS: ");
    fgets(str, 30, stdin);
    split(str, Q, ','); /* Retira vírgulas */

    /*LEITURA: estado inicial q0 */
    do {
        printf("Entre com o ESTADO INICIAL: ");
        scanf(" %c", q0);
        getchar();

        if (!(valid = isEstadoValido(*q0, Q)))
            printf("[ERRO] Estado invalido!\n");
    } while (!valid);

    /* LEITURA: estados de aceitação F */
    do {
        printf("Entre com os ESTADOS DE ACEITACAO: ");
        fgets(str, 30, stdin);
        split(str, F, ','); /* Retira vírgulas */

        if (!(valid = isElementosValidos(F, Q)))
            printf("[ERRO] Existem estados invalidos!\n");
    } while (!valid);

    /* LEITURA: estados de transição Delta */
    leiaDelta(Delta, A, Q);

    printf("\n[AVISO] Automato criado com sucesso!\n");
    pause();
}
/* Leitura de Delta (transição entre estados com dadas entradas) */
void leiaDelta(char Delta[][10], const char *A, const char *Q)
{
    unsigned indexQ, indexA;
    bool valid;
    /* Leitura dos estados de transição */
    for (indexQ = 0; indexQ < strlen(Q); ++indexQ) {

        for (indexA = 0; indexA < strlen(A); ++indexA) {
            do {
                printf("Delta(%c,%c): ", Q[indexQ], A[indexA]);
                scanf(" %c", &Delta[indexQ][indexA]);

                if (!(valid = isEstadoValido(Delta[indexQ][indexA], Q)))
                    printf("[ERRO] Estado invalido!\n");
            } while (!valid);
        }
    }
}
/* Retira vírgulas de um array */
void split(const char *src, char *dest, const char avoid)
{
    unsigned i = 0, j = 0;

    while (i < strlen(src)-1) {
        if (src[i] != avoid)
            dest[j++] = src[i];
            
        ++i;
    }
    dest[j] = '\0';
}

/* Leitura e verificação de uma palavra fornecida */
void verificarPalavra(const char *A, const char *Q, const char q0, const char *F, char Delta[][10])
{
    getchar();

    unsigned indexW, indexA, indexQ, indexF;
    char Qcurrent, W[20];  
    bool accepted;  

    leiaPalavra(W, A); /* Leitura da palavra a ser verificada */

    printf("\nTransicao de estados\n");
    Qcurrent = q0; /* O estado atual inicia-se com q0 (estado inicial) */

    /* Realiza verificações com cada caracter da palavra W fornecida */
    for (indexW = 0; indexW < strlen(W); ++indexW) {
        printf("%c (%c)", Qcurrent, W[indexW]); /* Print: estado atual */

        /* Verifica em qual índice Q está o estado atual */
        for (indexQ = 0; indexQ < strlen(Q); ++indexQ) {

            if (Q[indexQ] == Qcurrent) {
                /* Verifica em qual índice A está o caracter da palavra */
                for (indexA = 0; indexA < strlen(A); ++indexA) {
                    
                    if (A[indexA] == W[indexW]) {
                        /* O estado atual passa a ser a intersecção (Q,A) */
                        Qcurrent = Delta[indexQ][indexA];

                        goto nextW; /* Sai dos loops */
                    }
                }
            }
        }
        nextW: printf("-> "); /* Print: caminho do estado atual */
    }
    printf("%c\n\n", Qcurrent); /* Print: estado atual (final) */

    accepted = false;
    /* Verifica se o estado final é um estado de aceitação */
    for (indexF = 0; indexF < strlen(F); ++indexF) {
        if (Qcurrent == F[indexF]) {
            accepted = true;
            break;
        }
    }
    /* Saída */
    accepted ? printf("[AVISO] Palavra aceita!\n") : printf("[AVISO] Palavra rejeitada!\n");

    pause();
}
/* Leitura de uma palavra W */
void leiaPalavra(char *W, const char *A)
{
    bool valid;
    /* Leitura da palavra */
    do {
        system("clear");

        printf("Entre com a palavra a ser verificada:\n\n--> ");
        fgets(W, 20, stdin);

        W[strlen(W)-1] = '\0'; /* Retira '\n' */

        if (!(valid = isElementosValidos(W, A))) {
            printf("\n[ERRO] A palavra contem simbolo invalido!\n");
            pause();
        }
    } while (!valid);
    /* Repete quando a palavra possui algum símbolo inexistente no alfabeto */
}

/* Verifica se dado elemento está contido em um conjunto X */
bool isEstadoValido(const char elem, const char *setX)
{
    unsigned indexX;

    for (indexX = 0; indexX < strlen(setX); ++indexX) {
        if (elem == setX[indexX]) {
            return true;
        }
    }
    return false;
}
/* Verifica se cada elemento de dado conjunto X está contido em outro conjunto Y */
bool isElementosValidos(const char *setX, const char *setY)
{
    unsigned indexX, indexY;
    bool valid;

    for (indexX = 0; indexX < strlen(setX); ++indexX) {

        valid = false;
        for (indexY = 0; indexY < strlen(setY); ++indexY) {

            if (setX[indexX] == setY[indexY]) {
                valid = true;
                break;
            } 
        }
        if (!valid) return false;
    }
    return true;
}

/* Pausa a exibição */
void pause()
{
    system("read -p '\nAperte qualquer tecla para retornar...\n' exiting");
}
