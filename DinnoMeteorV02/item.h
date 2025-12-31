#ifndef ITEM_H
#define ITEM_H

#include <allegro5/allegro.h>
#include "dinossauro.h"
#include "config.h" 

#define MAX_ITENS 4
#define ITEM_LARGURA 30
#define ITEM_ALTURA 30
#define VELOCIDADE_QUEDA_ITEM 3.0
#define DANO_VIDA_RECUPERADA 25
#define DURACAO_BONUS_SEGUNDOS 15.0
#define TEMPO_ENTRE_GERACAO_ITENS 30.0

typedef enum {
    ITEM_NENHUM = 0,
    ITEM_VIDA,
    ITEM_BONUS_1,
    ITEM_BONUS_2
} TipoItem;

typedef struct {
    float x;
    float y;
    float velocidade_y;
    TipoItem tipo;
    int ativo;
} Item;

void inicializar_itens(Item itens[]);
void gerar_item(Item itens[], TipoItem tipo);
void atualizar_itens(Item itens[]);
void desenhar_itens(Item itens[]);
void checar_coleta_item(Dinossauro *dino, Item itens[]);

#endif