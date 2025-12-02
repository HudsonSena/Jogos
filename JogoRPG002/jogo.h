// ===================================
// ARQUIVO: jogo.h
// ===================================
#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>
#include <stdio.h> 

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// --- CONSTANTES DE JOGO E TELA ---
#define MAX_VERTENTES 1
#define TAMANHO_TABULEIRO 14
#define SCREEN_W 800
#define SCREEN_H 600
#define HUD_X_START 650
#define MAX_OBJETIVOS 3
#define TAM_NICKNAME 50
#define MAX_ARMAS 5 
#define MAX_GEMAS 5 
#define MAX_VIDA 200

// --- ENUMS DE ESTADO E TIPOS ---
typedef enum {
    ESTADO_MENU, ESTADO_EXPLORACAO, ESTADO_BATALHA, ESTADO_OPCAO_GEMA,
    ESTADO_HIGH_SCORES, ESTADO_SAIR
} JOGO_ESTADO;

typedef enum { CASA_VAZIA, CASA_BATALHA, CASA_ARMA, CASA_GEMA, CASA_CHEGADA } TipoCasa;

// --- ESTRUTURAS DE DADOS ---
typedef struct { int id; char nome[50]; int bonus_ataque; } Arma;
typedef struct { int id; int poder_extra; } Gema;
typedef struct {
    TipoCasa tipo; char descricao[100]; int valor_evento; 
    int id_proximas_casas[MAX_VERTENTES]; int num_vertentes; 
    int id_arma_recompensa; int id_gema_recompensa; 
} Casa;
typedef struct { char nome[50]; int score; int data_salva; } HighScore;
typedef struct { char nome_jogador[TAM_NICKNAME]; int score; int vida_atual; int nivel_atual; int x_posicao; int y_posicao; } JogoSalvo;
typedef struct { char nome[100]; int valor_necessario; int valor_atual; bool completado; } Objetivo;

typedef struct {
    char nome[50]; int vida; int ataque_base; int poder_ganho_batalha;
    int moedas; int score; int posicao_atual;
    Arma armas_inventario[MAX_ARMAS]; int num_armas_inventario; int id_arma_equipada; 
    Gema gemas_inventario[MAX_GEMAS]; int num_gemas_inventario; int poder_gema_permanente;
    int total_gemas; int armas_coletadas; 
} Jogador;


// --- PROTÓTIPOS DE FUNÇÕES (Declaradas em logica.c e render.c) ---
void desenha_tabuleiro(ALLEGRO_FONT *font);
void desenha_menu(ALLEGRO_FONT *font);
void desenha_hud(ALLEGRO_FONT *font, Jogador *j);
void desenha_high_scores(ALLEGRO_FONT *font);
void desenha_opcoes_gema(ALLEGRO_FONT *font);
void desenha_menu_carregar_jogo();

void atualizar_objetivos();
void processar_evento_casa();
int calcular_poder_total(const Jogador *j);
void avancar_passo(); 

// Funções de I/O de arquivo
void carregar_high_scores(HighScore scores[]);
void salvar_jogo(const JogoSalvo *jogo, int slot_index);
int carregar_jogo(JogoSalvo *jogo, int slot_index);

#endif // JOGO_H