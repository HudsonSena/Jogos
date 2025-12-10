#include "dinossauro.h"
#include "config.h"
#include <allegro5/allegro_primitives.h>

void dinossauro_inicializar(Dinossauro *dino, float x_inicial) {
    dino->x = x_inicial;
    dino->y = ALTURA_CHAO - DINOSSAURO_ALTURA;
    dino->dy = 0.0;
    dino->dx = 0.0;
    dino->vida = 100;
    dino->poder_bonus = 0;
    dino->tempo_bonus = 0.0;
    dino->em_pulso = 0;
    dino->no_chao = 1;
    dino->fase_atual = 1;
    dino->score = 0;
}

void dinossauro_desenhar(Dinossauro *dino) {
    ALLEGRO_COLOR cor_dino = al_map_rgb(0, 150, 0);

    if (dino->poder_bonus == 1) {
        cor_dino = al_map_rgb(255, 255, 0);
    } else if (dino->poder_bonus == 2) {
        cor_dino = al_map_rgb(0, 0, 255);
    }

    al_draw_filled_rectangle(dino->x, dino->y, 
                             dino->x + DINOSSAURO_LARGURA, dino->y + DINOSSAURO_ALTURA, 
                             cor_dino);
}

void desenhar_chao() {
    ALLEGRO_COLOR cor_chao = al_map_rgb(100, 50, 0);

    al_draw_filled_rectangle(0, ALTURA_CHAO, LARGURA_TELA, ALTURA_TELA, cor_chao);
}

void dinossauro_pular(Dinossauro *dino) {
    if (dino->no_chao) {
        dino->dy = -ALTURA_PULO;
        dino->no_chao = 0;
    }
}

void dinossauro_atualizar(Dinossauro *dino) {
    dino->y += dino->dy;
    dino->x += dino->dx;

    if (dino->y + DINOSSAURO_ALTURA < ALTURA_CHAO) {
        dino->dy += GRAVIDADE;
        dino->no_chao = 0;
    } 
    
    if (dino->y + DINOSSAURO_ALTURA >= ALTURA_CHAO) {
        dino->y = ALTURA_CHAO - DINOSSAURO_ALTURA;
        dino->dy = 0.0;
        dino->no_chao = 1;
    }

    if (dino->x < 0) {
        dino->x = 0;
        dino->dx = 0;
    }
    
    if (dino->x + DINOSSAURO_LARGURA > LARGURA_TELA) {
        dino->x = LARGURA_TELA - DINOSSAURO_LARGURA;
        dino->dx = 0;
    }
}