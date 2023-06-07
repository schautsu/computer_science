#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define file_carro "carro.dbf"
#define file_ord "carro.ord"

#define ERR_OPEN_CARRO "\n[ERRO] Nao foi possivel abrir o arquivo binario requerido"
#define ERR_OPEN_ORD "\n[ERRO] Nao foi possivel abrir o arquivo binario ordenado"

#define TAM 50

typedef struct {
    long int id_reg;
    char placa[9];
    char modelo[TAM];
    char fabricante[TAM];
    int ano_fabricacao;
    int ano_modelo;
    char combustivel[TAM];
    char cor[TAM];
    int opcional[8];
    float preco_compra;
} CARRO;

char opcionais[][TAM] = {
    {"4.portas"},
    {"cambio.automatico"},
    {"vidros.eletricos"},
    {"abs"},
    {"air.bag"},
    {"ar.condicionado"},
    {"banco.couro"},
    {"sensor.estacionamento"}
};
/*Menu*/
int escolha_menu(void);
void handler_menu(void);
void gera_menu(void);
/*Funcoes principais*/
void numero_registros(void);
void ordena_registros(void);
void mostra_registros(void);
void numero_combustivel(void);
void numero_opcional(void);
/*Funcoes auxiliares*/
int total_registros(FILE *);
void ordena_array(CARRO *, int);
void display_registro(CARRO);

int main()
{
    FILE *fp_carro = fopen(file_carro, "rb");

    if (fp_carro) {
        handler_menu();
    } else {
        perror(ERR_OPEN_CARRO);
    }
    fclose(fp_carro);

    return (0);
}

void handler_menu()
{
    int escolha;

    do {
        escolha = escolha_menu();

        switch (escolha) {
            //Informar quantos registros tem "carro.dbf"
            case 1:
                numero_registros();
            break;
            //Ordenar os registros de "carro.dbf" de forma crescente pelo campo "placa" e armazena-los no arquivo "carro.ord"
            case 2:
                ordena_registros();
            break;
            //Mostrar os registros de "carro.ord"
            case 3:
                mostra_registros();
            break;
            //Para cada tipo de combustivel informar a quantidade de carros
            case 4:
                numero_combustivel();
            break;
            //Para cada tipo de opcional informar a quantidade de carros
            case 5:
                numero_opcional();
            break;
        }
    } while (escolha != 0);
}

void gera_menu() /*Exibicao do menu de opcoes*/
{
    system("cls");

    printf("\nArquivo-base: '%s'\n", file_carro);
    fputs("\n[1] Numero de registros\n[2] Ordenar registros: Placa - Crescente\n[3] Exibir registros ordenados\n[4] Quantidade de carros por combustivel\n[5] Quantidade de carros por opcional\n[0] Encerrar\n\nQual opcao: ", stdout);
}

int escolha_menu() /*Gravacao e verificacao de escolha*/
{
    int escolha;

    do {
        gera_menu();
    } while ((escolha < 1 && escolha > 5) || (!scanf("%d", &escolha) && getchar()));

    return (escolha);
}

void numero_registros()
{
    FILE *fp_carro = fopen(file_carro, "rb");
    int numReg;

    numReg = total_registros(fp_carro);

    printf("\nO arquivo tem (%d) registros.\n\n", numReg);

    fclose(fp_carro);

    system("pause");
}

void ordena_registros()
{
    FILE *fp_carro = fopen(file_carro, "rb");
    FILE *fp_ord = fopen(file_ord, "wb");

    int i = 0, numReg = total_registros(fp_carro);
    CARRO c, cArray[numReg];

    while (fread(&c, sizeof(CARRO), 1, fp_carro)) { //cArray recebe as estruturas do arquivo desordenado
        cArray[i++] = c;
    }
    
    ordena_array(cArray, numReg); //Ordenacao de cArray em ordem crescente por Placa

    for (i = 0; i < numReg; i++) { //Gravacao no arquivo ordenado
        fwrite(&cArray[i], sizeof(CARRO), 1, fp_ord);
    }
    printf("\n[SUCESSO] O arquivo ordenado (%s) foi gerado!\n\n", file_ord);

    fclose(fp_carro);
    fclose(fp_ord);

    system("pause");
}

void mostra_registros()
{
    system("cls");

    FILE *fp_ord = fopen(file_ord, "rb");

    if (fp_ord)
    {
        int count = 0;
        CARRO c;

        while (fread(&c, sizeof(CARRO), 1, fp_ord))
        {
            count++;

            display_registro(c); //impressao das informacoes do carro
            /*
            A seguinte condicional limita a 300 registros impressos por vez (ate o ultimo, eventualmente);
            Isso evita que informacoes antigas impressas sejam perdidas/cortadas, como no caso de imprimir todos
            os registros existentes de uma vez, em que somente os ultimos 500 (aprox.) podiam ser observados na output
            */
            if (!(count % 300)) {
                puts("");
                system("pause"); 
                system("cls");
            }
        }
    }
    else {
        perror(ERR_OPEN_ORD);
    }
    puts("");

    fclose(fp_ord);

    system("pause");
}

void numero_combustivel()
{
    FILE *fp_carro = fopen(file_carro, "rb");

    int alcool = 0, diesel = 0, flex = 0, gasolina = 0;
    CARRO c;
    
    while (fread(&c, sizeof(CARRO), 1, fp_carro))
    {
        if (c.combustivel[0] == 'a')
            alcool++;
        else if (c.combustivel[0] == 'd')
            diesel++;
        else if (c.combustivel[0] == 'f')
            flex++;
        else    
            gasolina++;
    }
    printf("\n: Alcool\t(%d)\n: Diesel\t(%d)\n: Flex\t\t(%d)\n: Gasolina\t(%d)\n\n", alcool, diesel, flex, gasolina);

    fclose(fp_carro);

    system("pause");
}

void numero_opcional()
{
    FILE *fp_carro = fopen(file_carro, "rb");

    int i, tam, opc[8];
    CARRO c;

    memset(opc, 0, 8*sizeof(int)); //Inicializa o array em 0

    while (fread(&c, sizeof(CARRO), 1, fp_carro))
    {
        for (i = 0; i < 8; i++) {
            if (c.opcional[i]) opc[i]++; //Contador por opcional
        }
    }

    for (i = 0; i < 8; i++) //Impressao dos opcionais (com output formatada)
    { 
        tam = strlen(opcionais[i]);
        
        if (tam > 14)
            printf("\n: %s\t(%d)", opcionais[i], opc[i]);
        else if (tam > 3)
            printf("\n: %s\t\t(%d)", opcionais[i], opc[i]);
        else
            printf("\n: %s\t\t\t(%d)", opcionais[i], opc[i]);
    }
    puts("\n");

    fclose(fp_carro);

    system("pause");
}
////
int total_registros(FILE *fp)
{
    int endFile, numReg;

    fseek(fp, 0, SEEK_END);

    endFile = ftell(fp);
    numReg = endFile / sizeof(CARRO);

    rewind(fp);

    return (numReg);
}

void ordena_array(CARRO *cArray, int numReg)
{
    int i, j;
    CARRO aux;

    for (i = 0; i < numReg; i++) { 
        for (j = 0; j < numReg; j++) {  

            if ((strcmp(cArray[i].placa, cArray[j].placa)) < 0) {
                aux = cArray[i];
                cArray[i] = cArray[j];
                cArray[j] = aux;
            }
        }
    }
}

void display_registro(CARRO c)
{
    int i = 0, j = 0;

    printf("\n: ID\t\t%ld\n: Placa\t\t%s\n: Modelo\t%s\n: Fabricante\t%s\n: A.Fabricacao\t%d\n: A.Modelo\t%d\n: Combustivel\t%s\n: Cor\t\t%s\n\n", c.id_reg, c.placa, c.modelo, c.fabricante, c.ano_fabricacao, c.ano_modelo, c.combustivel, c.cor);

    while (i < 8) c.opcional[i] ? printf(": Opcional (%d)\t%s\n", ++j, opcionais[i++]) : i++; //impressao de opcionais

    printf("\n: Preco\t\tR$ %.2lf\n", c.preco_compra);
    puts("=====================================");
}
