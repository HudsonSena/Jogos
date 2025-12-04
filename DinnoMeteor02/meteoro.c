// meteoro.c

#include "meteoro.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h> 

// Incluindo as variáveis globais de tela
#include "config.h" 

void inicializar_meteoros(Meteoro meteoros[]) {
    // Inicializa o gerador de números aleatórios (para a função rand() na geração)
    srand(time(NULL)); 
    
    // Marca todos os meteoros como inativos (essencial!)
    for (int i = 0; i < MAX_METEOROS; i++) {
        meteoros[i].ativo = 0;
        // Zera outras propriedades para segurança
        meteoros[i].nivel = 0;
        meteoros[i].vida_atual = 0;
    }
}

// Função auxiliar para definir propriedades baseadas no nível
void configurar_meteoro_por_nivel(Meteoro *meteoro) {
    switch (meteoro->nivel) {
        case 1:
            // Meteoro Nível 1: 2 cabeceios para destruir, 25% dano no chão
            meteoro->vida_maxima = 1;
            meteoro->dano_chao = DANO_CHAO_NIVEL_1;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO; 
            break;
        case 2:
            // Meteoro Nível 2: 3 cabeceios para destruir, 50% dano no chão
            meteoro->vida_maxima = 2;
            meteoro->dano_chao = DANO_CHAO_NIVEL_2;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO * 1.2; // 20% mais rápido
            break;
        case 3:
            // Meteoro Nível 3: 4 cabeceios para destruir (sugestão), 75% dano no chão
            meteoro->vida_maxima = 3;
            meteoro->dano_chao = DANO_CHAO_NIVEL_3;
            meteoro->velocidade_y = VELOCIDADE_INICIAL_JOGO * 1.5; // 50% mais rápido
            break;
        default:
            // Fallback para Nível 1
            configurar_meteoro_por_nivel(meteoro);
            break;
    }
    meteoro->vida_atual = meteoro->vida_maxima;
}

// -----------------------------------------------------------------------------
// GERAÇÃO E ATUALIZAÇÃO
// -----------------------------------------------------------------------------

void gerar_novo_meteoro(Meteoro meteoros[], float dificuldade_fator) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (!meteoros[i].ativo) {
            
            // 1. Escolhe o Nível (Exemplo: aumenta chance de nível alto com a dificuldade)
            int chance = rand() % 100;
            if (dificuldade_fator < 1.5) { // Dificuldade baixa/média
                meteoros[i].nivel = (chance < 70) ? 1 : 2; 
            } else { // Dificuldade alta
                meteoros[i].nivel = (chance < 50) ? 2 : 3; 
            }
            
            // 2. Configura as propriedades baseadas no nível
            configurar_meteoro_por_nivel(&meteoros[i]);
            
            // 3. Posição e Ativação
            meteoros[i].ativo = 1;

            int margem = (int)(LARGURA_TELA * MARGEM_HORIZONTAL_PERCENTUAL);
            // Largura segura é a largura total menos 2x a margem e o tamanho do meteoro.
            int largura_segura = LARGURA_TELA - (2 * margem) - METEORO_LARGURA;

            // Posição X: Aleatória, caindo em linhas diferentes
            meteoros[i].x = margem + (rand() % largura_segura); 
            // Posição Y: Começa no topo (fora da tela)
            meteoros[i].y = -(rand() % 200) - METEORO_ALTURA;
            
            // Aplica o fator de dificuldade à velocidade Y baseada no nível
            meteoros[i].velocidade_y *= dificuldade_fator;

            meteoros[i].velocidade_y_base = meteoros[i].velocidade_y;
            
            return;
        }
    }
}

void atualizar_meteoros(Meteoro meteoros[]) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            meteoros[i].velocidade_y += GRAVIDADE; 
            
            // 2. NOVO: Checa se a velocidade é maior que a velocidade base.
            // Se for, redefine para a velocidade base para evitar aceleração contínua.
            if (meteoros[i].velocidade_y > meteoros[i].velocidade_y_base) {
                meteoros[i].velocidade_y = meteoros[i].velocidade_y_base;
            }
            
            // 3. Aplica o movimento
            meteoros[i].y += meteoros[i].velocidade_y;
        }
    }
}

// -----------------------------------------------------------------------------
// CHECAGEM DE CHÃO E COLISÃO
// -----------------------------------------------------------------------------

// Retorna o dano total se algum meteoro cair no chão.
int checar_e_tratar_chao(Meteoro meteoros[]) {
    int dano_total = 0;
    
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            // Se o meteoro atingir a linha do chão
            if (meteoros[i].y + METEORO_ALTURA >= ALTURA_CHAO) {
                dano_total += meteoros[i].dano_chao;
                meteoros[i].ativo = 0; // Destrói o meteoro
            }
        }
    }
    return dano_total;
}

// Retorna o índice do meteoro atingido pela cabeça do dinossauro.
int checar_colisao_com_cabeca(Dinossauro *dino, Meteoro meteoros[]) {
    // Altura da área de colisão da cabeça (Ex: 5% da altura total do dinossauro)
    const float ALTURA_CABECA_COLISAO = DINOSSAURO_ALTURA * 0.05;
    // Definimos a área de "cabeceio" como a parte superior do dinossauro.
    float dx1 = dino->x + DINOSSAURO_LARGURA * 0.1; // 10% de margem
    float dx2 = dino->x + DINOSSAURO_LARGURA * 0.9; 
    // NOVO: Área de Cabeceio (apenas os 10% superiores do Dinossauro)
    float dy1 = dino->y;                            // Topo do dinossauro
    float dy2 = dino->y + ALTURA_CABECA_COLISAO;        // Base da área de cabeceio

    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            float mx1 = meteoros[i].x;
            float my1 = meteoros[i].y;
            float mx2 = meteoros[i].x + METEORO_LARGURA;
            float my2 = meteoros[i].y + METEORO_ALTURA;

            // Checagem de Colisão AABB (área de cabeceio vs. meteoro)
            if (dx1 < mx2 && dx2 > mx1 && dy1 < my2 && dy2 > my1) {
                dino->dy = FORCA_IMPACTO_DESCENDENTE;
                dino->no_chao = 0;
                return i; // Retorna o índice do meteoro atingido
            }
        }
    }
    return -1; // Nenhum meteoro atingido
}

void desenhar_meteoros(Meteoro meteoros[]) {
    // Desenho é importante para identificar o nível/vida
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            ALLEGRO_COLOR cor_meteoro = al_map_rgb(150, 150, 150); // Cinza base
            
            // Mude a cor/visual conforme o nível
            if (meteoros[i].nivel == 2) cor_meteoro = al_map_rgb(200, 100, 0); // Laranja/Marrom
            if (meteoros[i].nivel == 3) cor_meteoro = al_map_rgb(200, 0, 0);   // Vermelho Escuro
            
            // Desenha o corpo principal do meteoro
            al_draw_filled_rectangle(meteoros[i].x, meteoros[i].y,
                                     meteoros[i].x + METEORO_LARGURA, meteoros[i].y + METEORO_ALTURA,
                                     cor_meteoro);
                                     
            // Desenha a "Vida" (opcional: uma barra ou texto)
            // al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), meteoros[i].x + METEORO_LARGURA / 2, meteoros[i].y + 5, ALLEGRO_ALIGN_CENTER, "%d", meteoros[i].vida_atual);
        }
    }
}