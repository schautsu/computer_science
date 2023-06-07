/*
    Author: Vinícius Alves Schautz (viniciusaschautz@gmail.com)
    Year: 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ERR_OPEN_CODE "\n[ERRO] Nao foi possivel abrir o arquivo codificado"
#define ERR_OPEN_DECODE "\n[ERRO] Nao foi possivel abrir o arquivo decodificado"

#define MAX_CHAR_LEN 80
#define MAX_LINE_LEN 100
#define MAX_ROWS 800

#define file_code "readme.code.txt"
#define file_decode "readme.decifra.txt"
#define file_nlines "readme.nlines.txt"

/*Function prototypes*/
int escolha_menu(void);
void handler_menu(void);
void gera_menu(void);
void decifra_arquivo(void);
void grava_linhas(void);
void numero_linhas(void);
void maior_linha(void);
void pesquisa_palavra(void);
void flush_in(void);

int main()
{
    FILE *fp_code;

    if ((fp_code = fopen(file_code, "r"))) {
        handler_menu();
    } else {
        perror(ERR_OPEN_CODE);
    }
    fclose(fp_code);

    return (0);
}

void handler_menu() /*Gerenciador de opcoes do menu*/
{
    int escolha;
    do {
        escolha = escolha_menu();

        switch (escolha) {
            //Decifra arquivo
            case 1:
                decifra_arquivo();
            break;
            //Grava numero linha em arquivo
            case 2:
                grava_linhas();
            break;
            //Quantas linhas o arquivo tem
            case 3:
                numero_linhas();
            break;
            //Qual a maior linha
            case 4:
                maior_linha();
            break;
            //Pesquisar uma palavra
            case 5:
                pesquisa_palavra();
            break;
        }
    } while (escolha != 0);
}

void gera_menu() /*Exibicao do menu de opcoes*/
{
    system("cls");
    fputs("\n[1] Decifra arquivo\n[2] Grava numero linha em arquivo\n[3] Quantas linhas tem o arquivo\n[4] Qual a maior linha\n[5] Pesquisar uma palavra\n[0] Encerrar\n\nQual opcao: ", stdout);
}

int escolha_menu() /*Gravacao e verificacao de escolha*/
{
    int escolha;
    do {
        gera_menu();
    } while ((escolha < 1 && escolha > 5) || (!scanf("%d", &escolha) && getchar()));

    return (escolha);
}

void decifra_arquivo() /*Decodifica o arquivo .txt na pasta*/
{
    FILE *fp_code, *fp_decode;

    fp_code = fopen(file_code, "r");
    fp_decode = fopen(file_decode, "w");

    char fElement, chrTemp[4], chrFinal;
    int inTemp, index = 0;

    while (fread(&fElement, 1, 1, fp_code))
    {
        if (fElement != '[' && fElement != ']') {
            chrTemp[index++] = fElement; //Armazena como char cada numero de um caracter ASCII
        }
        else if (fElement == ']') { //Leu 1 caractere [...]
            inTemp = atoi(chrTemp);
            chrFinal = (char) inTemp;

            fwrite(&chrFinal, 1, 1, fp_decode); //Registra o caracter no arquivo decifrado
            
            index = 0;
            memset(chrTemp, 0, sizeof(chrTemp)); //Reset do array temporario
        }
    }
    fclose(fp_code);
    fclose(fp_decode);

    puts("\n[SUCESSO] Arquivo decifrado!\n");

    system("pause");
}

void grava_linhas() /*Cria um arquivo com linhas enumeradas*/
{
    FILE *fp_decode, *fp_nlines;

    if ((fp_decode = fopen(file_decode, "r")))
    {
        fp_nlines = fopen(file_nlines, "w");

        char lineTemp[MAX_LINE_LEN];
        int numRows = 0;

        while (fgets(lineTemp, MAX_LINE_LEN, fp_decode))
            fprintf(fp_nlines, "[linha:%d] %s", ++numRows, lineTemp); //Registra a string-linha no arquivo
        
        fclose(fp_nlines);

        puts("\n[SUCESSO] Arquivo enumerado!\n");
    } 
    else { //Erro
        perror(ERR_OPEN_DECODE);
        puts("");
    }
    fclose(fp_decode);

    system("pause");
}

void numero_linhas() /*Informa o numero de linhas do arquivo decifrado*/
{
    FILE *fp_decode;

    if ((fp_decode = fopen(file_decode, "r")))
    {
        char lineTemp[MAX_LINE_LEN];
        int numRows = 0;

        while (fgets(lineTemp, MAX_LINE_LEN, fp_decode))
            numRows++; //Contador de linhas
        
        printf("\nO arquivo %s tem (%d) linhas.\n\n", file_decode, numRows);
    }
    else { //Erro
        perror(ERR_OPEN_DECODE);
        puts("");
    }
    fclose(fp_decode);

    system("pause");
}

void maior_linha() /*Informa qual e a maior linha e quantos caracteres ela tem*/
{
    FILE *fp_decode;

    if ((fp_decode = fopen(file_decode, "r")))
    {
        char lineTemp[MAX_LINE_LEN]; 
        unsigned numRows = 0, maior_numChr = 0, maior_row = 0;

        while (fgets(lineTemp, MAX_LINE_LEN, fp_decode))
        {
            numRows++;   

            if (strlen(lineTemp) > maior_numChr) { //Verifica se a linha atual tem mais caracteres do que a maior
                maior_row = numRows;
                maior_numChr = strlen(lineTemp);
            }
        }
        
        printf("\nA linha [%u] e a maior com (%u) caracteres.\n\n", maior_row, maior_numChr);
    }
    else { //Erro
        perror(ERR_OPEN_DECODE);
        puts("");
    }
    fclose(fp_decode);

    system("pause");
}

void pesquisa_palavra() /*Procura uma dada palavra e exibe sua frequencia*/
{
    FILE *fp_decode;

    if ((fp_decode = fopen(file_decode, "r")))
    {
        char word[MAX_CHAR_LEN], linesExist[MAX_ROWS][MAX_LINE_LEN];
        char rowTemp[MAX_LINE_LEN], tempStr[MAX_LINE_LEN], *subStr;
        unsigned numRows = 0, existFile = 0, existLine = 0, indexExist = 0, i;

        fputs("\nQual palavra pesquisar: ", stdout);
        flush_in();
        fgets(word, MAX_CHAR_LEN, stdin);

        word[strlen(word) - 1] = 0; //remove '\n'

        while (fgets(rowTemp, MAX_LINE_LEN, fp_decode))
        {
            numRows++;
            strcpy(tempStr, rowTemp); //tempStr recebe a string-linha atual do arquivo decodificado

            for (i = 0; i < strlen(tempStr); i++) { //Substitui qualquer char nao alfanumerico por SPACE
                if (!isalnum(tempStr[i]))
                    tempStr[i] = ' ';
            }
            subStr = strtok(tempStr, " "); //Divide certo (sem pontuaçoes etc.)

            while (subStr) 
            {
                if (!strcmp(subStr, word)) //Verifica a ocorrencia da palavra, comparando-a com as palavras da linha
                {
                    existLine++;

                    if (existLine == 1) {
                        rowTemp[strlen(rowTemp) - 1] = 0;
                        snprintf(linesExist[indexExist++], MAX_LINE_LEN, "[linha:%d] %s", numRows, rowTemp);
                        //Registra a string-linha de ocorrencia
                    }
                }
                subStr = strtok(NULL, " ");
            }    
            existFile += existLine; //Numero total de ocorrencias
            existLine = 0;
        }
        //Saidas
        if (existFile) {
            printf("\nAs linhas em que a palavra '%s' ocorre:\n\n", word);

            for (i = 0; i < indexExist; i++) {
                printf("%s\n", linesExist[i]); 
            }
            printf("\ne tem %d ocorrencias.\n\n", existFile);
        }
        else printf("\nNao existem ocorrencias da palavra '%s'!\n\n", word);
    } 
    else { //Erro
        perror(ERR_OPEN_DECODE);
        puts("");
    }
    fclose(fp_decode);

    system("pause");
}

void flush_in() /*Limpeza do buffer do teclado*/
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
