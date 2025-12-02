// ===================================
// ARQUIVO: dados.h
// ===================================
#ifndef DADOS_H
#define DADOS_H

#include <allegro5/allegro.h>
#include "jogo.h" 

// --- DADOS GLOBAIS DE ESTADO E RECURSOS (extern) ---
extern Jogador jogador_principal;
extern JOGO_ESTADO estado_atual;
extern ALLEGRO_FONT *font_padrao;
extern ALLEGRO_DISPLAY *display; 
extern ALLEGRO_EVENT_QUEUE *event_queue;

extern Arma todas_as_armas[MAX_ARMAS];
extern Gema todas_as_gemas[MAX_GEMAS];
extern Casa tabuleiro[TAMANHO_TABULEIRO];
extern Objetivo objetivos[MAX_OBJETIVOS];

// --- CONSTANTES DE I/O ---
#define NOME_ARQUIVO_SCORES "highscores.dat"
#define MAX_HIGH_SCORES 5
#define NUM_SLOTS 3 

#endif // DADOS_H