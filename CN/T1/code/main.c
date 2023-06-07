/*
    Author: Vinícius Alves Schautz (viniciusaschautz@gmail.com)
    Year: 2023
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* Protótipos de funções */
short mainOption(void);

void consoleMatrix(const short order, double (*)[order]);
void consoleInverseOperationMatrix(const short order, double (*)[order], double (*)[order]);

void handlerInverseMatrix(void);
void inverseMatrix(const short order, double (*)[order], double (*)[order]);
bool swapPivotIsZero(const short order, const short, double (*)[order], double (*)[order]);

void handlerLinearSystem(void);
void linearSystem(const short order, double (*)[order], double *, double *, const double);
bool rowsCriteria(const short order, double (*)[order]);
bool swapCriteriaIsFalse(const short order, double (*)[order], double *);
void swapRowsLinear(const short order, const short, const short, double (*)[order], double *);
bool isHigherPrecision(const short, const double *, const double *, const double);

void pause(void);
////////////////////
int main()
{
    short option;
    do {
        option = mainOption();

        switch (option) {
            case 1:
                handlerInverseMatrix();
            break;
            case 2:
                handlerLinearSystem();
            break;
        }
    } while (option != 3);

    system("clear");

    return 0;
}
////////////////////
short mainOption()
{
    short option;
    do {
        system("clear");
        printf("Escolha uma opcao:\n\n[1] Inversao de matrizes\n[2] Resolucao de sistemas lineares\n[3] Encerrar\n\n--> ");
    }
    while ((!scanf("%hd", &option) && getchar()) || (option < 1 || option > 3));

    return option;
}
/* Impressão de uma matriz qualquer */
void consoleMatrix(const short order, double (*m)[order])
{
    short row, column;

    puts("");
    for (row = 0; row < order; ++row) {
        for (column = 0; column < order; ++column) {
                printf("%.1lf\t", m[row][column]);
        }
        printf("\n");
    }
    puts("");
}
/* Impressão das matrizes inicial e inversa */
void consoleInverseOperationMatrix(const short order, double (*mat)[order], double (*inv)[order])
{
    short row, column;

    puts("");
    for (row = 0; row < order; ++row) {

        for (column = 0; column < order; ++column) {
            printf("%.1lf\t", mat[row][column]);
        }
        printf(":\t");

        for (column = 0; column < order; ++column) {
            printf("%.1lf\t", inv[row][column]);
        }
        printf("\n");
    }
    puts("");
}

/* Controla definições gerais da inversa pré-operação */
void handlerInverseMatrix()
{
    short order; /* Leitura da ordem da matriz */
    do {
        system("clear");
        printf("Digite a ordem da matriz [2-10]:\n\n--> ");
    }
    while ((!scanf("%hd", &order) && getchar()) || (order < 2 || order > 10));

    /* Matrizes inicial M e inversa (que será gerada com base na inicial) */
    double (*mat)[order], (*inv)[order];

    mat = malloc(sizeof(*mat) * order);
    inv = malloc(sizeof(*inv) * order);

    /* Leitura dos elementos da matriz inicial */
    system("clear");
    short row, column;

    for (row = 0; row < order; ++row) {
        for (column = 0; column < order; ++column) {
            do {
                printf("Digite o elemento M[%hd][%hd] --> ", row+1, column+1);
            }
            while (!scanf("%lf", &mat[row][column]) && getchar());

            inv[row][column] = row == column ? 1 : 0; /* Geração da matriz identidade (inversa inicial) */
        }
    }

    /* Execução do processo de geração da matriz inversa e tempo de execução */
    clock_t begin = clock();

    inverseMatrix(order, mat, inv);

    clock_t end = clock();

    printf("\nTempo de execucao: %lfs\n", (double)(end - begin) / CLOCKS_PER_SEC);

    free(mat);
    free(inv);

    pause();
}
/* Gera a matriz inversa */
void inverseMatrix(const short order, double (*mat)[order], double (*inv)[order])
{
    system("clear");
    double mpl;
    short row, column, kol;

    /* Impressão da matriz inicial M */
    printf("Matriz inicial M:\n");
    consoleMatrix(order, mat);
    consoleInverseOperationMatrix(order, mat, inv);

    /* Navega pelas linhas de uma coluna (para zerar os elementos com base no pivô da coluna) */
    for (column = 0; column < order; ++column) {
        /* Verifica se o pivô é nulo. Se for, tenta trocar linhas com a próxima que não tenha seu candidato a pivô nulo */
        /* Caso não haja possibilidade de trocar linhas, não existe inversa de M e retorna */
        if (!mat[column][column] && !swapPivotIsZero(order, column, mat, inv)) {
           printf("[ERRO] Matriz nao inversivel!\n");
           return;
        }

        for (row = 0; row < order; ++row) {
            /* Se o elemento não for o pivô e nem nulo, inicia-se a operação para zerá-lo */
            if ((row != column) && (mat[row][column])) {
                mpl = -mat[row][column] / mat[column][column]; /* multiplicador = (-1)*queremos zerar / pivô */

                for (kol = 0; kol < order; ++kol) { /* Operações com a linha do alvo e a linha do pivô */
                    mat[row][kol] += mpl*mat[column][kol];
                    inv[row][kol] += mpl*inv[column][kol];
                }
                /* Impressão descritiva da operação e das matrizes */
                printf("L%hd = %.3lf * L%hd + L%hd\n", row+1, mpl, column+1, row+1); /* row+1: L_dest; column+1: L_src */
                consoleInverseOperationMatrix(order, mat, inv);
            }  
        }
    }
    /* Simplificação da diagonal principal (1 - identidade) */
    for (row = 0; row < order; ++row) {

        if (mat[row][row] != 1) {
            mpl = 1/mat[row][row]; /* Se o elemento da diagonal principal já não for 1, o multiplicador recebe 1 / elemento */

            for (kol = 0; kol < order; ++kol) {
                if (mat[row][kol]) /* Se o elemento for nulo, não faz nada (para evitar erros como "-0.0") */
                    mat[row][kol] *= mpl;
                if (inv[row][kol])
                    inv[row][kol] *= mpl;
            }
            /* Impressão descritiva da operação e das matrizes */
            printf("L%hd = %.3lf * L%hd\n", row+1, mpl, row+1); /* row+1: L_dest */
            consoleInverseOperationMatrix(order, mat, inv);
        }
    }
    /* Impressão final da inversa */
    printf("Matriz inversa de M:\n");
    consoleMatrix(order, inv);
}
/* Procura o primeiro candidato a pivô que não seja nulo (0) e troca as linhas */
bool swapPivotIsZero(const short order, const short index, double (*mat)[order], double (*inv)[order]) 
{
    short i, j;
    double aux;

    for (i = 1; index + i < order; ++i)
    { 
        if (mat[index+i][index]) /* Não é nulo */
        {
            for (j = 0; j < order; ++j) {
                aux = mat[index+i][j];
                mat[index+i][j] = mat[index][j];
                mat[index][j] = aux;

                aux = inv[index+i][j];
                inv[index+i][j] = inv[index][j];
                inv[index][j] = aux;
            }
            /* Impressão descritiva da operação e das matrizes */
            printf("L%hd = L%hd\nL%hd = L%hd\n", index+1, index+1 + i, index+1 + i, index+1);
            consoleInverseOperationMatrix(order, mat, inv);

            return true; /* Troca feita com sucesso */
        }
    }
    return false; /* Não encontrou um candidato adequado (nulo ou não há mais linhas) */
}

/* Controla definições gerais do sistema linear pré-operação */
void handlerLinearSystem()
{
    short order; /* Leitura da ordem da matriz */
    do {
        system("clear");
        printf("Digite a ordem da matriz dos coeficientes [2-10]:\n\n--> ");
    }
    while ((!scanf("%hd", &order) && getchar()) || (order < 2 || order > 10));

    /* Matrizes que conterão os coeficientes e resultados, e solução inicial e precisão */
    double (*a)[order], *b, *solution, precision;

    a = malloc(sizeof(*a) * order);
    b = malloc(sizeof(b) * order);
    solution = malloc(sizeof(solution) * order);

    /* Leitura dos coeficientes e resultados */
    system("clear");
    short row, column;

    for (row = 0; row < order; ++row) {
        for (column = 0; column < order; ++column) {
            do {
                printf("Linha %hd: coeficiente %hd --> ", row+1, column+1);
            } while (!scanf("%lf", &a[row][column]) && getchar());
        }
        do {
            printf("Linha %hd: resultado --> ", row+1);
        } while (!scanf("%lf", &b[row]) && getchar());

        printf("\n");
    }
    /* Leitura da solução inicial (x(0)) */
    for (row = 0; row < order; ++row) {
        do {
            printf("Solução inicial [x(0)]: x%hd --> ", row+1);
        } while (!scanf("%lf", &solution[row]) && getchar());
    }
    /* Leitura da precisão */
    do {
        printf("\nPrecisao --> ");
    } while ((!scanf("%lf", &precision) && getchar()) || (precision < 0.0));
    
    /* Execução do processo de resolução do sistema e tempo de execução */
    clock_t begin = clock();

    linearSystem(order, a, b, solution, precision);

    clock_t end = clock();

    printf("\nTempo de execucao: %lfs\n", (double)(end - begin) / CLOCKS_PER_SEC);

    free(a);
    free(b);
    free(solution);

    pause();
} 
/* Resolve o sistema linear */
void linearSystem(const short order, double (*a)[order], double *b, double *solution_zero, const double precision)
{
    static int iteration = 0; /* Contador de iteração */
    short i, j;
    double solution_one[order]; /* Array que armazena a solução da iteração */

    /* Verifica se o critério das linhas é atendido. Se não, tenta trocar linhas */
    /* Caso não tenha sucesso, a convergência não é garantida e retorna */
    if (!rowsCriteria(order, a) && !swapCriteriaIsFalse(order, a, b)) {
        printf("\nConvergencia nao garantida!\n");
        return;
    }
    /* Realiza o cálculo do x(k+1) de cada linha */
    for (i = 0; i < order; ++i) {
        solution_one[i] = b[i]; /* A nova solução x(k+1) da linha recebe o resultado anterior (... = res) */

        for (j = 0; j < order; ++j) {
            if (i == j) ++j; /* Evita a diagonal principal */
            /* Nova solução[linha] -= elemento da linha * solução anterior[linha] */
            solution_one[i] -= a[i][j] * solution_zero[j];
        }
        solution_one[i] /= a[i][i]; /* Divide pelo elemento da diagonal principal */
    }
    /* Impressão da solução da iteração corrente */
    puts("");
    for (i = 0; i < order; ++i) {
        if (i == order/2) 
            printf("x(%d) =\t%.7lf\n", ++iteration, solution_one[i]);
        else
            printf("      \t%.7lf\n", solution_one[i]);
    }
    /* Verifica se a precisão ainda é maior que a definida. Se for, faz nova iteração */
    if (isHigherPrecision(order, solution_one, solution_zero, precision))
        linearSystem(order, a, b, solution_one, precision);
    else {
        printf("\n\n| Solucao: x(%d) |\n", iteration); /* Precisão atual <= que a definida, sucesso */
        iteration = 0; /* Retorna a iteração para seu nível inicial, já que a execução acabou */
    }
}
/* Critério das linhas */
bool rowsCriteria(const short order, double (*a)[order])
{
    int row, column;
    double sum;
    bool converge = true; /* Assume-se que converge */

    for (row = 0; row < order; ++row) {

        sum = 0 ;
        for (column = 0; column < order; ++column) {
            if (row != column) sum += fabs(a[row][column]);
        }
        if (sum / fabs(a[row][row]) >= 1) { /* Se o resultado não é menor que 1, vai tentar trocar linhas */
            converge = false; /* Não converge */
            break;
        }
    }
    return converge; /* true or false */
}
/* Não converge; torna diagonalmente dominante */
bool swapCriteriaIsFalse(const short order, double (*a)[order], double *b)
{
    int row, column, gtCol;
    double greatest;

    for (row = 0; row < order; ++row) {

        gtCol = greatest = 0; /* coluna do maior elemento da linha e maior elemento da linha */
        for (column = 0; column < order; ++column) {
            if (fabs(a[row][column]) > greatest) { /* Verifica qual é o maior elemento da linha */
                greatest = fabs(a[row][column]);
                gtCol = column;
            }
        }
        if (greatest != a[row][row]) { /* Se o maior elemento for diferente do que já é diagonal principal */
            swapRowsLinear(order, row, gtCol, a, b); /* Deixa os maiores elementos de cada linha na diagonal principal */
        }
    }
    return rowsCriteria(order, a); /* Retorna o resultado de uma nova verificação do critério das linhas */
}
/* Troca de linhas */
void swapRowsLinear(const short order, const short rowOne, const short rowTwo, double (*a)[order], double *b)
{
    short column;
    double rowElement;

    for (column = 0; column < order; ++column) {
        rowElement = a[rowOne][column];
        a[rowOne][column] = a[rowTwo][column];
        a[rowTwo][column] = rowElement;
    }
    rowElement = b[rowOne];
    b[rowOne] = b[rowTwo];
    b[rowTwo] = rowElement;
}
/* Calcula e verifica se a precisão é superior à definida */
bool isHigherPrecision(const short order, const double *solution_one, const double *solution_zero, const double precision)
{
    short i;
    double largestOneZero = 0.0, largestOne = 0.0, error;

    for (i = 0; i < order; ++i) {
        if (fabs(solution_one[i] - solution_zero[i]) > largestOneZero)
            largestOneZero = fabs(solution_one[i] - solution_zero[i]); /* max[|x(k+1) - x(k)|] */
        
        if (fabs(solution_one[i]) > largestOne) 
            largestOne = fabs(solution_one[i]); /* max[|x(k+1)|] */
    }
    error = largestOneZero/largestOne; /* Precisão */
    printf("\nErro: %.7lf\n", error);

    return (error > precision); /* Retorna a verificação se é maior ou não (true or false) */
}

/* Pausa a exibição */
void pause()
{
    system("read -p '\nAperte qualquer tecla para retornar...\n' exiting");
}
