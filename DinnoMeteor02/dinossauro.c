#include "dinossauro.h"
#include "config.h"
#include <allegro5/allegro_primitives.h>

// -----------------------------------------------------------------------------
// INICIALIZAÇÃO E MOSTRAGEM
// -----------------------------------------------------------------------------

void dinossauro_inicializar(Dinossauro *dino, float x_inicial) {
    dino->x = x_inicial;
    dino->y = ALTURA_CHAO - DINOSSAURO_ALTURA; // Coloca o topo do dino no chão
    dino->dy = 0.0;
    dino->dx = 0.0;
    dino->vida = 100;
    dino->poder_bonus = 0;
    dino->tempo_bonus = 0.0;
    dino->em_pulso = 0;
    dino->no_chao = 1;
}

void dinossauro_desenhar(Dinossauro *dino) {
    ALLEGRO_COLOR cor_dino = al_map_rgb(0, 150, 0); // Verde padrão
    
    // Se estiver em modo bônus (futuramente será feito com frames/sprites)
    if (dino->poder_bonus == 1) { // Amarelo
        cor_dino = al_map_rgb(255, 255, 0);
    } else if (dino->poder_bonus == 2) { // Vermelho
        cor_dino = al_map_rgb(255, 0, 0);
    }

    // Desenha o retângulo do dinossauro
    al_draw_filled_rectangle(dino->x, dino->y, 
                             dino->x + DINOSSAURO_LARGURA, dino->y + DINOSSAURO_ALTURA, 
                             cor_dino);
}

void desenhar_chao() {
    ALLEGRO_COLOR cor_chao = al_map_rgb(100, 50, 0); // Marrom
    // Desenha uma linha grossa (ou retângulo preenchido) para o chão
    al_draw_filled_rectangle(0, ALTURA_CHAO, 
                             LARGURA_TELA, ALTURA_TELA, 
                             cor_chao);
}

// -----------------------------------------------------------------------------
// LÓGICA DE MOVIMENTO
// -----------------------------------------------------------------------------

void dinossauro_pular(Dinossauro *dino) {
    if (dino->no_chao) {
        dino->dy = -ALTURA_PULO; // Velocidade negativa para subir
        dino->no_chao = 0;
    }
}

void dinossauro_atualizar(Dinossauro *dino) {
    // Aplica a velocidade vertical
    dino->y += dino->dy;
    // Aplica a velocidade horizontal
    dino->x += dino->dx;

    // Aplica a Gravidade
    // Se não estiver no chão, aumenta a velocidade de queda
    if (dino->y + DINOSSAURO_ALTURA < ALTURA_CHAO) {
        dino->dy += GRAVIDADE;
        dino->no_chao = 0;
    } 
    
    // Checa a Colisão com o Chão
    if (dino->y + DINOSSAURO_ALTURA >= ALTURA_CHAO) {
        dino->y = ALTURA_CHAO - DINOSSAURO_ALTURA; // Reposiciona no chão
        dino->dy = 0.0;
        dino->no_chao = 1;
    }

    // Limites da Tela (Bordas Horizontais)
    // Limite esquerdo
    if (dino->x < 0) {
        dino->x = 0;
        dino->dx = 0; // Para parar instantaneamente ao bater na borda
    }
    // Limite direito
    if (dino->x + DINOSSAURO_LARGURA > LARGURA_TELA) {
        dino->x = LARGURA_TELA - DINOSSAURO_LARGURA;
        dino->dx = 0; // Para parar instantaneamente ao bater na borda
    }
}