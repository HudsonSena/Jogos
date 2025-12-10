#include "item.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h> 
#include "config.h"
#include <stdio.h>

void inicializar_itens(Item itens[]) {
    for (int i = 0; i < MAX_ITENS; i++) {
        itens[i].ativo = 0;
    }
}

void gerar_item(Item itens[], TipoItem tipo) {
    for (int i = 0; i < MAX_ITENS; i++) {
        if (!itens[i].ativo) {
            itens[i].ativo = 1;
            itens[i].tipo = tipo;
            itens[i].velocidade_y = VELOCIDADE_QUEDA_ITEM;

            int margem = (int)(LARGURA_TELA * 0.10);
            int largura_segura = LARGURA_TELA - (2 * margem) - ITEM_LARGURA;
            itens[i].x = margem + (rand() % largura_segura);
            
            itens[i].y = -(rand() % 100) - ITEM_ALTURA;
            return;
        }
    }
}

void atualizar_itens(Item itens[]) {
    for (int i = 0; i < MAX_ITENS; i++) {
        if (itens[i].ativo) {
            itens[i].y += itens[i].velocidade_y;
            
            if (itens[i].y + ITEM_ALTURA >= ALTURA_CHAO) {
                itens[i].ativo = 0;
            }
        }
    }
}

void desenhar_itens(Item itens[]) {
    for (int i = 0; i < MAX_ITENS; i++) {
        if (itens[i].ativo) {
            ALLEGRO_COLOR cor;
            switch (itens[i].tipo) {
                case ITEM_VIDA:
                    cor = al_map_rgb(0, 255, 0);
                    break;
                case ITEM_BONUS_1:
                    cor = al_map_rgb(255, 255, 0);
                    break;
                case ITEM_BONUS_2:
                    cor = al_map_rgb(0, 0, 255);
                    break;
                default:
                    cor = al_map_rgb(255, 255, 255);
                    break;
            }
            al_draw_filled_rectangle(itens[i].x, itens[i].y, itens[i].x + ITEM_LARGURA, itens[i].y + ITEM_ALTURA, cor);
        }
    }
}

void checar_coleta_item(Dinossauro *dino, Item itens[]) {
    float dx1 = dino->x;
    float dy1 = dino->y;
    float dx2 = dino->x + DINOSSAURO_LARGURA;
    float dy2 = dino->y + DINOSSAURO_ALTURA;

    for (int i = 0; i < MAX_ITENS; i++) {
        if (itens[i].ativo) {
            float ix1 = itens[i].x;
            float iy1 = itens[i].y;
            float ix2 = itens[i].x + ITEM_LARGURA;
            float iy2 = itens[i].y + ITEM_ALTURA;

            if (dx1 < ix2 && dx2 > ix1 && dy1 < iy2 && dy2 > iy1) {
                switch (itens[i].tipo) {
                    case ITEM_VIDA:
                        dino->vida += DANO_VIDA_RECUPERADA;
                        if (dino->vida > 100) dino->vida = 100;
                        printf("Item coletado: Vida (+%d)\n", DANO_VIDA_RECUPERADA);
                        break;
                        
                    case ITEM_BONUS_1:
                        dino->poder_bonus = 1;
                        dino->tempo_bonus = DURACAO_BONUS_SEGUNDOS;
                        printf("Item coletado: Bônus Amarelo!\n");
                        break;
                        
                    case ITEM_BONUS_2:
                        dino->poder_bonus = 2;
                        dino->tempo_bonus = DURACAO_BONUS_SEGUNDOS;
                        printf("Item coletado: Bônus Vermelho!\n");
                        break;
                    default:
                        break;
                }
                
                itens[i].ativo = 0; 
            }
        }
    }
}