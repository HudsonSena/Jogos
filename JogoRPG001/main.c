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

// Definição de Armas Únicas
Arma todas_as_armas[MAX_ARMAS] = {
    {1, "Espada de Treino", 10},
    {2, "Cajado Magico", 25},
    {3, "Machado de Guerra", 40},
    {4, "Lanca Lendaria", 60},
    {5, "Espada Final", 100}
};

// Definição de Gemas Únicas
Gema todas_as_gemas[MAX_GEMAS] = {
    {1, 15}, {2, 30}, {3, 50}, {4, 80}, {5, 120}
};

// Mapa Linear (TAMANHO_TABULEIRO = 10)
Casa tabuleiro[TAMANHO_TABULEIRO] = {
    {CASA_VAZIA, "Inicio da Aventura.", 0, {1}, 1, 0, 0},
    {CASA_BATALHA, "Goblin Fraco.", 15, {2}, 1, 0, 0}, 
    {CASA_ARMA, "Encontrou Espada de Treino!", 0, {3}, 1, 1, 0}, 
    {CASA_BATALHA, "Orc Guerreiro.", 35, {4}, 1, 0, 0}, 
    {CASA_GEMA, "Encontrou Gema de Poder!", 0, {5}, 1, 0, 1}, 
    {CASA_BATALHA, "Minotauro Furioso.", 60, {6}, 1, 0, 0}, 
    {CASA_ARMA, "Encontrou Cajado Magico!", 0, {7}, 1, 2, 0}, 
    {CASA_BATALHA, "Dragao Pequeno.", 95, {8}, 1, 0, 0},
    {CASA_GEMA, "Encontrou Gema de Poder Avancada!", 0, {9}, 1, 0, 2}, 
    {CASA_CHEGADA, "FIM DO JOGO!", 0, {-1}, 0, 0, 0}
};

Objetivo objetivos[MAX_OBJETIVOS] = {
    {"Coletar 3 Armas", 3, 0, false},
    {"Acumular 500 Gemas", 500, 0, false},
    {"Chegar na Casa 10", 9, 0, false}
};


// --- IMPLEMENTAÇÃO DE FUNÇÕES DE LÓGICA E UTILIDADE (Protótipos no jogo.h) ---

int calcular_poder_total(const Jogador *j) {
    int poder_arma = 0;
    
    for (int i = 0; i < MAX_ARMAS; i++) {
        if (todas_as_armas[i].id == j->id_arma_equipada) {
            poder_arma = todas_as_armas[i].bonus_ataque;
            break;
        }
    }
    return j->ataque_base + j->poder_ganho_batalha + poder_arma + j->poder_gema_ativo;
}

void atualizar_objetivos() {
    objetivos[0].valor_atual = jogador_principal.armas_coletadas;
    if (jogador_principal.armas_coletadas >= objetivos[0].valor_necessario) {
        objetivos[0].completado = true;
    }
    objetivos[2].valor_atual = jogador_principal.posicao_atual;
    if (jogador_principal.posicao_atual >= objetivos[2].valor_necessario) {
        objetivos[2].completado = true;
    }
}

void processar_evento_casa() {
    int pos = jogador_principal.posicao_atual;
    if (pos >= TAMANHO_TABULEIRO) return; 
    const Casa *casa = &tabuleiro[pos];
    
    printf("\n--- CASA %d - EVENTO: %s ---\n", pos + 1, casa->descricao);

    if (estado_atual != ESTADO_OPCAO_GEMA) {
        jogador_principal.poder_gema_ativo = 0; 
    }

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
                printf("VITORIA! Ganha +5 de poder, 50 moedas e uma Gema Unica (se houver).\n");
                jogador_principal.poder_ganho_batalha += 5;
                jogador_principal.moedas += 50;
                
                int id_proxima_gema = jogador_principal.num_gemas_inventario + 1;
                if (id_proxima_gema <= MAX_GEMAS) {
                    jogador_principal.gemas_inventario[jogador_principal.num_gemas_inventario] = todas_as_gemas[id_proxima_gema - 1];
                    jogador_principal.num_gemas_inventario++;
                    printf("Nova Gema Unica Adquirida! Total: %d\n", jogador_principal.num_gemas_inventario);
                }
                
            } else {
                printf("DERROTA! Consuma gema [G] ou tente novamente [ESPACO].\n");
                estado_atual = ESTADO_OPCAO_GEMA;
            }
            if (jogador_principal.vida <= 0) estado_atual = ESTADO_SAIR;
            break;
        }
        
        case CASA_GEMA:
            jogador_principal.moedas += casa->valor_evento;
            jogador_principal.total_gemas += casa->valor_evento;
            printf("GEMA COLETADA! Ganhou %d moedas. Total Gemas Valor: %d.\n", casa->valor_evento, jogador_principal.total_gemas);
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
            printf("--- FIM DO JOGO --- Parabens!\n");
            estado_atual = ESTADO_SAIR;
            break;
    }

    atualizar_objetivos();
}

// --- IMPLEMENTAÇÃO DE FUNÇÕES DE RENDERIZAÇÃO (Solução para o Linker Error) ---

void desenha_tabuleiro(ALLEGRO_FONT *font) {
    int i;
    int start_x = 50; 
    int start_y = 100;
    int cell_size = 80;
    int spacing = 20;
    int cells_per_row = 5;

    ALLEGRO_COLOR cor_jogador = al_map_rgb(255, 0, 0);       
    ALLEGRO_COLOR cor_base = al_map_rgb(150, 150, 150);      
    ALLEGRO_COLOR cor_texto = al_map_rgb(0, 0, 0);           

    for (i = 0; i < TAMANHO_TABULEIRO; i++) {
        int x = start_x + (i % cells_per_row) * (cell_size + spacing);
        int y = start_y + (i / cells_per_row) * (cell_size + spacing);
        
        ALLEGRO_COLOR cor = cor_base;
        
        if (i == jogador_principal.posicao_atual) {
            cor = cor_jogador; 
        } else if (tabuleiro[i].tipo == CASA_BATALHA) {
            cor = al_map_rgb(200, 50, 50); 
        } else if (tabuleiro[i].tipo == CASA_GEMA) {
            cor = al_map_rgb(0, 150, 255);
        } else if (tabuleiro[i].tipo == CASA_CHEGADA) {
            cor = al_map_rgb(0, 255, 0);
        }
        
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
    
    al_draw_filled_rectangle(150, 100, 650, 500, al_map_rgb(50, 50, 50));
    al_draw_rectangle(150, 100, 650, 500, branco, 2);

    al_draw_text(font, amarelo, 400, 150, ALLEGRO_ALIGN_CENTER, "AVENTURA DE TABULEIRO");
    
    al_draw_text(font, branco, 400, 250, ALLEGRO_ALIGN_CENTER, "1. INICIAR NOVO JOGO");
    al_draw_text(font, branco, 250, 300, ALLEGRO_ALIGN_LEFT, "2. CARREGAR JOGO SALVO");
    al_draw_text(font, branco, 250, 350, ALLEGRO_ALIGN_LEFT, "3. MELHORES SCORES");
    al_draw_text(font, branco, 250, 400, ALLEGRO_ALIGN_LEFT, "4. SAIR");
    
    al_draw_text(font, al_map_rgb(150, 150, 150), 400, 450, ALLEGRO_ALIGN_CENTER, "Escolha uma opcao (1-4)...");
}

void desenha_hud(ALLEGRO_FONT *font, Jogador *j) {
    int x_start = HUD_X_START;
    int y_pos = 10;
    ALLEGRO_COLOR cor_principal = al_map_rgb(200, 200, 255);
    
    al_draw_filled_rectangle(x_start - 5, 0, SCREEN_W, SCREEN_H, al_map_rgb(30, 30, 30));
    al_draw_line(x_start - 5, 0, x_start - 5, SCREEN_H, al_map_rgb(255, 255, 255), 3);

    al_draw_text(font, al_map_rgb(255, 255, 0), x_start, y_pos, 0, "--- STATUS ---");
    y_pos += 40;

    int total_ataque = calcular_poder_total(j);

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

    // OBJETIVOS - A implementação de desenha_objetivos completa deve ser chamada aqui
}

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

// Em main.c (junto com as outras funções de desenho)

// Em main.c (junto com outras funcoes de I/O de arquivo)

// Funcao que carrega e ordena os High Scores para exibicao
void carregar_high_scores(HighScore scores[]) {
    FILE *file = fopen(NOME_ARQUIVO_SCORES, "rb");
    int i = 0;
    
    // 1. Inicializa o array de scores para evitar lixo de memoria
    for (i = 0; i < MAX_HIGH_SCORES; i++) {
        scores[i].score = -1; 
        scores[i].nome[0] = '\0';
    }

    // 2. Tenta abrir o arquivo e ler
    if (file) {
        i = 0;
        // Le as structs de HighScore do arquivo
        while (fread(&scores[i], sizeof(HighScore), 1, file) == 1 && i < MAX_HIGH_SCORES) {
            i++;
        }
        fclose(file);
    }
    
    // Nota: A logica de ordenacao por score (se houver) deve vir aqui,
    // garantindo que os 5 melhores scores estejam no topo.
}

void desenha_high_scores(ALLEGRO_FONT *font) {
    // Variáveis locais para armazenar os scores carregados
    HighScore scores[MAX_HIGH_SCORES]; 
    
    // Tenta carregar os scores. (A implementação desta função deve existir!)
    // Nota: Se carregar_high_scores nao existe, este sera o proximo erro de ligacao.
    // Assumimos que a implementacao de carregar_high_scores esta em main.c
    carregar_high_scores(scores); 

    ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR amarelo = al_map_rgb(255, 255, 0);

    // Desenha a caixa central
    al_draw_filled_rectangle(150, 100, 650, 500, al_map_rgb(50, 50, 50));
    al_draw_rectangle(150, 100, 650, 500, branco, 2);

    al_draw_text(font, amarelo, 400, 120, ALLEGRO_ALIGN_CENTER, "--- 5 MELHORES SCORES ---");

    int y_pos = 180;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        // Assume que score >= 0 significa slot preenchido
        if (scores[i].score >= 0) { 
            al_draw_textf(font, branco, 200, y_pos, 0, 
                          "%d. %s", i + 1, scores[i].nome); // Exibe Rank e Nome
            al_draw_textf(font, branco, 550, y_pos, ALLEGRO_ALIGN_RIGHT, 
                          "%d pts", scores[i].score); // Exibe Pontuação
        } else {
            al_draw_textf(font, al_map_rgb(150, 150, 150), 400, y_pos, ALLEGRO_ALIGN_CENTER, 
                          "Slot %d vazio", i + 1);
        }
        y_pos += 40;
    }
    
    al_draw_text(font, al_map_rgb(150, 150, 150), 400, 450, ALLEGRO_ALIGN_CENTER, 
                 "Pressione ESC para voltar ao Menu.");
}

// Implementações de funções de I/O de arquivo (carregar_high_scores, salvar_jogo, carregar_jogo) e as outras funções de desenho (desenha_objetivos, desenha_high_scores, desenha_menu_carregar_jogo) devem ser adicionadas aqui.
// Elas foram omitidas para o foco principal ser a correção do 'undefined reference'.

// --- MAIN ---
int main() {
    if (!al_init()) return -1;
    if (!al_init_font_addon()) return -1;
    if (!al_init_ttf_addon()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar add-on de primitivas!\n");
        return -1;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    event_queue = al_create_event_queue();

    font_padrao = al_load_font("arial.ttf", 16, 0); 
    if (!font_padrao) {
        font_padrao = al_create_builtin_font();
        if (!font_padrao) {
            fprintf(stderr, "Erro critico: Falha ao carregar qualquer fonte.\n");
            al_destroy_display(display);
            return -1;
        }
    }

    // Inicialização do Jogador
    jogador_principal.vida = 100;
    jogador_principal.ataque_base = 15; 
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

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                if (estado_atual == ESTADO_HIGH_SCORES) {
                    estado_atual = ESTADO_MENU; 
                } else {
                    running = false; 
                }
            }
            
            // Tratamento de Menu e Seleção de Estado (omissões no switch de estados)
            // ...

            // Tratamento de Combate/Avanco (omissões no switch de estados)
            // ...
        }

        // --- RENDERIZAÇÃO ---
        al_clear_to_color(al_map_rgb(10, 10, 50));

        if (estado_atual == ESTADO_MENU) {
            desenha_menu(font_padrao);
        } else if (estado_atual == ESTADO_HIGH_SCORES) {
            desenha_high_scores(font_padrao);
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