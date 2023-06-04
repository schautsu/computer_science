/*
Author: Vinícius Alves Schautz (viniciusaschautz@gmail.com)
Year: 2023
*/
#include "gfx/gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
/* Definições gerais */
#define APP_WIDTH 600
#define APP_HEIGHT 600
#define CIRCLE_DIAMETER 512
#define SQ_SIDE APP_WIDTH/3
/* Protótipos de funções */
unsigned short mainOption(void);
void handlerCircleSquare(const unsigned short);
void createCircle(unsigned short, float, float, float);
void createSquare(unsigned short, float, float, float, float, float);

int main()
{
	unsigned short option;
	do {
		if ((option = mainOption()) != 3)
			handlerCircleSquare(option);
	} 
	while (option != 3);

	system("clear");

	return 0;
}

unsigned short mainOption() /* Gerenciamento das opções principais */
{
	unsigned short option;
	do {
		system("clear");
		printf("Digite uma opcao:\n\n[1] Circulo\n[2] Quadrado\n[3] Encerrar\n\n--> ");
	}
	while ((!scanf("%hu", &option) && getchar()) || (option < 1 || option > 3));

	return option;
}

void handlerCircleSquare(const unsigned short selected) /* Nível de recursão (iterações) e geração 2D consoante forma escolhida */
{
	unsigned short iterations;
	do {
		system("clear");
		printf("Digite o nivel de recursao [0-10]:\n\n--> ");
	}
	while ((!scanf("%hu", &iterations) && getchar()) || (iterations > 10));

	switch (selected) { /* 1: Círculo 2: Quadrado */
		case 1:
			gfx_init(APP_WIDTH, APP_HEIGHT, "Círculos");
			createCircle(iterations, APP_WIDTH/2, APP_HEIGHT/2, CIRCLE_DIAMETER/2);
		break;
		case 2:
			gfx_init(APP_WIDTH, APP_HEIGHT, "Quadrados");
			createSquare(iterations, SQ_SIDE, SQ_SIDE, 2*SQ_SIDE, 2*SQ_SIDE, SQ_SIDE);
		break;
	}
	gfx_paint();
	sleep(5);
	gfx_quit();
}

void createCircle(unsigned short iterations, float centerX, float centerY, float radius)
{ /* Lógica usada: coordenadas polares para descobrir os pontos/centros dos círculos recursivos */
	if (iterations)
	{ /* Chamadas recursivas para a criação das formas homólogas nos diferentes quadrantes */
		createCircle(iterations-1, centerX + ((radius/2)*(sqrt(2)/2)), centerY - ((radius/2)*(sqrt(2)/2)), radius/2); /* Superior direito */
		createCircle(iterations-1, centerX - ((radius/2)*(sqrt(2)/2)), centerY + ((radius/2)*(sqrt(2)/2)), radius/2); /* Inferior esquerdo */

		createCircle(iterations-1, centerX - ((radius/2)*(sqrt(2)/2)), centerY - ((radius/2)*(sqrt(2)/2)), radius/2); /* Superior esquerdo */
		createCircle(iterations-1, centerX + ((radius/2)*(sqrt(2)/2)), centerY + ((radius/2)*(sqrt(2)/2)), radius/2); /* Inferior direito */
	}
	gfx_ellipse(centerX, centerY, radius, radius); /* Geração da circunferência consoante parâmetros */
}

void createSquare(unsigned short iterations, float initX, float initY, float finalX, float finalY, float side)
{ /* Lógica usada: de forma abstrata, pensar coisas como "quanto o initX do quadrado maior tem que ser deslocado para se tornar o initX do menor?", etc. */
	if (iterations)
	{ /* Chamadas recursivas para a criação de formas homólogas nos diferentes direções */
		createSquare(iterations-1, initX + side, initY + side/4, finalX + side/2, finalY - side/4, side/2); /* Direita */
		createSquare(iterations-1, initX + side/4, initY + side, finalX - side/4, finalY + side/2, side/2); /* Inferior */
		createSquare(iterations-1, initX - side/2, initY + side/4, finalX - side, finalY - side/4, side/2); /* Esquerda */
		createSquare(iterations-1, initX + side/4, initY - side/2, finalX - side/4, finalY - side, side/2); /* Superior */
	}
	/* Geração do quadrado não visível para esconder a presença de quadrados internos (preenchido em preto) */
	gfx_set_color(0, 0, 0);
	gfx_filled_rectangle(initX+1, initY+1, finalX-1, finalY-1); /* 1px menor que o quadrado visível */
	/* Geração do quadrado visível (linhas brancas) */
	gfx_set_color(255, 255, 255);
	gfx_rectangle(initX, initY, finalX, finalY);
}
