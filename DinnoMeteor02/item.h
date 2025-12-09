#ifndef ITEM_H
#define ITEM_H

#include <allegro5/allegro.h>
#include "dinossauro.h"
#include "config.h" 

// --- CONSTANTES ---
#define MAX_ITENS 4
#define ITEM_LARGURA 30
#define ITEM_ALTURA 30
#define VELOCIDADE_QUEDA_ITEM 3.0
#define DANO_VIDA_RECUPERADA 25          // +25% de vida
#define DURACAO_BONUS_SEGUNDOS 15.0     // 15 segundos de bônus
#define TEMPO_ENTRE_GERACAO_ITENS 30.0  // 1 minuto (60 segundos)

// --- TIPOS DE ITEM ---
typedef enum {
    ITEM_NENHUM = 0,
    ITEM_VIDA,       // Cura (+25% de vida)
    ITEM_BONUS_1,    // Bônus Amarelo (poder 1)
    ITEM_BONUS_2     // Bônus Vermelho (poder 2)
} TipoItem;

// --- STRUCT DO ITEM ---
typedef struct {
    float x;
    float y;
    float velocidade_y;
    TipoItem tipo;
    int ativo;
} Item;

// --- ASSINATURAS DE FUNÇÕES ---

void inicializar_itens(Item itens[]);
void gerar_item(Item itens[], TipoItem tipo);
void atualizar_itens(Item itens[]);
void desenhar_itens(Item itens[]);
void checar_coleta_item(Dinossauro *dino, Item itens[]);

#endif // ITEM_H