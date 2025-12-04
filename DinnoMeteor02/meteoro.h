#ifndef METEORO_H
#define METEORO_H

#include <allegro5/allegro.h>
#include "dinossauro.h" 
#include "config.h"      // Para LARGURA_TELA e ALTURA_CHAO

// --- CONSTANTES ---
#define METEORO_BOUNCE_DISPLACEMENT -150.0 // NOVO: Deslocamento vertical para cima (negativo)
#define MARGEM_HORIZONTAL_PERCENTUAL 0.20 // NOVO: 20% de margem de cada lado
#define MAX_METEOROS 3
#define METEORO_LARGURA 30
#define METEORO_ALTURA 30
#define VELOCIDADE_INICIAL_JOGO 2.0 // Velocidade base dos meteoros
#define DANO_CHAO_NIVEL_1 10
#define DANO_CHAO_NIVEL_2 25
#define DANO_CHAO_NIVEL_3 40 // Sugestão
#define FORCA_RECUO_METEORO -10.0

// --- STRUCT DO METEORO (ATUALIZADA) ---
typedef struct {
    float x;
    float y;
    float velocidade_y;   // Meteoros de nível mais alto podem cair mais rápido
    float velocidade_y_base;
    int nivel;            // 1, 2, ou 3
    int vida_atual;       // Cabeceios restantes para destruir
    int vida_maxima;      // Cabeceios necessários originalmente (para o dinossauro normal)
    int dano_chao;        // Dano em % se cair no chão
    int ativo;
} Meteoro;

// --- ASSINATURAS DE FUNÇÕES ---

void inicializar_meteoros(Meteoro meteoros[]);
// Gera novo meteoro, escolhendo o nível baseado na dificuldade ou sorte
void gerar_novo_meteoro(Meteoro meteoros[], float dificuldade_fator); 
void atualizar_meteoros(Meteoro meteoros[]);
void desenhar_meteoros(Meteoro meteoros[]);

// Checa a colisão, retorna o dano ao dinossauro se cair no chão.
// O dano ao meteoro/destruição será tratado na main/loop de colisão.
int checar_e_tratar_chao(Meteoro meteoros[]);

// Checa a colisão, retorna o índice do meteoro atingido (ou -1 se nenhum)
int checar_colisao_com_cabeca(Dinossauro *dino, Meteoro meteoros[]);

#endif // METEORO_H