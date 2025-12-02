// ===================================
// ARQUIVO: render.c
// ===================================

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "dados.h"

// Funções de Desenho

void desenha_tabuleiro(ALLEGRO_FONT *font) {
    int i;
    int start_x = 50; 
    int start_y = 100; 
    int cell_size = 80; 
    int spacing = 20; 
    int cells_per_row = 5;
    ALLEGRO_COLOR cor_jogador = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR cor_base = al_map_rgb(90, 90, 90);
    ALLEGRO_COLOR cor_texto = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR cor_principal = al_map_rgb(200, 200, 255);
    ALLEGRO_COLOR cor_branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_dourado = al_map_rgb(255, 205, 55);
    ALLEGRO_COLOR cor_azul = al_map_rgb(95, 95, 255);
    ALLEGRO_COLOR cor_vermelho = al_map_rgb(255, 95, 95);
    ALLEGRO_COLOR cor_vermelho2 = al_map_rgb(150, 50, 50);
    ALLEGRO_COLOR cor_cinza = al_map_rgb(90, 90, 90);
    ALLEGRO_COLOR cor_verde = al_map_rgb(55, 205, 55);          

    for (i = 0; i < TAMANHO_TABULEIRO; i++) {
        int x = start_x + (i % cells_per_row) * (cell_size + spacing); int y = start_y + (i / cells_per_row) * (cell_size + spacing);
        ALLEGRO_COLOR cor = cor_base;
        if (i == jogador_principal.posicao_atual) cor = cor_jogador; 
        else if (tabuleiro[i].tipo == CASA_BATALHA) cor = cor_vermelho2; 
        else if (tabuleiro[i].tipo == CASA_GEMA) cor = cor_azul;
        else if (tabuleiro[i].tipo == CASA_CHEGADA) cor = cor_verde;
        
        al_draw_filled_rectangle(x, y, x + cell_size, y + cell_size, cor);
        al_draw_rectangle(x, y, x + cell_size, y + cell_size, al_map_rgb(255, 255, 255), 1);
        
        al_draw_textf(font, cor_texto, x + cell_size / 2, y + cell_size / 2 - 10, ALLEGRO_ALIGN_CENTER, "CASA %d", i + 1);
        al_draw_textf(font, cor_texto, x + cell_size / 2, y + cell_size / 2 + 10, ALLEGRO_ALIGN_CENTER, 
                      (i == jogador_principal.posicao_atual) ? "JOGADOR" : (tabuleiro[i].tipo == CASA_BATALHA ? "BATALHA" : "OUTRO"));
    }
}

void desenha_menu(ALLEGRO_FONT *font) {
    ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR amarelo = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR cinza = al_map_rgb(150, 150, 150);

    al_draw_filled_rectangle(150, 100, 650, 500, al_map_rgb(50, 50, 50));
    al_draw_rectangle(150, 100, 650, 500, branco, 2);
    al_draw_text(font, amarelo, 400, 150, ALLEGRO_ALIGN_CENTER, "AVENTURA DE TABULEIRO");
    al_draw_text(font, branco, 400, 250, ALLEGRO_ALIGN_CENTER, "1. INICIAR NOVO JOGO");
    al_draw_text(font, branco, 250, 300, ALLEGRO_ALIGN_LEFT, "2. CARREGAR JOGO SALVO");
    al_draw_text(font, branco, 250, 350, ALLEGRO_ALIGN_LEFT, "3. MELHORES SCORES");
    al_draw_text(font, branco, 250, 400, ALLEGRO_ALIGN_LEFT, "4. SAIR");
    al_draw_text(font, cinza, 400, 450, ALLEGRO_ALIGN_CENTER, "Escolha uma opcao (1-4)...");
}

void desenha_hud(ALLEGRO_FONT *font, Jogador *j) {
    int x_start = HUD_X_START; int y_pos = 10;
    ALLEGRO_COLOR cor_principal = al_map_rgb(200, 200, 255);
    ALLEGRO_COLOR cor_branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_dourado = al_map_rgb(255, 205, 55);
    ALLEGRO_COLOR cor_azul = al_map_rgb(95, 95, 255);
    ALLEGRO_COLOR cor_vermelho = al_map_rgb(255, 95, 95);
    ALLEGRO_COLOR cor_vermelho2 = al_map_rgb(255, 50, 50);
    ALLEGRO_COLOR cor_cinza = al_map_rgb(90, 90, 90);
    ALLEGRO_COLOR cor_verde = al_map_rgb(55, 205, 55);
      
    int total_ataque = calcular_poder_total(j);
    
    al_draw_filled_rectangle(x_start - 5, 0, SCREEN_W, SCREEN_H, al_map_rgb(30, 30, 30));
    al_draw_line(x_start - 5, 0, x_start - 5, SCREEN_H, cor_branco, 3);

    al_draw_text(font, cor_branco, x_start, y_pos, 0, "--- STATUS ---");
    y_pos += 40;

    al_draw_textf(font, cor_verde, x_start, y_pos, 0, "HP: %d", j->vida);
    y_pos += 30;
    al_draw_textf(font, cor_principal, x_start, y_pos, 0, "Poder Total: %d", total_ataque);
    y_pos += 30;
    al_draw_textf(font, cor_dourado, x_start, y_pos, 0, "Moedas: %d", j->moedas);
    y_pos += 30;
    al_draw_textf(font, cor_branco, x_start, y_pos, 0, "Gemas (Itens): %d", j->num_gemas_inventario);
    y_pos += 40;
    al_draw_textf(font, cor_branco, x_start, y_pos, 0, "SCORE: %d", j->score);
    y_pos += 40;
    
    al_draw_text(font, cor_principal, x_start, y_pos, 0, "Arma Equip.:");
    y_pos += 30;
    al_draw_text(font, cor_branco, x_start, y_pos, 0, (j->id_arma_equipada > 0) ? todas_as_armas[j->id_arma_equipada - 1].nome : "Nenhuma");
    // desenha_objetivos deve ser chamada aqui.
}

void desenha_opcoes_gema(ALLEGRO_FONT *font) {
    al_draw_filled_rectangle(10, 200, 640, 350, al_map_rgb(20, 20, 70));
    al_draw_rectangle(10, 200, 640, 350, al_map_rgb(255, 255, 0), 2);

    al_draw_text(font, al_map_rgb(255, 255, 255), 325, 210, ALLEGRO_ALIGN_CENTER, "DERROTA! A batalha foi perdida.");
    al_draw_textf(font, al_map_rgb(0, 255, 255), 325, 250, ALLEGRO_ALIGN_CENTER, "Gemas disponiveis: %d", jogador_principal.num_gemas_inventario);

    if (jogador_principal.num_gemas_inventario > 0) {
        al_draw_text(font, al_map_rgb(0, 255, 0), 325, 280, ALLEGRO_ALIGN_CENTER, "Pressione [G] para CONSUMIR GEMA e tentar de novo!");
    }
    al_draw_text(font, al_map_rgb(255, 100, 100), 325, 310, ALLEGRO_ALIGN_CENTER, "Pressione [ESPACO] para tentar de novo sem gema.");
}

void desenha_menu_carregar_jogo() {
    // ... Implementação de carregamento e exibição de slots
}

void desenha_high_scores(ALLEGRO_FONT *font) {
    // ... Implementação de high scores ...
}