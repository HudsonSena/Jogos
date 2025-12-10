#ifndef METEORO_H
#define METEORO_H

#include <allegro5/allegro.h>
#include "dinossauro.h" 
#include "config.h"

#define METEORO_BOUNCE_DISPLACEMENT -150.0
#define MARGEM_HORIZONTAL_PERCENTUAL 0.20
#define MAX_METEOROS 3
#define METEORO_LARGURA 30
#define METEORO_ALTURA 30
#define VELOCIDADE_INICIAL_JOGO 2.0
#define DANO_CHAO_NIVEL_1 10
#define DANO_CHAO_NIVEL_2 25
#define DANO_CHAO_NIVEL_3 40
#define FORCA_RECUO_METEORO -10.0
#define PONTOS_POR_METEORO_NIVEL_1 10
#define PONTOS_POR_METEORO_NIVEL_2 30
#define PONTOS_POR_METEORO_NIVEL_3 50

typedef struct {
    float x;
    float y;
    float velocidade_y;
    float velocidade_y_base;
    int nivel;
    int vida_atual;
    int vida_maxima;
    int dano_chao;
    int ativo;
} Meteoro;

void limpar_meteoros(Meteoro meteoros[]);

void inicializar_meteoros(Meteoro meteoros[]);
void gerar_novo_meteoro(Meteoro meteoros[], float dificuldade_fator); 
void atualizar_meteoros(Meteoro meteoros[]);
void desenhar_meteoros(Meteoro meteoros[]);

int checar_e_tratar_chao(Meteoro meteoros[]);

int checar_colisao_com_cabeca(Dinossauro *dino, Meteoro meteoros[]);

#endif