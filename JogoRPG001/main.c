// ===================================
// ARQUIVO: main.c
// ===================================

#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "jogo.h" 

// --- VARIÁVEIS DE ESTADO E GLOBAl ---
Jogador jogador_principal;
JOGO_ESTADO estado_atual = ESTADO_MENU;
ALLEGRO_FONT *font_padrao = NULL;
ALLEGRO_DISPLAY *display = NULL; 
ALLEGRO_EVENT_QUEUE *event_queue = NULL;

#define NOME_ARQUIVO_SCORES "highscores.dat"
#define MAX_HIGH_SCORES 5
#define NUM_SLOTS 3 

// Definições de Itens (Externas para referencia)
Arma todas_as_armas[MAX_ARMAS] = {
    {1, "Espada de Treino", 10}, {2, "Cajado Magico", 25}, {3, "Machado de Guerra", 40}, {4, "Lanca Lendaria", 60}, {5, "Espada Final", 100}
};
Gema todas_as_gemas[MAX_GEMAS] = {
    {1, 20}, {2, 30}, {3, 50}, {4, 80}, {5, 120}
};

// --- TABULEIRO LINEAR (14 CASAS) ---
Casa tabuleiro[TAMANHO_TABULEIRO] = {
    // ID: 0 (Casa 1)
    {CASA_VAZIA, "Inicio da Aventura.", 0, {1}, 1, 0, 0},
    // ID: 1 (Casa 2) - Batalha 1
    {CASA_BATALHA, "Goblin Fraco.", 15, {2}, 1, 0, 0},        
    // ID: 2 (Casa 3) - Batalha 2
    {CASA_BATALHA, "Orc Guerreiro.", 30, {3}, 1, 0, 0},       
    // ID: 3 (Casa 4) - Gema/Moeda
    {CASA_GEMA, "Encontrou 100 Moedas.", 100, {4}, 1, 0, 0},  
    // ID: 4 (Casa 5) - Arma 1
    {CASA_ARMA, "Encontrou Espada de Treino!", 0, {5}, 1, 1, 0}, 
    // ID: 5 (Casa 6) - Batalha 3
    {CASA_BATALHA, "Troll de Rocha.", 50, {6}, 1, 0, 0},      
    // ID: 6 (Casa 7) - Cura
    {CASA_VAZIA, "Descanso. Recupera 100 HP.", +100, {7}, 1, 0, 0},
    // ID: 7 (Casa 8) - Arma 2
    {CASA_ARMA, "Encontrou Cajado Magico!", 0, {8}, 1, 2, 0},    
    // ID: 8 (Casa 9) - Batalha 4
    {CASA_BATALHA, "Dragao Pequeno.", 90, {9}, 1, 0, 0},      
    // ID: 9 (Casa 10) - Arma 3
    {CASA_ARMA, "Encontrou Escudo!", 0, {10}, 1, 3, 0},  
    // ID: 10 (Casa 11) - Gema/Moeda
    {CASA_GEMA, "Jackpot! 250 Moedas.", 250, {11}, 1, 0, 0}, 
    // ID: 11 (Casa 12) - Batalha 5
    {CASA_BATALHA, "Chefe Final! Grande Demonio.", +120, {12}, 1, 0, 0}, 
    // ID: 12 (Casa 13) - Cura
    {CASA_VAZIA, "Ultimo Ponto de Descanço.", 0, {13}, 1, 0, 0},
    // ID: 13 (Casa 14) - Fim
    {CASA_CHEGADA, "VITORIA! Voce zerou o jogo.", 0, {-1}, 0, 0, 0} 
};

Objetivo objetivos[MAX_OBJETIVOS] = {
    {"Coletar 3 Armas", 3, 0, false},
    {"Acumular 3 Gemas", 3, 0, false},
    {"Chegar na Casa 14", 13, 0, false}
};


// --- IMPLEMENTAÇÃO DE FUNÇÕES DE LÓGICA E UTILIDADE ---

int calcular_poder_total(const Jogador *j) {
    int poder_arma = 0;
    if (j->id_arma_equipada > 0) {
        poder_arma = todas_as_armas[j->id_arma_equipada - 1].bonus_ataque;
    }
    return j->ataque_base + j->poder_ganho_batalha + poder_arma + j->poder_gema_permanente;
}

void atualizar_objetivos() {
    objetivos[0].valor_atual = jogador_principal.armas_coletadas;
    if (objetivos[0].valor_atual >= objetivos[0].valor_necessario) {
        objetivos[0].completado = true;
    }
    objetivos[2].valor_atual = jogador_principal.posicao_atual;
    if (objetivos[2].valor_atual >= objetivos[2].valor_necessario) {
        objetivos[2].completado = true;
    }
}

void avancar_passo() {
    int pos = jogador_principal.posicao_atual;
    const Casa *casa = &tabuleiro[pos];

    if (pos >= TAMANHO_TABULEIRO - 1) { 
        return; 
    }
    
    // Avanca linearmente para a proxima casa
    jogador_principal.posicao_atual = casa->id_proximas_casas[0];
    
    // Processa o evento da nova casa
    processar_evento_casa();
}

void processar_evento_casa() {
    int pos = jogador_principal.posicao_atual;
    if (pos >= TAMANHO_TABULEIRO) return; 
    const Casa *casa = &tabuleiro[pos];
    
    printf("\n--- CASA %d - EVENTO: %s ---\n", pos + 1, casa->descricao);


    switch (casa->tipo) {
        case CASA_BATALHA: {
            int poder_total = calcular_poder_total(&jogador_principal);
            int forca_inimigo = casa->valor_evento;
            int dano_sofrido = forca_inimigo / 3;

            if (estado_atual != ESTADO_OPCAO_GEMA) {
                 jogador_principal.vida -= dano_sofrido;
            }

            printf("Batalha! Seu Poder: %d. Inimigo Forca: %d. Dano sofrido: %d. HP: %d\n", 
                   poder_total, forca_inimigo, dano_sofrido, jogador_principal.vida);

            if (poder_total > forca_inimigo) {
                // VITÓRIA: Avanco no proximo loop
                printf("VITORIA! Ganha +5 de poder, 50 moedas e uma Gema Unica (se houver).\n");
                jogador_principal.poder_ganho_batalha += 5;
                jogador_principal.vida += 20;
                jogador_principal.moedas += 50;
                
                int id_proxima_gema = jogador_principal.num_gemas_inventario + 1;
                if (id_proxima_gema <= MAX_GEMAS) {
                    jogador_principal.gemas_inventario[jogador_principal.num_gemas_inventario] = todas_as_gemas[id_proxima_gema - 1];
                    jogador_principal.num_gemas_inventario++;
                }
                
            } else {
                // DERROTA: Fica na casa
                printf("DERROTA! Poder insuficiente. Consuma gema [G] ou tente novamente [ESPACO].\n");
                estado_atual = ESTADO_OPCAO_GEMA;
            }
            if (jogador_principal.vida <= 0) estado_atual = ESTADO_SAIR;
            break;
        }
        
        case CASA_GEMA:
            jogador_principal.moedas += casa->valor_evento;
            jogador_principal.total_gemas += casa->valor_evento; 
            printf("GEMA COLETADA! Ganhou %d moedas.\n", casa->valor_evento);
            jogador_principal.posicao_atual++; 
            break;
        
        case CASA_ARMA: {
            int id_nova_arma = casa->id_arma_recompensa;
            bool ja_possui = false;
            for (int i = 0; i < jogador_principal.num_armas_inventario; i++) {
                if (jogador_principal.armas_inventario[i].id == id_nova_arma) {
                    ja_possui = true;
                    break;
                }
            }
            
            if (!ja_possui) {
                Arma nova_arma = todas_as_armas[id_nova_arma - 1];
                jogador_principal.armas_inventario[jogador_principal.num_armas_inventario] = nova_arma;
                jogador_principal.num_armas_inventario++;
                jogador_principal.armas_coletadas++;
                
                if (jogador_principal.id_arma_equipada == 0 || nova_arma.bonus_ataque > todas_as_armas[jogador_principal.id_arma_equipada - 1].bonus_ataque) {
                    jogador_principal.id_arma_equipada = id_nova_arma;
                    printf("ARMA EQUIPADA: %s!\n", nova_arma.nome);
                }
            }
            jogador_principal.posicao_atual++; 
            break;
        }

        case CASA_VAZIA:
             printf("Casa vazia. Nada acontece.\n");
             jogador_principal.posicao_atual++;
             break;
            
        case CASA_CHEGADA:
            printf("--- VITORIA FINAL ---\n");
            estado_atual = ESTADO_SAIR;
            break;
    }

    atualizar_objetivos();
}

// --- IMPLEMENTAÇÕES DE FUNÇÕES DE RENDERIZAÇÃO E UTILIDADE (Omitidas por brevidade, mas necessárias) ---

void desenha_tabuleiro(ALLEGRO_FONT *font) {
    int i;
    int start_x = 50; int start_y = 100; int cell_size = 80; int spacing = 20; int cells_per_row = 5;
    ALLEGRO_COLOR cor_jogador = al_map_rgb(255, 0, 0); ALLEGRO_COLOR cor_base = al_map_rgb(150, 150, 150); ALLEGRO_COLOR cor_texto = al_map_rgb(0, 0, 0);           

    for (i = 0; i < TAMANHO_TABULEIRO; i++) {
        int x = start_x + (i % cells_per_row) * (cell_size + spacing); int y = start_y + (i / cells_per_row) * (cell_size + spacing);
        ALLEGRO_COLOR cor = cor_base;
        if (i == jogador_principal.posicao_atual) cor = cor_jogador; 
        else if (tabuleiro[i].tipo == CASA_BATALHA) cor = al_map_rgb(200, 50, 50); 
        else if (tabuleiro[i].tipo == CASA_GEMA) cor = al_map_rgb(0, 150, 255);
        else if (tabuleiro[i].tipo == CASA_CHEGADA) cor = al_map_rgb(0, 255, 0);
        
        al_draw_filled_rectangle(x, y, x + cell_size, y + cell_size, cor);
        al_draw_rectangle(x, y, x + cell_size, y + cell_size, al_map_rgb(255, 255, 255), 1);
        
        al_draw_textf(font, cor_texto, x + cell_size / 2, y + cell_size / 2 - 10, ALLEGRO_ALIGN_CENTER, "CASA %d", i + 1);
        al_draw_textf(font, cor_texto, x + cell_size / 2, y + cell_size / 2 + 10, ALLEGRO_ALIGN_CENTER, 
                      (i == jogador_principal.posicao_atual) ? "JOGADOR" : (tabuleiro[i].tipo == CASA_BATALHA ? "BATALHA" : "OUTRO"));
    }
}
// Implementações de desenha_menu, desenha_hud, desenha_opcoes_gema, etc. devem ser adicionadas aqui.

// Implementação de desenha_menu
void desenha_menu(ALLEGRO_FONT *font) {
    ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR amarelo = al_map_rgb(255, 255, 0);
    
    al_draw_filled_rectangle(150, 100, 650, 500, al_map_rgb(50, 50, 50));
    al_draw_rectangle(150, 100, 650, 500, branco, 2);

    al_draw_text(font, amarelo, 400, 150, ALLEGRO_ALIGN_CENTER, "AVENTURA DE TABULEIRO");
    
    al_draw_text(font, branco, 400, 250, ALLEGRO_ALIGN_CENTER, "1. INICIAR NOVO JOGO");
    al_draw_text(font, branco, 250, 300, ALLEGRO_ALIGN_LEFT, "2. CARREGAR JOGO SALVO");
    al_draw_text(font, branco, 250, 350, ALLEGRO_ALIGN_LEFT, "3. MELHORES SCORES");
    al_draw_text(font, branco, 250, 400, ALLEGRO_ALIGN_LEFT, "4. SAIR");
    
    al_draw_text(font, al_map_rgb(150, 150, 150), 400, 450, ALLEGRO_ALIGN_CENTER, "Escolha uma opcao (1-4)...");
}

// Implementação de desenha_hud
void desenha_hud(ALLEGRO_FONT *font, Jogador *j) {
    int x_start = HUD_X_START;
    int y_pos = 10;
    ALLEGRO_COLOR cor_principal = al_map_rgb(200, 200, 255);
    
    int total_ataque = calcular_poder_total(j); // Chama a funcao auxiliar
    
    // Fundo do Painel Lateral
    al_draw_filled_rectangle(x_start - 5, 0, SCREEN_W, SCREEN_H, al_map_rgb(30, 30, 30));
    al_draw_line(x_start - 5, 0, x_start - 5, SCREEN_H, al_map_rgb(255, 255, 255), 3);

    al_draw_text(font, al_map_rgb(255, 255, 0), x_start, y_pos, 0, "--- STATUS ---");
    y_pos += 40;

    al_draw_textf(font, al_map_rgb(255, 50, 50), x_start, y_pos, 0, "HP: %d", j->vida);
    y_pos += 30;
    al_draw_textf(font, cor_principal, x_start, y_pos, 0, "Poder Total: %d", total_ataque);
    y_pos += 30;
    al_draw_textf(font, al_map_rgb(0, 255, 0), x_start, y_pos, 0, "Moedas: %d", j->moedas);
    y_pos += 30;
    al_draw_textf(font, al_map_rgb(0, 255, 0), x_start, y_pos, 0, "Gemas (Itens): %d", j->num_gemas_inventario);
    y_pos += 40;
    al_draw_textf(font, al_map_rgb(255, 255, 255), x_start, y_pos, 0, "SCORE: %d", j->score);
    y_pos += 40;
    
    // Arma Equipamento
    al_draw_text(font, cor_principal, x_start, y_pos, 0, "Arma Equip.:");
    y_pos += 30;
    al_draw_text(font, al_map_rgb(150, 200, 255), x_start, y_pos, 0, (j->id_arma_equipada > 0) ? todas_as_armas[j->id_arma_equipada - 1].nome : "Nenhuma");

    // NOTA: A chamada para desenha_objetivos deve vir aqui
}

// Implementação de desenha_opcoes_gema
void desenha_opcoes_gema(ALLEGRO_FONT *font) {
    al_draw_filled_rectangle(10, 200, 640, 350, al_map_rgb(20, 20, 70));
    al_draw_rectangle(10, 200, 640, 350, al_map_rgb(255, 255, 0), 2);

    al_draw_text(font, al_map_rgb(255, 255, 255), 325, 210, ALLEGRO_ALIGN_CENTER, 
                 "DERROTA! A batalha foi perdida.");
    al_draw_textf(font, al_map_rgb(0, 255, 255), 325, 250, ALLEGRO_ALIGN_CENTER, 
                  "Gemas disponiveis: %d", jogador_principal.num_gemas_inventario);

    if (jogador_principal.num_gemas_inventario > 0) {
        al_draw_text(font, al_map_rgb(0, 255, 0), 325, 280, ALLEGRO_ALIGN_CENTER, 
                     "Pressione [G] para CONSUMIR GEMA e tentar de novo!");
    }
    al_draw_text(font, al_map_rgb(255, 100, 100), 325, 310, ALLEGRO_ALIGN_CENTER, 
                 "Pressione [ESPACO] para tentar de novo sem gema.");
}

// --- MAIN ---
int main() {
    // 1. Inicialização OBRIGATÓRIA
    if (!al_init() || !al_init_font_addon() || !al_init_ttf_addon() || !al_install_keyboard() || !al_init_primitives_addon()) {
        fprintf(stderr, "Falha na inicializacao do Allegro.\n"); return -1;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    event_queue = al_create_event_queue();

    font_padrao = al_load_font("arial.ttf", 16, 0); 
    if (!font_padrao) font_padrao = al_create_builtin_font(); // Fallback

    // Inicialização do Jogador
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

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    bool running = true;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            
            if (estado_atual == ESTADO_MENU && event.keyboard.keycode == ALLEGRO_KEY_1) {
                 estado_atual = ESTADO_EXPLORACAO;
                 // As outras opcoes de menu (2, 3, 4) devem ser implementadas aqui

            } else if (estado_atual == ESTADO_EXPLORACAO) {
                if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    if (tabuleiro[jogador_principal.posicao_atual].tipo == CASA_BATALHA) {
                         // Se for Batalha, so avanca se o poder for maior que a forca inimiga
                         if (calcular_poder_total(&jogador_principal) > tabuleiro[jogador_principal.posicao_atual].valor_evento) {
                            avancar_passo(); 
                         }
                    } else {
                         avancar_passo(); // Avanca e processa
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
                        printf("GEMA CONSUMIDA! Poder permanente de +%d adicionado. Novo poder total: %d\n", poder_da_gema, calcular_poder_total(&jogador_principal));
                        estado_atual = ESTADO_EXPLORACAO;
                        processar_evento_casa(); // Tenta de novo
                    }
                } else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    estado_atual = ESTADO_EXPLORACAO; 
                    // Nao chama processar_evento_casa; o proximo ESPACO iniciara a nova tentativa
                }
            }
            // ... (Logica do ESC para sair)
        }

        // --- RENDERIZAÇÃO ---
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

    // --- DESTRUIÇÃO ---
    al_destroy_font(font_padrao);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    
    return 0;
}