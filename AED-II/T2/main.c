/*
Author: Vinícius Alves Schautz (viniciusaschautz@gmail.com)
Year: 2023
*/
#include <stdio.h> /* I/O */
#include <stdlib.h> /* system */
#include <stdbool.h> /* bool */
#include <limits.h> /* UINT_MAX */
#include "gfx/gfx.h"

/* Valores de referência da gfx. Ponto flutuante para não ocorrerem problemas de precisão com operações posteriores de divisão */
#define APP_WIDTH 1024.0 /* Largura da tela gfx */
#define APP_HEIGHT 768.0 /* Altura da tela gfx */
#define MEMBOX_UPPER APP_HEIGHT/3 - 8 /* Medida da parte superior da barra de memória */
#define MEMBOX_LOWER APP_HEIGHT/3 * 2 + 8 /* Medida da parte inferior da barra de memória */

typedef struct Node_USED /* Lista duplamente encadeada de nós alocados */
{
    struct Node_USED *prev;

    unsigned id;
	unsigned addrInit;
    unsigned addrFinal;

	struct Node_USED *next;	
} Node_U;

typedef struct Node_FREE /* Lista simplesmente encadeada de nós disponíveis */
{
	unsigned addrInit;
    unsigned sizeB;

	struct Node_FREE *next;	
} Node_F;

/* Protótipos de funções */
unsigned mainOption();
void handlerInsertUsedNode(Node_U **, Node_F **);
void handlerRemoveUsedNode(Node_U **, Node_F **);

unsigned setFreeInitialSpaceM(Node_F **);
unsigned maxInsertFreeNodeSpace(Node_F **);
unsigned totalUsedSpace(Node_U *);

void insertUsedNode(Node_U **, const unsigned, const unsigned);
bool removeUsedNode(Node_U **, const unsigned, Node_F **);

void insertFreeNodeFromUsed(Node_F **, Node_U *);
unsigned removeFreeNode(Node_F **, const unsigned);

void mergeFreeNodes(Node_F **);
void sortFreeList(Node_F **, const char);
void doSwap(Node_F *, Node_F *);

void deallocAllUsed(Node_U **);
void deallocAllFree(Node_F **);
void pause(void);

void handlerConsoleMemory(Node_U *, Node_F *, const unsigned);
void consoleFreeNode(const float, const float, const Node_F *);
void consoleUsedNode(const float, const float, const Node_U *);
void consoleUsedPercentage(Node_U *, const unsigned);
void consoleLabel(void);

int main()
{
	Node_U *rootNode_USED = NULL;
    Node_F *rootNode_FREE = NULL;
    unsigned option, totalSpace;

    /* totalSpace contém o valor total M da memória */
    totalSpace = setFreeInitialSpaceM(&rootNode_FREE);

    /* Geração da janela gráfica */
    gfx_init(APP_WIDTH, APP_HEIGHT, "Alocação encadeada");
    handlerConsoleMemory(rootNode_USED, rootNode_FREE, totalSpace);

    /* Menu */
    do {
        option = mainOption();

        switch (option) {
            case 1: /* Alocação de blocos */
                handlerInsertUsedNode(&rootNode_USED, &rootNode_FREE);
                handlerConsoleMemory(rootNode_USED, rootNode_FREE, totalSpace);
            break;
            case 2: /* Desalocação de blocos */
                handlerRemoveUsedNode(&rootNode_USED, &rootNode_FREE);
                handlerConsoleMemory(rootNode_USED, rootNode_FREE, totalSpace);
            break;
        }
    } while (option != 3); /* Encerrar o programa */

    gfx_quit();
    /* Libera todos os nós */
    deallocAllUsed(&rootNode_USED);
    deallocAllFree(&rootNode_FREE);

    system("clear");

    return 0;
}

/* Menu de opções */
unsigned mainOption()
{
    unsigned option;
    do {
        system("clear");
        printf("Digite uma opcao:\n\n[1] Inserir um novo bloco\n[2] Remover um bloco existente\n[3] Encerrar\n\n--> ");
    }
    while ((!scanf("%u", &option) && getchar()) || (option < 1 || option > 3));

    return option;
}
/* Gerenciador de inserção de nó nos alocados */
void handlerInsertUsedNode(Node_U **root_u, Node_F **root_f)
{
    if (*root_f != NULL)
    {
        unsigned limitFree = maxInsertFreeNodeSpace(root_f);
        unsigned initialAddress; /* Endereço inicial do novo nó a ser inserido */
        unsigned sizeT; /* Tamanho pedido */
        do {
            system("clear");
            printf("Digite o tamanho (bytes) do novo bloco [1 - %u]:\n\n--> ", limitFree);
        }
        while ((!scanf("%u", &sizeT) && getchar()) || (sizeT < 1 || sizeT > limitFree));

        initialAddress = removeFreeNode(root_f, sizeT);
        insertUsedNode(root_u, sizeT, initialAddress); /* Insere o nó */

        printf("\n[AVISO] Bloco alocado com sucesso!\n");
    }
    else printf("\n[ERRO] Nao existe espaco disponivel!\n");

    pause();
}
/* Gerenciador de remoção de nó dos alocados */
void handlerRemoveUsedNode(Node_U **root_u, Node_F **root_f)
{
    if (*root_u != NULL) {
        unsigned id; 
        do {
            system("clear");
            printf("Digite a ID do bloco a ser removido:\n\n--> ");
        }
        while ((!scanf("%u", &id) && getchar()) || (id < 1));

        /* Remoção mal ou bem-sucedida */
        if (removeUsedNode(root_u, id, root_f))
            printf("\n[AVISO] Bloco removido com sucesso!\n");
        else
            printf("\n[ERRO] ID nao encontrada!\n");
    }
    else printf("\n[ERRO] Nao existem blocos alocados!\n");

    pause();
}
/* Define o valor M total de memória */
unsigned setFreeInitialSpaceM(Node_F **rootNode)
{
    Node_F *initBlock = (Node_F*) malloc(sizeof(Node_F));
    unsigned sizeM; 

    do {
        system("clear");
        printf("Digite o tamanho total (bytes) do bloco de memoria:\n\n--> ");
    }
    while ((!scanf("%u", &sizeM) && getchar()) || (sizeM < 1));

    initBlock->addrInit = 0;
    initBlock->sizeB = sizeM;
    initBlock->next = NULL;

    *rootNode = initBlock;

    return sizeM;
}
/* Retorna o valor máximo que pode ser pedido na alocação (nó disponível com maior tamanho b) */
unsigned maxInsertFreeNodeSpace(Node_F **rootNode)
{
    Node_F *auxNode = *rootNode;

    while (auxNode->next != NULL)
        auxNode = auxNode->next;
    
    return auxNode->sizeB;
}
/* Retorna o espaço usado total (bytes, no caso) */
unsigned totalUsedSpace(Node_U *node)
{
    unsigned usedSpace = 0;

    while (node) {
        usedSpace += node->addrFinal - node->addrInit + 1;
        node = node->next; 
    }
    return usedSpace;
}

/* Insere um nó na lista dos ocupados */
void insertUsedNode(Node_U **rootNode, const unsigned sizeT, const unsigned initialAddress)
{
    static unsigned identifier = 0; /* ID que será incrementada a cada nova alocação */
	Node_U *auxNode = *rootNode; /* Nó auxiliar para percorrer a lista de blocos alocados */
	Node_U *newNode = (Node_U*) malloc(sizeof(Node_U)); /* Novo nó que será alocado */

    newNode->id = ++identifier;
	newNode->next = NULL; /* Como será o último nó da lista, o próximo é null */

	if (*rootNode == NULL) { 
        /* Caso a lista de blocos alocados seja vazia */
        newNode->prev = NULL;
        newNode->addrInit = 0;
        newNode->addrFinal = sizeT - 1;

		*rootNode = newNode; /* O nó raiz aponta para o primeiro novo nó */
		return;
	} 
    /* Inserção do novo nó no final da lista */
	while (auxNode->next != NULL) {
		auxNode = auxNode->next;
	}
	auxNode->next = newNode;
    /* Atribuições */
    newNode->prev = auxNode;
    newNode->addrInit = initialAddress;
    newNode->addrFinal = newNode->addrInit + sizeT - 1;
}
/* Remove um nó da lista de ocupados */
bool removeUsedNode(Node_U **root_u, const unsigned identifier, Node_F **root_f)
{
    Node_U *auxNode = *root_u;

    /* Procura o nó com a id informada */
    while (auxNode->id != identifier) {

        if (auxNode->next == NULL) /* Foi até o fim da lista e não encontrou a id */
            return false;
            
        auxNode = auxNode->next;
    }
    
    if (auxNode == *root_u) /* Se o auxiliar aponta para o mesmo lugar onde o raiz aponta, é o primeiro bloco da lista */
        *root_u = auxNode->next; /* Caso não haja mais blocos alocados adiante, a lista fica vazia */

    if (auxNode->next != NULL) /* Se houver um bloco posterior na lista */
        auxNode->next->prev = auxNode->prev; /* o anterior do próximo bloco passa a ser o anterior do bloco a ser removido;
                                                se for null, passa a ser o primeiro bloco da lista */

    if (auxNode->prev != NULL) /* Se houver um bloco anerior na lista */
        auxNode->prev->next = auxNode->next; /* o posterior do bloco anterior passa a ser o posterior do bloco a ser removido;
                                                se for null, passa a ser o último bloco da lista */

    insertFreeNodeFromUsed(root_f, auxNode); /* Adiciona o nó na lista de desalocados */
    free(auxNode);

    return true;
}
/* Com base no nó ocupado que será excluído, insere-o na lista de nós disponíveis */
void insertFreeNodeFromUsed(Node_F **rootNode, Node_U *refNode)
{
    Node_F *auxNode = *rootNode;
    Node_F *newNode = (Node_F*) malloc(sizeof(Node_F));

    /* refNode tem as informações do nó ocupado */
    /* Atribuições e inserção do novo nó no início da lista */
    newNode->addrInit = refNode->addrInit;
    newNode->sizeB = refNode->addrFinal - refNode->addrInit + 1;
    newNode->next = auxNode;

    *rootNode = newNode;
    
    sortFreeList(rootNode, 'a'); /* Deixa a lista em ordem crescente de addrInit */
    mergeFreeNodes(rootNode); /* Une os blocos adjacentes */ 
    sortFreeList(rootNode, 's'); /* Deixa a lista em ordem crescente de sizeB */
}
/* Remove parte ou um nó inteiro do bloco dos disponíveis */
unsigned removeFreeNode(Node_F **rootNode, const unsigned sizeT)
{
    Node_F *current = *rootNode;
    Node_F *previous;

    unsigned initialAddress; /* Armazena o endereço inicial que será atribuído ao novo bloco alocado */

    while (current->sizeB < sizeT) {  /* Percorre a lista até encontrar o primeiro bloco com tamanho suficiente */                                             
        previous = current;             /* Pela condicinal, significa que não é o primeiro nó, então existe um anterior */
        current = current->next;        /* Assim, o atual sempre fica após o anterior */
    }

    if (current->sizeB == sizeT) { /* Caso seja verdadeiro, o nó atual deverá ser excluído */

        initialAddress = current->addrInit; /* (i) o endereço inicial do novo alocado é o mesmo do que foi encontrado */

        if (current == *rootNode) /* Se for o primeiro nó, a raiz aponta para o próximo */
            *rootNode = current->next;
        else 
            previous->next = current->next; /* Senão, o próximo do anterior passa a ser o nó seguinte do atual */
        
        free(current);
    }
    else {
        initialAddress = current->addrInit; /* (i) */
        /* Atribuições */
        current->addrInit += sizeT;
        current->sizeB -= sizeT;
    }
    return initialAddress;
}

/* Junta os nós de endereços contíguos */
void mergeFreeNodes(Node_F **rootNode)
{
    Node_F *auxNode = *rootNode;
    Node_F *nextNode;

    while (auxNode->next != NULL) { /* (*) auxNode só avança para o nó seguinte quando não há nós adjacentes a ele para serem juntados */

        nextNode = auxNode->next; /* (**) nó que percorre a lista e compara-se com o auxNode para saber se é adjacente em endereço */

        if (auxNode->addrInit + auxNode->sizeB == nextNode->addrInit) { /* (**) Remoção do nó seguinte que terá seu valor juntado */
            auxNode->sizeB += nextNode->sizeB; /* sizeB dos dois nós são somados */
            /* Exclusão na lista: nó com valor juntado e não deve mais existir */
            auxNode->next = nextNode->next;
            free(nextNode);
        }
        else if (auxNode->next) /* (*) Não existem mais blocos adjacentes a serem juntados ao bloco livre de referência  */
            auxNode = auxNode->next; /* O bloco disponível de referência passa a ser o próximo, se existir */  
    }
}
/* Ordena os nós. Auxilia o mergeFreeNodes() */
void sortFreeList(Node_F **rootNode, const char order)
{
    Node_F *aNode = *rootNode;
    Node_F *bNode;

    /* Percorre a lista comparando os elementos */
    while (aNode->next != NULL) {

        /* bNode percorre a lista fazendo trocas quando convém */
        bNode = aNode;
        while (bNode->next != NULL) {

            bNode = bNode->next;

            switch (order) {
                case 'a': /* Endereço inicial (address) */
                    if (bNode->addrInit < aNode->addrInit)
                        doSwap(aNode, bNode);
                break;
                case 's': /* Tamanho (size) */
                    if (bNode->sizeB < aNode->sizeB)
                        doSwap(aNode, bNode);
                break;
            }
        }
		aNode = aNode->next;
	}
}
/* Troca tamanho e endereço inicial entre dois nós. Parte de sortFreeList() */
void doSwap(Node_F *aNode, Node_F *bNode)
{
    unsigned aux;
    /* Troca de endereço inicial */
    aux = bNode->addrInit;
    bNode->addrInit = aNode->addrInit;
    aNode->addrInit = aux;
    /* Troca de tamanho */
    aux = bNode->sizeB;
    bNode->sizeB = aNode->sizeB;
    aNode->sizeB = aux;
}

/* Desalocação dos nós ocupados */
void deallocAllUsed(Node_U **head)
{ 
    if (*head != NULL) {
        deallocAllUsed(&(*head)->next);

        *head = NULL;
        free(*head);
    }
}
/* Desalocação dos nós disponíveis */
void deallocAllFree(Node_F **head)
{
    if (*head != NULL) {
        deallocAllFree(&(*head)->next);

        *head = NULL;
        free(*head);
    }
}
/* Pausa a exibição */
void pause()
{
    system("read -p '\nAperte qualquer tecla para retornar...\n' exiting");
}

/* Exibe na tela as informações dos blocos de forma gráfica */
void handlerConsoleMemory(Node_U *usedNode, Node_F *freeNode, const unsigned totalSpace)
{
    gfx_clear();

    consoleUsedPercentage(usedNode, totalSpace); /* Exibe a porcentagem de memória usada */
    consoleLabel(); /* Exibe a legenda dos blocos (cores) */

    /* Pixel inicial e pixel final de cada bloco na janela */
    float pxFinal, pxInit;

    /* Impressão dos blocos disponíveis */
    while (freeNode) {
        /* Cálculo das medidas do bloco em relação à tela */
        pxInit = APP_WIDTH / totalSpace * freeNode->addrInit;
        pxFinal = APP_WIDTH / totalSpace * freeNode->sizeB + pxInit;

        /* Criação e impressão do bloco disponível */
        consoleFreeNode(pxInit, pxFinal, freeNode);

        freeNode = freeNode->next;
    }
    /* Impressão dos blocos ocupados */
    while (usedNode) {
        /* Cálculo das medidas do bloco em relação à tela */
        pxInit = APP_WIDTH / totalSpace * usedNode->addrInit;
        pxFinal = APP_WIDTH / totalSpace * (usedNode->addrFinal - usedNode->addrInit + 1) + pxInit;

        /* Criação e impressão do bloco ocupado */
        consoleUsedNode(pxInit, pxFinal, usedNode);

        /* Linha de divisão entre blocos (exceto quando não há fronteira com algum bloco) */
        gfx_set_color(50, 255, 50);
        if (usedNode->addrInit != 0)
            gfx_filled_rectangle(pxInit + 1, MEMBOX_UPPER, pxInit, MEMBOX_LOWER); /* Lado esquerdo */
        if (usedNode->addrFinal + 1 != totalSpace)
            gfx_filled_rectangle(pxFinal - 1, MEMBOX_UPPER, pxFinal, MEMBOX_LOWER); /* Lado direito */

        usedNode = usedNode->next;
    }
    gfx_paint();
}
/* Exibe o bloco disponível */
void consoleFreeNode(const float pxInit, const float pxFinal, const Node_F *node)
{
    char text[15];
    int width, height; /* Medidas de text */

    gfx_set_font_size(14);
    /* Impressão do bloco disponível */
    gfx_set_color(100, 100, 100);
    gfx_filled_rectangle(pxInit, MEMBOX_UPPER, pxFinal, MEMBOX_LOWER);

    /* Impressão de linhas nas extremidades do bloco (estética) */
    gfx_set_color(50, 50, 50);
    gfx_filled_rectangle(pxInit, MEMBOX_UPPER, pxFinal, MEMBOX_UPPER + 8); /* Linha superior  */
    gfx_filled_rectangle(pxInit, MEMBOX_LOWER, pxFinal, MEMBOX_LOWER - 8); /* Linha inferior */

    gfx_set_color(255, 255, 255);
    /* Impressão do tamanho b do bloco em seu centro */
    snprintf(text, 15, "%u", node->sizeB);
    gfx_get_text_size(text, &width, &height);
    gfx_text((pxFinal + pxInit)/2 - width/2, (MEMBOX_LOWER + MEMBOX_UPPER) / 2 - height/2, text);
    /* Impressão do endereço inicial acima do bloco */
    snprintf(text, 15, "%u", node->addrInit);
    gfx_get_text_size(text, &width, &height);
    gfx_text(pxInit, MEMBOX_LOWER, text);
    /* Impressão do endereço final acima do bloco*/
    snprintf(text, 15, "%u", node->addrInit + node->sizeB - 1);
    gfx_get_text_size(text, &width, &height);
    gfx_text(pxFinal - width, MEMBOX_LOWER, text);
}
/* Exibe o bloco ocupado */
void consoleUsedNode(const float pxInit, const float pxFinal, const Node_U *node)
{ 
    char text[15];
    int width, height; /* Medidas de text */

    gfx_set_font_size(14);
    /* Impressão do bloco ocupado */
    gfx_set_color(255, 50, 50);
    gfx_filled_rectangle(pxInit, MEMBOX_UPPER, pxFinal, MEMBOX_LOWER);

    /* Impressão de linhas nas extremidades do bloco (estética) */
    gfx_set_color(200, 5, 5);
    gfx_filled_rectangle(pxInit, MEMBOX_UPPER, pxFinal, MEMBOX_UPPER + 8); /* Linha superior */
    gfx_filled_rectangle(pxInit, MEMBOX_LOWER, pxFinal, MEMBOX_LOWER - 8); /* Linha inferior */

    gfx_set_color(255, 255, 255);
    /* Impressão do endereço inicial abaixo do bloco */
    snprintf(text, 15, "%u", node->addrInit);
    gfx_get_text_size(text, &width, &height);
    gfx_text(pxInit + 1, MEMBOX_UPPER - height, text);
    /* Impressão do endereço final abaixo do bloco */
    snprintf(text, 15, "%u", node->addrFinal);
    gfx_get_text_size(text, &width, &height);
    gfx_text(pxFinal - width - 1, MEMBOX_UPPER - height, text);

    gfx_set_font_size(26);
    /* Impressão da ID abaixo do bloco */
    snprintf(text, 15, "%u", node->id);
    gfx_get_text_size(text, &width, &height);
    gfx_text((pxFinal + pxInit)/2 - width/2, MEMBOX_LOWER + height, text);
}
/* Exibe a porcentagem de memória usada */
void consoleUsedPercentage(Node_U *usedList, const unsigned totalSpace)
{
    gfx_set_color(255, 255, 255);
    gfx_set_font_size(26);

    char text[20];
    float percentUsed;
    int width, height; /* Medidas de text */

    /* Porcentagem de memória usada */
    percentUsed = (float) totalUsedSpace(usedList) / totalSpace * 100;

    /* Texto (expositivo) */
    snprintf(text, 20, "Memoria em uso");
    gfx_get_text_size(text, &width, &height);
    gfx_text(APP_WIDTH/2 - width/2, MEMBOX_UPPER - 3*height, text);

    /* Texto (porcentagem) */
    snprintf(text, 20, "%.2f%%", percentUsed);
    gfx_get_text_size(text, &width, &height);
    gfx_text(APP_WIDTH/2 - width/2, MEMBOX_UPPER - 2*height, text);
}
/* Exibe a legenda dos blocos (cores) */
void consoleLabel()
{
    gfx_set_font_size(16);

    char text[20];
    int width, height;

    gfx_set_color(255, 255, 255);
    /* Alocados: legenda */
    snprintf(text, 20, "Blocos alocados");
    gfx_get_text_size(text, &width, &height);
    gfx_text(20 + height, APP_HEIGHT - 10 - height, text);
    /* Alocados: caixa de legenda - Contorno */
    gfx_set_color(200, 5, 5);
    gfx_filled_rectangle(10, APP_HEIGHT - 10, 10 + height, APP_HEIGHT - 10 - height);
    /* Alocados: caixa de legenda */
    gfx_set_color(255, 50, 50);
    gfx_filled_rectangle(13, APP_HEIGHT - 13, 7 + height, APP_HEIGHT - 7 - height);

    //////////////////////////////

    gfx_set_color(255, 255, 255);
    /* Disponíveis: legenda */
    snprintf(text, 20, "Blocos disponiveis");
    gfx_text(20 + height, APP_HEIGHT - 20 - 2*height, text);
    /* Disponíveis: caixa de legenda - Contorno */
    gfx_set_color(50, 50, 50);
    gfx_filled_rectangle(10, APP_HEIGHT - 20 - height, 10 + height, APP_HEIGHT - 20 - 2*height);
    /* Disponíveis: caixa de legenda */
    gfx_set_color(100, 100, 100); 
    gfx_filled_rectangle(13, APP_HEIGHT - 23 - height, 7 + height, APP_HEIGHT - 17 - 2*height);
}
