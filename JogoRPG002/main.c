// ===================================
// ARQUIVO: main.c
// ===================================

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "dados.h" // Inclui todas as structs e dados globais

// Definição das variáveis globais (externas no dados.h, definidas em logica.c)
extern Jogador jogador_principal;
extern JOGO_ESTADO estado_atual;
extern ALLEGRO_FONT *font_padrao;
extern ALLEGRO_DISPLAY *display; 
extern ALLEGRO_EVENT_QUEUE *event_queue;


int main() {
    // --- 1. INICIALIZAÇÃO OBRIGATÓRIA ---
    if (!al_init() || !al_init_font_addon() || !al_init_ttf_addon() || !al_install_keyboard() || !al_init_primitives_addon()) {
        fprintf(stderr, "Falha na inicializacao do Allegro.\n"); return -1;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    event_queue = al_create_event_queue();

    font_padrao = al_load_font("arial.ttf", 16, 0); 
    if (!font_padrao) font_padrao = al_create_builtin_font();

    // --- INICIALIZAÇÃO DE VARIÁVEIS DO JOGADOR ---
    jogador_principal.vida = 100;
    jogador_principal.ataque_base = 20;
    jogador_principal.poder_ganho_batalha = 0;
    jogador_principal.moedas = 0; 
    jogador_principal.total_gemas = 0;
    jogador_principal.score = 0;
    jogador_principal.armas_coletadas = 0;
    jogador_principal.num_armas_inventario = 0;
    jogador_principal.num_gemas_inventario = 0;
    jogador_principal.posicao_atual = 0;
    jogador_principal.id_arma_equipada = 0; 
    jogador_principal.poder_gema_permanente = 0; 

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    bool running = true;

    // --- 2. LOOP PRINCIPAL ---
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            
            if (estado_atual == ESTADO_MENU) {
                if (event.keyboard.keycode == ALLEGRO_KEY_1) estado_atual = ESTADO_EXPLORACAO;
                if (event.keyboard.keycode == ALLEGRO_KEY_4) running = false;
            } 
            
            else if (estado_atual == ESTADO_EXPLORACAO) {
                if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    if (tabuleiro[jogador_principal.posicao_atual].tipo == CASA_BATALHA) {
                         if (calcular_poder_total(&jogador_principal) > tabuleiro[jogador_principal.posicao_atual].valor_evento) {
                            avancar_passo(); 
                         }
                    } else {
                         avancar_passo(); 
                    }
                }
            }
            
            else if (estado_atual == ESTADO_OPCAO_GEMA) {
                if (event.keyboard.keycode == ALLEGRO_KEY_G) {
                    if (jogador_principal.num_gemas_inventario > 0) {
                        int id_gema = jogador_principal.gemas_inventario[jogador_principal.num_gemas_inventario - 1].id;
                        int poder_da_gema = todas_as_gemas[id_gema -1].poder_extra;
                        
                        jogador_principal.poder_gema_permanente += poder_da_gema;
                        jogador_principal.num_gemas_inventario--;
                        
                        estado_atual = ESTADO_EXPLORACAO;
                        processar_evento_casa(); 
                    }
                } else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    estado_atual = ESTADO_EXPLORACAO; 
                }
            }
        }

        // --- 3. RENDERIZAÇÃO ---
        al_clear_to_color(al_map_rgb(10, 10, 50));

        if (estado_atual == ESTADO_MENU) {
            desenha_menu(font_padrao);
        } else if (estado_atual == ESTADO_EXPLORACAO || estado_atual == ESTADO_OPCAO_GEMA) {
            desenha_tabuleiro(font_padrao); 
            al_draw_textf(font_padrao, al_map_rgb(255, 255, 255), 10, 10, 0, "CASA %d", jogador_principal.posicao_atual + 1);
            al_draw_text(font_padrao, al_map_rgb(255, 255, 255), 10, 40, 0, tabuleiro[jogador_principal.posicao_atual].descricao);
            
            if (estado_atual == ESTADO_OPCAO_GEMA) {
                desenha_opcoes_gema(font_padrao);
            } else {
                al_draw_text(font_padrao, al_map_rgb(0, 255, 255), 10, 550, 0, "Pressione ESPACO para AVANCAR.");
            }
            desenha_hud(font_padrao, &jogador_principal);
        }

        al_flip_display();
    }

    // --- 4. DESTRUIÇÃO ---
    al_destroy_font(font_padrao);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    
    return 0;
}