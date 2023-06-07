#ifndef MYLIST_H_INCLUDED
#define MYLIST_H_INCLUDED

//define uma lista como estrutura
struct lista {
   int info;
   struct lista* prox;
};

typedef struct lista Lista;

void inicializa (void);    //deve ser chamada antes de usar a estrutura
void insere (int);         //inserir um numero na estrutura fora da ordem
void insereOrdem (int);    //inserir um numero na estrutura na ordem
void imprime (void);       //imprimir o conteudo da estrutura
int busca (int);           //localiza um valor na estrutura, devolvendo 1 se encontrou ou 0 caso contrario
void retira (int);         //apaga um numero da estrutura
void esvazia();            //deve ser chamada quando nao for mais usar a estrutura

#endif // MYLIST_H_INCLUDED
