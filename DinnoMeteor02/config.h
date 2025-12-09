// config.h

#ifndef CONFIG_H
#define CONFIG_H

#define LARGURA_TELA 800
#define ALTURA_TELA 600

// Posição vertical do Chão (base do dinossauro)
// ALTURA_CHAO deve ser calculada aqui ou baseada nessas constantes
#define ALTURA_CHAO (ALTURA_TELA - 100) 

#define PONTOS_POR_FASE 300.0   // A cada 200 pontos, a dificuldade aumenta
#define FATOR_ACELERACAO 0.30   // Aumenta a velocidade dos meteoros em 30% a cada fase
#define TEMPO_TRANSICAO 3.0

#endif // CONFIG_H