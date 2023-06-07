/*
  ALUNOS
- Emerson B. Filho
- Gustavo K. Volobueff
- Gustavo M. Goncalves
- Victor K. O. Takatu
- Vinicius A. Schautz

  TERMINAL - OPERACAO EM LINUX (Pasta com 'main.c | myList.a | myList.h')

- Compilacao: gcc main.c -lm myList.a -o main
- Execucao: ./main
*/
#include "myList.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*DADOS-BASE*/
#define MIN_N 10
#define MAX_N 1000000
/*SAIDAS DE ERRO - INSUCESSO*/
#define SAIDA_FORA_LIMITE "\n[ERRO] O Valor informado deve ser entre %d e %d!\n"
#define SAIDA_BUSCA_0 "\n[ERRO] O numero %d nao existe na lista!\n"
#define S_OU_N "\n[ERRO] Informe uma resposta valida!\n"
/*SAIDAS DE RESULTADOS*/
#define SAIDA_INEXISTE "\nNao existem numeros narcisistas entre %d e %d [lista vazia]!\n"

/*prototipos de funcao*/
void clear(void);
void pause(void);
void handler_excluir(int,int);
void excluiNum(int);
int leitura_inicial(void);
int numNarcisista(int);
int escolhaValida(char);

int main()
{
    int n, i, existe = 0;

    n = leitura_inicial();

    inicializa();
    
    /*procurar por numeros narcisitas entre 10 e n*/
    for(i=MIN_N;i<=n;i++){
        if(numNarcisista(i)){
            existe++;
            insereOrdem(i);
        }
    }
    /*se existir numeros narcisistas entre 10 e n*/
    if(existe){
        handler_excluir(n, existe);
        esvazia();
    } else printf(SAIDA_INEXISTE, MIN_N, n);

    printf("\n\n-----Fim da execucao-----\n"); 
}

int leitura_inicial() /*ler a entrada*/
{
    int num;

    do{
        clear();

        printf("Informe um numero entre %d e %d:\n--> ", MIN_N, MAX_N);
        scanf("%d", &num);

        if((num < MIN_N) || (num > MAX_N)){
            printf(SAIDA_FORA_LIMITE, MIN_N, MAX_N);
            pause();
        }
    }while((num < MIN_N) || (num > MAX_N));

    return num;
}

int numNarcisista(int num) /*verificar se dado numero e narcisista*/
{
    int exp = log10(num)+1, sum = 0, num_aux = num;

    while(num_aux > 0)
    {  
        sum += pow(num_aux % 10, exp);
        num_aux /= 10;
    }

    if(sum == num) return num;
    return 0;
}

void handler_excluir(int n, int existe) /*operacao geral de exclusao de numeros*/
{
    int n_delete;
    char escolha;

    do{
        do{
            clear();
            imprime();

            printf("\nDeseja excluir algum numero da lista? (S/n)\n--> ");
            scanf(" %c", &escolha);
        }while(!escolhaValida(escolha));

        if(escolha == 'S' || escolha == 's'){
            do{
                clear();
                imprime();

                printf("\nInforme o numero que deseja excluir:\n--> ");
                scanf("%d", &n_delete);

                if(!busca(n_delete)){
                    printf(SAIDA_BUSCA_0, n_delete);
                    pause();
                }
            }while(!busca(n_delete));

            excluiNum(n_delete);
            existe--;
        } else break;
    }while(existe);

    if(!existe){
        clear();
        printf(SAIDA_INEXISTE, MIN_N, n);
    }
}

int escolhaValida(char escolha) /*retorna 0 se a escolha for invalida ou 1 se for valida*/
{
    if(escolha != 'S' && escolha != 's' && escolha != 'N' && escolha != 'n'){
        printf(S_OU_N);
        pause();

        return 0;
    }
    return 1;
}

void excluiNum(int num) /*excluir um numero e exibir a nova lista*/
{
    retira(num);

    clear();
    imprime();
}
/*como em system("pause")*/
void pause(){system("read -p '\n\nAperte qualquer tecla para continuar...\n' var");}
/*como em system("clear")*/
void clear(){puts("\x1b[H\x1b[2J");}