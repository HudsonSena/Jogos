#include "meteoro.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h> 

#include "config.h" 

void inicializar_meteoros(Meteoro meteoros[]) {
    srand(time(NULL)); 
    
    for (int i = 0; i < MAX_METEOROS; i++) {
        meteoros[i].ativo = 0;
        meteoros[i].nivel = 0;
        meteoros[i].vida_atual = 0;
    }
}

void configurar_meteoro_por_nivel(Meteoro *meteoro) {
    switch (meteoro->nivel) {
        case 1:
            meteoro->vida_maxima = 1;
            meteoro->dano_chao = DANO_CHAO_NIVEL_1;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO; 
            break;
        case 2:
            meteoro->vida_maxima = 2;
            meteoro->dano_chao = DANO_CHAO_NIVEL_2;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO * 1.2;
            break;
        case 3:
            meteoro->vida_maxima = 3;
            meteoro->dano_chao = DANO_CHAO_NIVEL_3;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO * 1.5;
            break;
        default:
            configurar_meteoro_por_nivel(meteoro);
            break;
    }
    meteoro->vida_atual = meteoro->vida_maxima;
}

void gerar_novo_meteoro(Meteoro meteoros[], float dificuldade_fator) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (!meteoros[i].ativo) {
            int chance = rand() % 100;
            if (dificuldade_fator < 1.5) {
                meteoros[i].nivel = (chance < 70) ? 1 : 2; 
            } else {
                meteoros[i].nivel = (chance < 50) ? 2 : 3; 
            }

            configurar_meteoro_por_nivel(&meteoros[i]);

            meteoros[i].velocidade_y_base = meteoros[i].velocidade_y;
            meteoros[i].velocidade_y *= dificuldade_fator;
            meteoros[i].ativo = 1;

            int margem = (int)(LARGURA_TELA * MARGEM_HORIZONTAL_PERCENTUAL);
            int largura_segura = LARGURA_TELA - (2 * margem) - METEORO_LARGURA;

            meteoros[i].x = margem + (rand() % largura_segura);
            meteoros[i].y = -(rand() % 200) - METEORO_ALTURA;
            
            return;
        }
    }
}

void atualizar_meteoros(Meteoro meteoros[]) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            meteoros[i].velocidade_y += GRAVIDADE; 

            if (meteoros[i].velocidade_y > meteoros[i].velocidade_y_base) {
                meteoros[i].velocidade_y = meteoros[i].velocidade_y_base;
            }
            
            meteoros[i].y += meteoros[i].velocidade_y;
        }
    }
}

int checar_e_tratar_chao(Meteoro meteoros[]) {
    int dano_total = 0;
    
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            if (meteoros[i].y + METEORO_ALTURA >= ALTURA_CHAO) {
                dano_total += meteoros[i].dano_chao;
                meteoros[i].ativo = 0;
            }
        }
    }
    return dano_total;
}

int checar_colisao_com_cabeca(Dinossauro *dino, Meteoro meteoros[]) {
    const float ALTURA_CABECA_COLISAO = DINOSSAURO_ALTURA * 0.05;
    float dx1 = dino->x + DINOSSAURO_LARGURA * 0.1;
    float dx2 = dino->x + DINOSSAURO_LARGURA * 0.9;
    float dy1 = dino->y;
    float dy2 = dino->y + ALTURA_CABECA_COLISAO;

    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            float mx1 = meteoros[i].x;
            float my1 = meteoros[i].y;
            float mx2 = meteoros[i].x + METEORO_LARGURA;
            float my2 = meteoros[i].y + METEORO_ALTURA;

            if (dx1 < mx2 && dx2 > mx1 && dy1 < my2 && dy2 > my1) {
                dino->dy = FORCA_IMPACTO_DESCENDENTE;
                dino->no_chao = 0;
                return i;
            }
        }
    }
    return -1;
}

void desenhar_meteoros(Meteoro meteoros[]) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            ALLEGRO_COLOR cor_meteoro = al_map_rgb(150, 150, 150);
            
            // Mude a cor/visual conforme o nível
            if (meteoros[i].nivel == 2) cor_meteoro = al_map_rgb(200, 100, 0);
            if (meteoros[i].nivel == 3) cor_meteoro = al_map_rgb(200, 0, 0);

            al_draw_filled_rectangle(meteoros[i].x, meteoros[i].y, meteoros[i].x + METEORO_LARGURA, meteoros[i].y + METEORO_ALTURA, cor_meteoro);
        }
    }
}