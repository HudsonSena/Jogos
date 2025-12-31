#ifndef DINOSSAURO_H
#define DINOSSAURO_H

#include <allegro5/allegro.h>

#define GRAVIDADE 0.8
#define ALTURA_PULO 22.0
#define DINOSSAURO_LARGURA 60
#define DINOSSAURO_ALTURA 80
#define VELOCIDADE_X 8.0
#define FORCA_IMPACTO_DESCENDENTE 10.0
#define ALTURA_CHAO (ALTURA_TELA - 100)

typedef struct {
    float x;
    float y;
    float dy;
    float dx;
    int vida;
    int poder_bonus;
    double tempo_bonus;
    int em_pulso;
    int no_chao;
    int score;
    int fase_atual;
    int cor;
    double tempo_fase;
} Dinossauro;

void dinossauro_ejetar_para_chao(Dinossauro *dino);

void dinossauro_inicializar(Dinossauro *dino, float x_inicial);

void dinossauro_pular(Dinossauro *dino);

void dinossauro_atualizar(Dinossauro *dino);

void dinossauro_desenhar(Dinossauro *dino);

void desenhar_chao();

#endif