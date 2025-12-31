#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "dinossauro.h"
#include "meteoro.h"
#include "item.h"
#include "score.h"

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0 

char nome_jogador[30] = "JOGADOR";
int opcao_selecionada = 1;
int modo_edicao_nome = 1;
int max_caracteres_nome = 15;
double ultimo_tempo_meteoro = 0.0;
float dificuldade_fator = 1.0;
float transicao_timer = 0.0;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_FONT *fonte_titulo = NULL;
ALLEGRO_FONT *fonte_menu = NULL;
ScoreEntry top_scores[MAX_SCORES];
Meteoro meteoros[MAX_METEOROS];

extern Meteoro meteoros[];

typedef enum {
    ESTADO_MENU_PRINCIPAL,
    ESTADO_INPUT_NOME,
    ESTADO_JOGO_ROLANDO,
    ESTADO_TRANSICAO_FASE,
    ESTADO_MELHORES_SCORES,
    ESTADO_SAIR,
} GAME_STATE;

GAME_STATE estado_atual = ESTADO_MENU_PRINCIPAL;

void inicializar_novo_jogo(Dinossauro *dino, Meteoro meteoros[], Item itens[]);
void gerenciar_input_menu(ALLEGRO_EVENT *ev, Dinossauro *dino, Meteoro meteoros[], Item itens[]);
void aumentar_dificuldade(Dinossauro *dino) {
    limpar_meteoros(meteoros);
    
    dificuldade_fator += FATOR_ACELERACAO;
    dino->fase_atual++;
    dino->tempo_fase = 0.0;
    
    transicao_timer = TEMPO_TRANSICAO;
    estado_atual = ESTADO_TRANSICAO_FASE;

    printf("--- MUDANCA DE FASE! ---\n");
    printf("Nova Fase: %d | Fator Dificuldade: %.2f\n", dino->fase_atual, dificuldade_fator);
}

void desenhar_input_nome() {
    al_clear_to_color(al_map_rgb(10, 10, 20));

    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255); 
    ALLEGRO_COLOR cor_nome_input = al_map_rgb(255, 255, 0);

    al_draw_text(fonte_titulo, cor_nome_input, LARGURA_TELA / 2, ALTURA_TELA / 2 - 100, ALLEGRO_ALIGN_CENTER, "ENTRADA DE JOGADOR");

    char texto_input[50];
    if (((int)(al_get_time() * 2)) % 2 == 0) {
        sprintf(texto_input, "NOME: %s|", nome_jogador);
    } else {
        sprintf(texto_input, "NOME: %s", nome_jogador);
    }

    al_draw_text(fonte_menu, cor_texto, LARGURA_TELA / 2, ALTURA_TELA / 2, ALLEGRO_ALIGN_CENTER, texto_input);

    al_draw_text(fonte_menu, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, ALTURA_TELA / 2 + 50, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para iniciar.");
}

void desenhar_menu_principal() {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    ALLEGRO_COLOR cor_fundo_retangulo = al_map_rgb(30, 30, 40);
    ALLEGRO_COLOR cor_texto_normal = al_map_rgb(200, 200, 255);
    ALLEGRO_COLOR cor_texto_selecionado = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR cor_borda_retangulo = al_map_rgb(100, 100, 150);

    int ret_largura = 400;
    int ret_altura = 350;
    int ret_x1 = (LARGURA_TELA - ret_largura) / 2;
    int ret_y1 = (ALTURA_TELA - ret_altura) / 2;
    int ret_x2 = ret_x1 + ret_largura;
    int ret_y2 = ret_y1 + ret_altura;

    al_draw_filled_rectangle(ret_x1, ret_y1, ret_x2, ret_y2, cor_fundo_retangulo);
    al_draw_rectangle(ret_x1, ret_y1, ret_x2, ret_y2, cor_borda_retangulo, 4);

    if (fonte_titulo) {
        al_draw_text(fonte_titulo, cor_texto_selecionado, LARGURA_TELA / 2, ret_y1 + 30, ALLEGRO_ALIGN_CENTER, "DINNO METEOR");
    }

    int y_pos = ret_y1 + 120;
    int espaco_y = 50;
    
    if (modo_edicao_nome) {
        al_draw_text(fonte_menu, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, y_pos + 25, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para continuar");
    }
    
    y_pos += espaco_y;
    y_pos += 15;

    ALLEGRO_COLOR cor_opt1 = (opcao_selecionada == 1) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt1, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "1. INICIAR JOGO");
    y_pos += espaco_y;

    ALLEGRO_COLOR cor_opt2 = (opcao_selecionada == 2) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt2, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "2. MELHORES SCORES");
    y_pos += espaco_y;

    ALLEGRO_COLOR cor_opt3 = (opcao_selecionada == 3) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt3, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "3. SAIR");
}

void gerenciar_input_menu(ALLEGRO_EVENT *ev, Dinossauro *dino, Meteoro meteoros[], Item itens[]) {
    if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (estado_atual == ESTADO_MENU_PRINCIPAL) {
            switch (ev->keyboard.keycode) {
                case ALLEGRO_KEY_DOWN:
                    opcao_selecionada++;
                    if (opcao_selecionada > 3) opcao_selecionada = 1;
                    break;
                case ALLEGRO_KEY_UP:
                    opcao_selecionada--;
                    if (opcao_selecionada < 1) opcao_selecionada = 3;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if (opcao_selecionada == 1) { 
                        estado_atual = ESTADO_INPUT_NOME;
                        modo_edicao_nome = 1; 
                    } else if (opcao_selecionada == 2) {
                        carregar_scores(top_scores);
                        estado_atual = ESTADO_MELHORES_SCORES;
                    } else if (opcao_selecionada == 3) { 
                        estado_atual = ESTADO_SAIR; 
                    }
                    break;
            }
        } else if (estado_atual == ESTADO_INPUT_NOME) {
            if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                inicializar_novo_jogo(dino, meteoros, itens);
                estado_atual = ESTADO_JOGO_ROLANDO; 
                modo_edicao_nome = 0;
            } else if (ev->keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                if (strlen(nome_jogador) > 0) {
                    nome_jogador[strlen(nome_jogador) - 1] = '\0';
                }
            }
        }
    } else if (ev->type == ALLEGRO_EVENT_KEY_CHAR) {
        if (estado_atual == ESTADO_INPUT_NOME) {
            if (ev->keyboard.unichar >= ' ' && ev->keyboard.unichar <= '~' && strlen(nome_jogador) < max_caracteres_nome) {
                char ch = ev->keyboard.unichar;
                size_t len = strlen(nome_jogador);
                nome_jogador[len] = ch;
                nome_jogador[len + 1] = '\0';
            }
        }
    }
}

void desenhar_scores(ALLEGRO_FONT *font_menu, ALLEGRO_FONT *fonte_titulo, ScoreEntry top_scores[]) {
    for (int i = 0; i < MAX_SCORES; i++) {
        char linha_score[100];

        al_draw_text(fonte_menu, al_map_rgb(200, 200, 255), LARGURA_TELA / 2, 520, ALLEGRO_ALIGN_CENTER, "Pressione ESC para sair");

        snprintf(linha_score, 100, "%02d. %-15s - SCORE: %d (FASE %d)", i + 1, top_scores[i].nome, top_scores[i].score, top_scores[i].fase_final);
                 
        al_draw_text(font_menu, al_map_rgb(200, 200, 255), LARGURA_TELA / 2, 120 + i * 40, ALLEGRO_ALIGN_CENTER, linha_score);
    }
}

void renderizar_transicao_fase(ALLEGRO_FONT *font, int fase_atual, float dificuldade_fator) {
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 200));

    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 0); 
    
    char msg_titulo[50];
    sprintf(msg_titulo, "FASE %d CONCLUÍDA!", fase_atual - 1); 

    char msg_proxima[50];
    sprintf(msg_proxima, "INICIANDO FASE %d", fase_atual);

    float aumento_total = (dificuldade_fator - 1.0) * 100;

    char msg_dificuldade[50];
    sprintf(msg_dificuldade, "Velocidade: +%.0f%%", aumento_total);

    char msg_tempo_transicao[50];
    sprintf(msg_tempo_transicao, "VOLTANDO EM: %.1fs", transicao_timer);    

    al_draw_text(font, cor_texto, LARGURA_TELA / 2, ALTURA_TELA / 2 - 50, ALLEGRO_ALIGN_CENTER, msg_titulo);
                 
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2, ALLEGRO_ALIGN_CENTER, msg_proxima);
                 
    al_draw_text(font, al_map_rgb(100, 255, 100), LARGURA_TELA / 2, ALTURA_TELA / 2 + 50, ALLEGRO_ALIGN_CENTER, msg_dificuldade);

    al_draw_textf(fonte_menu, al_map_rgb(255, 255, 0), LARGURA_TELA / 2, ALTURA_TELA /2 + 85, ALLEGRO_ALIGN_CENTER, msg_tempo_transicao);
}

void limpar_meteoros(Meteoro meteoros[]) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        if (meteoros[i].ativo) {
            meteoros[i].ativo = 0;
        }
    }
}

void tratar_dano_meteoro(Dinossauro *dino, Meteoro *meteoro) {
    int dano_base = 1; 
    int dano_real = dano_base;

    if (meteoro->nivel > 1) { 
        if (dino->poder_bonus == 1) {
            dano_real = 2; 
        } else if (dino->poder_bonus == 2) {
            dano_real = meteoro->vida_atual;
        }
    }

    if (meteoro->vida_atual > dano_real) {
        meteoro->velocidade_y = FORCA_IMPACTO_DESCENDENTE;
        meteoro->y += METEORO_BOUNCE_DISPLACEMENT;
        
        printf("Meteoro Nível %d cabeceado. Restam %d hits.\n", meteoro->nivel, meteoro->vida_atual - dano_real);
    }

    meteoro->vida_atual -= dano_real;

    if (meteoro->vida_atual <= 0) {
        meteoro->ativo = 0;
        if (meteoro->nivel == 1) {
            dino->score += PONTOS_POR_METEORO_NIVEL_1;
        } else if (meteoro->nivel == 2) {
            dino->score += PONTOS_POR_METEORO_NIVEL_2;
        } else if (meteoro->nivel == 3) {
            dino->score += PONTOS_POR_METEORO_NIVEL_3;
        }
        
        printf("Meteoro Nível %d destruído! Score: %d\n", meteoro->nivel, dino->score);
    }
}

int inicializar_allegro() {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar o Allegro.\n");
        return 0;
    }
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar primitives addon.\n");
        return 0;
    }
    if (!al_init_font_addon() || !al_init_ttf_addon()) {
        fprintf(stderr, "Falha ao inicializar font ou ttf addon.\n");
        return 0;
    }
    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao inicializar o teclado.\n");
        return 0;
    }

    if(!al_init_image_addon()) {
        fprintf(stderr, "Falha ao iniciar imagens");
        return 0;
    }


    display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        fprintf(stderr, "Falha ao criar o display.\n");
        return 0;
    }

    fonte_titulo = al_create_builtin_font();
    fonte_menu = al_create_builtin_font();

    if (!fonte_titulo || !fonte_menu) {
        fprintf(stderr, "Aviso: Falha ao carregar fontes padrão do Allegro.\n");
        return 0;
    }

    al_set_window_title(display, "Dinno Meteor");
    return 1;
}

void desinicializar_allegro() {
    if (fonte_titulo) al_destroy_font(fonte_titulo);
    if (fonte_menu) al_destroy_font(fonte_menu);
    if (display) al_destroy_display(display);
}

void inicializar_novo_jogo(Dinossauro *dino, Meteoro meteoros[], Item itens[]) {
    dinossauro_inicializar(dino, LARGURA_TELA / 4.0);

    dino->score = 0;
    dino->fase_atual = 1;
    dino->tempo_fase = 0.0;
    dino->vida = 100;
    
    inicializar_meteoros(meteoros); 
    inicializar_itens(itens); 

    modo_edicao_nome = 1;
}

int main() {
    if (!inicializar_allegro()) {
        desinicializar_allegro();
        return -1;
    }

    ALLEGRO_TIMER *timer = NULL;
    Dinossauro dino;

    Meteoro meteoros[MAX_METEOROS];

    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "Falha ao criar o timer.\n");
    }

    Item itens[MAX_ITENS];
    double ultimo_tempo_item = 0.0;

    dinossauro_inicializar(&dino, LARGURA_TELA / 4);
    inicializar_meteoros(meteoros);
    inicializar_itens(itens);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    int rodando = 1;
    int redesenhar = 1;
    ALLEGRO_EVENT ev;

    while (rodando) {
        al_wait_for_event(event_queue, &ev);

        if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            if (estado_atual == ESTADO_MELHORES_SCORES) {
                estado_atual = ESTADO_MENU_PRINCIPAL; 
                modo_edicao_nome = 1;
            }
        }
        
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;            
        } 
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (estado_atual == ESTADO_MENU_PRINCIPAL || estado_atual == ESTADO_INPUT_NOME) {
                gerenciar_input_menu(&ev, &dino, meteoros, itens);
            } else if (estado_atual == ESTADO_JOGO_ROLANDO) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_SPACE:
                        dinossauro_pular(&dino);
                        break;
                    case ALLEGRO_KEY_LEFT:
                        dino.dx = -VELOCIDADE_X;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        dino.dx = VELOCIDADE_X;
                        break;
                }
            }
        } 
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (estado_atual == ESTADO_JOGO_ROLANDO) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_LEFT:
                        if (dino.dx < 0) dino.dx = 0;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        if (dino.dx > 0) dino.dx = 0;
                        break;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (estado_atual == ESTADO_INPUT_NOME) {
                gerenciar_input_menu(&ev, &dino, meteoros, itens);
            }

        } else if (ev.type == ALLEGRO_EVENT_TIMER) {
            double tempo_atual = al_get_time();
            int score_limite;
            
            if (estado_atual == ESTADO_JOGO_ROLANDO) {
                score_limite = dino.fase_atual * PONTOS_POR_FASE;
                if (tempo_atual - ultimo_tempo_meteoro > (1.5 + (rand() % 150) / 100.0) ) { 
                    gerar_novo_meteoro(meteoros, dificuldade_fator);
                    ultimo_tempo_meteoro = tempo_atual;
                }
                if (dino.score >= score_limite) {
                    aumentar_dificuldade(&dino);
                }

                dinossauro_atualizar(&dino);
                atualizar_meteoros(meteoros);

                if (dino.vida <= 0) {
                    atualizar_e_salvar_score(nome_jogador, dino.score, dino.fase_atual);
                    carregar_scores(top_scores);

                    estado_atual = ESTADO_MELHORES_SCORES;
                    redesenhar = 1;
                }

                if (dino.tempo_bonus > 0) {
                    dino.tempo_bonus -= (1.0 / FPS);
                    if (dino.tempo_bonus <= 0) {
                        dino.poder_bonus = 0;
                        printf("Bônus expirou. Dinossauro normal.\n");
                    }
                }

                atualizar_itens(itens);

                if (tempo_atual - ultimo_tempo_item > TEMPO_ENTRE_GERACAO_ITENS) {
                    TipoItem item_a_gerar;
                    int chance = rand() % 100;
                    
                    if (chance < 30) {
                        item_a_gerar = ITEM_VIDA;
                    } else if (chance < 70) {
                        item_a_gerar = ITEM_BONUS_1;
                    } else {
                        item_a_gerar = ITEM_BONUS_2;
                    }
                    
                    gerar_item(itens, item_a_gerar);
                    ultimo_tempo_item = tempo_atual;
                }

                checar_coleta_item(&dino, itens);

                int indice_meteoro_atingido = checar_colisao_com_cabeca(&dino, meteoros);
                        
                if (indice_meteoro_atingido != -1) {
                    tratar_dano_meteoro(&dino, &meteoros[indice_meteoro_atingido]); 
                }

                int dano_chao = checar_e_tratar_chao(meteoros);
                if (dano_chao > 0) {
                    dino.vida -= dano_chao;
                    if (dino.vida <= 0) {
                    }
                }

                score_limite = dino.fase_atual * PONTOS_POR_FASE;
                if (dino.score >= score_limite) {
                    aumentar_dificuldade(&dino);
                    redesenhar = 1;
                }
            } 
            
            else if (estado_atual == ESTADO_TRANSICAO_FASE) {
                transicao_timer -= 1.0 / FPS; 
                
                if (transicao_timer <= 0.0) {
                    estado_atual = ESTADO_JOGO_ROLANDO;
                }
            }
            redesenhar = 1;
        } 

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = 0;

            if (estado_atual == ESTADO_MENU_PRINCIPAL) {
                desenhar_menu_principal();
                
            } else if (estado_atual == ESTADO_INPUT_NOME) {
                desenhar_input_nome();
            }
            
            else if (estado_atual == ESTADO_JOGO_ROLANDO) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                desenhar_chao();
                dinossauro_desenhar(&dino);
                desenhar_meteoros(meteoros);
                desenhar_itens(itens);
                al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), 10, 10, 0, "Vida: %d", dino.vida);
                if (dino.poder_bonus > 0) {
                    al_draw_textf(fonte_menu, al_map_rgb(255, 255, 0), LARGURA_TELA - 10, 60, ALLEGRO_ALIGN_RIGHT, "BÔNUS: %.1fs", dino.tempo_bonus);
                }

                al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), LARGURA_TELA - 10, 10, ALLEGRO_ALIGN_RIGHT, "SCORE: %d", dino.score);
                al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), LARGURA_TELA - 10, 35, ALLEGRO_ALIGN_RIGHT, "FASE: %d", dino.fase_atual);
                al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 10, ALLEGRO_ALIGN_CENTER, "JOGADOR: %s", nome_jogador);
                
            } 
            
            else if (estado_atual == ESTADO_TRANSICAO_FASE) {
                renderizar_transicao_fase(fonte_menu, dino.fase_atual, dificuldade_fator);
            }

            else if (estado_atual == ESTADO_MELHORES_SCORES) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                desenhar_scores(fonte_menu, fonte_titulo, top_scores);
            }
            else if (estado_atual == ESTADO_SAIR) {
                rodando = 0;
            }
            
            al_flip_display();
        }
    }

    desinicializar_allegro();
    return 0;
}