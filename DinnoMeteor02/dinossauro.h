#ifndef DINOSSAURO_H
#define DINOSSAURO_H

#include <allegro5/allegro.h>

// --- CONSTANTES ---
#define GRAVIDADE 0.8
#define ALTURA_PULO 22.0
#define DINOSSAURO_LARGURA 60
#define DINOSSAURO_ALTURA 80
#define VELOCIDADE_X 8.0
#define FORCA_IMPACTO_DESCENDENTE 10.0

// Posição vertical do Chão (base do dinossauro)
#define ALTURA_CHAO (ALTURA_TELA - 100) // Assumindo ALTURA_TELA = 600, Chão = 500

// --- STRUCT DO DINOSSAURO ---
typedef struct {
    float x;
    float y;
    float dy;             // Velocidade vertical (para pulo e queda)
    float dx;             // Velocidade horizontal (movimento)
    int vida;             // 0 a 100
    int poder_bonus;      // 0, 1 (Amarelo), 2 (Vermelho)
    int em_pulso;         // Flag booleana (1 se estiver pulando, 0 caso contrário)
    int no_chao;          // Flag booleana (1 se estiver no chão)
} Dinossauro;

// --- ASSINATURAS DE FUNÇÕES ---
void dinossauro_ejetar_para_chao(Dinossauro *dino);

// Inicializa o dinossauro na posição inicial (no chão)
void dinossauro_inicializar(Dinossauro *dino, float x_inicial);

// Lógica de pulo (deve ser chamada quando a tecla de pulo é pressionada)
void dinossauro_pular(Dinossauro *dino);

// Atualiza a posição do dinossauro e aplica a gravidade
void dinossauro_atualizar(Dinossauro *dino);

// Desenha o dinossauro (por enquanto, um retângulo simples)
void dinossauro_desenhar(Dinossauro *dino);

// Desenha o chão
void desenhar_chao();

#endif // DINOSSAURO_H