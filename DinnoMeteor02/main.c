#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include "dinossauro.h"
#include "meteoro.h"
#include "item.h"
#include "score.h"

// --- DIMENSÕES DA TELA ---
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60.0 

// --- VARIÁVEIS GLOBAIS DE JOGO ---
char nome_jogador[30] = "JOGADOR"; // Nome inicial
int opcao_selecionada = 1;         // 1: Iniciar, 2: Scores, 3: Sair
int modo_edicao_nome = 1;          // NOVO: 1 se estiver editando, 0 caso contrário
int max_caracteres_nome = 15;      // Limite de caracteres para o nome
double ultimo_tempo_meteoro = 0.0;
float dificuldade_fator = 1.0;
float transicao_timer = 0.0;
// --- VARIÁVEIS DO ALLEGRO ---
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_FONT *fonte_titulo = NULL;
ALLEGRO_FONT *fonte_menu = NULL;
ScoreEntry top_scores[MAX_SCORES];
Meteoro meteoros[MAX_METEOROS];
extern Meteoro meteoros[];

// --- ESTRUTURA E ESTADOS DE JOGO (como definido antes) ---
typedef enum {
    ESTADO_MENU_PRINCIPAL,
    ESTADO_INPUT_NOME,
    ESTADO_JOGO_ROLANDO,
    ESTADO_TRANSICAO_FASE,
    ESTADO_MELHORES_SCORES,
    ESTADO_SAIR,
    // ... outros estados ...
} GAME_STATE;

GAME_STATE estado_atual = ESTADO_MENU_PRINCIPAL;

// -----------------------------------------------------------------------------
// FUNÇÕES DE DESENHO
// -----------------------------------------------------------------------------

// main.c (Nova função)

void inicializar_novo_jogo(Dinossauro *dino, Meteoro meteoros[], Item itens[]);
void gerenciar_input_menu(ALLEGRO_EVENT *ev, Dinossauro *dino, Meteoro meteoros[], Item itens[]);
//void aumentar_dificuldade(Dinossauro *dino);
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

    // Título
    al_draw_text(fonte_titulo, cor_nome_input, LARGURA_TELA / 2, ALTURA_TELA / 2 - 100, ALLEGRO_ALIGN_CENTER, "ENTRADA DE JOGADOR");

    // Lógica do Cursor Piscante
    char texto_input[50];
    if (((int)(al_get_time() * 2)) % 2 == 0) {
        sprintf(texto_input, "NOME: %s|", nome_jogador);
    } else {
        sprintf(texto_input, "NOME: %s", nome_jogador);
    }
    
    // Desenha o input
    al_draw_text(fonte_menu, cor_texto, LARGURA_TELA / 2, ALTURA_TELA / 2, ALLEGRO_ALIGN_CENTER, texto_input);
    
    // Dica de navegação
    al_draw_text(fonte_menu, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, ALTURA_TELA / 2 + 50, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para iniciar.");
}

void desenhar_menu_principal() {
    // 1. Fundo Preto
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // 2. Cores
    ALLEGRO_COLOR cor_fundo_retangulo = al_map_rgb(30, 30, 40); // Azul escuro suave
    ALLEGRO_COLOR cor_texto_normal = al_map_rgb(200, 200, 255); // Branco/Azul claro
    ALLEGRO_COLOR cor_texto_selecionado = al_map_rgb(255, 255, 0); // Amarelo vibrante
    ALLEGRO_COLOR cor_borda_retangulo = al_map_rgb(100, 100, 150); // Borda

    // 3. Posições do Retângulo Central
    int ret_largura = 400;
    int ret_altura = 350;
    int ret_x1 = (LARGURA_TELA - ret_largura) / 2;
    int ret_y1 = (ALTURA_TELA - ret_altura) / 2;
    int ret_x2 = ret_x1 + ret_largura;
    int ret_y2 = ret_y1 + ret_altura;

    // 4. Desenhar Retângulo de Fundo
    al_draw_filled_rectangle(ret_x1, ret_y1, ret_x2, ret_y2, cor_fundo_retangulo);
    al_draw_rectangle(ret_x1, ret_y1, ret_x2, ret_y2, cor_borda_retangulo, 4); // Borda

    // 5. Desenhar Título do Jogo
    if (fonte_titulo) {
        al_draw_text(fonte_titulo, cor_texto_selecionado, LARGURA_TELA / 2, ret_y1 + 30, ALLEGRO_ALIGN_CENTER, "DINNO METEOR");
    }

    // 6. Desenhar Opções de Menu
    
    // Posição Y inicial para as opções
    int y_pos = ret_y1 + 120;
    int espaco_y = 50;
    
    // Dica de navegação
    if (modo_edicao_nome) {
        al_draw_text(fonte_menu, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, y_pos + 25, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para continuar");
    }
    
    y_pos += espaco_y;
    y_pos += 15; // Adiciona um espaço extra para a linha de dica

    // --- Opção 1: Iniciar Jogo ---
    ALLEGRO_COLOR cor_opt1 = (opcao_selecionada == 1) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt1, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "1. INICIAR JOGO");
    y_pos += espaco_y;

    // --- Opção 2: Melhores Scores ---
    ALLEGRO_COLOR cor_opt2 = (opcao_selecionada == 2) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt2, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "2. MELHORES SCORES");
    y_pos += espaco_y;

    // --- Opção 3: Sair ---
    ALLEGRO_COLOR cor_opt3 = (opcao_selecionada == 3) ? cor_texto_selecionado : cor_texto_normal;
    al_draw_text(fonte_menu, cor_opt3, LARGURA_TELA / 2, y_pos, ALLEGRO_ALIGN_CENTER, "3. SAIR");
}

void gerenciar_input_menu(ALLEGRO_EVENT *ev, Dinossauro *dino, Meteoro meteoros[], Item itens[]) { // Lembre-se de passar dino
    if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {

        // --- 1. ESTADO DE NAVEGAÇÃO DO MENU PRINCIPAL ---
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
                // ... (UP/DOWN logic) ...
                case ALLEGRO_KEY_ENTER:
                    if (opcao_selecionada == 1) { 
                        // Mudar para o estado de INPUT DO NOME
                        estado_atual = ESTADO_INPUT_NOME; 
                        // Resetamos o modo_edicao_nome para 1 (ativado)
                        modo_edicao_nome = 1; 
                    } else if (opcao_selecionada == 2) {
                        carregar_scores(top_scores);
                        estado_atual = ESTADO_MELHORES_SCORES;
                    } else if (opcao_selecionada == 3) { 
                        estado_atual = ESTADO_SAIR; 
                    }
                    break;
            }
        
        // --- 2. ESTADO DE INPUT DO NOME (processa ENTER e BACKSPACE) ---
        } else if (estado_atual == ESTADO_INPUT_NOME) {
            if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                // Se ENTER no INPUT, inicia o jogo!
                inicializar_novo_jogo(dino, meteoros, itens); // Reset
                estado_atual = ESTADO_JOGO_ROLANDO; 
                modo_edicao_nome = 0; // Desativa a edição
            } else if (ev->keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                if (strlen(nome_jogador) > 0) {
                    nome_jogador[strlen(nome_jogador) - 1] = '\0';
                }
            }
        }
    
    // ------------------------------------
    // MODO EDIÇÃO DE NOME (Input de caracteres)
    // ------------------------------------
    } else if (ev->type == ALLEGRO_EVENT_KEY_CHAR) {
        // A digitação agora só é permitida no estado ESTADO_INPUT_NOME
        if (estado_atual == ESTADO_INPUT_NOME) { 
            // Sua lógica de entrada de caracteres permanece aqui...
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
    // ... (Código anterior) ...
    for (int i = 0; i < MAX_SCORES; i++) {
        char linha_score[100];
        
        // Exibe a Posição, Nome, Score e Fase Final.
        // O nome é lido diretamente do arquivo binário.
        snprintf(linha_score, 100, "%02d. %-15s - SCORE: %d (FASE %d)", 
                 i + 1, top_scores[i].nome, top_scores[i].score, top_scores[i].fase_final);
                 
        al_draw_text(font_menu, al_map_rgb(200, 200, 255), LARGURA_TELA / 2, 120 + i * 40, ALLEGRO_ALIGN_CENTER, linha_score);
    }
    // ...
}

void renderizar_transicao_fase(ALLEGRO_FONT *font, int fase_atual, float dificuldade_fator) {
    // Fundo semitransparente para focar na mensagem
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 200)); 
    
    // Cor amarela/branca para destaque
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 0); 
    
    char msg_titulo[50];
    sprintf(msg_titulo, "FASE %d CONCLUÍDA!", fase_atual - 1); 

    char msg_proxima[50];
    sprintf(msg_proxima, "INICIANDO FASE %d", fase_atual);
    
    // Calcula a porcentagem total de aumento
    float aumento_total = (dificuldade_fator - 1.0) * 100;
    char msg_dificuldade[50];
    sprintf(msg_dificuldade, "Velocidade: +%.0f%%", aumento_total);

    // Desenha as mensagens no centro
    al_draw_text(font, cor_texto, LARGURA_TELA / 2, ALTURA_TELA / 2 - 50,
                 ALLEGRO_ALIGN_CENTER, msg_titulo);
                 
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2,
                 ALLEGRO_ALIGN_CENTER, msg_proxima);
                 
    al_draw_text(font, al_map_rgb(100, 255, 100), LARGURA_TELA / 2, ALTURA_TELA / 2 + 50,
                 ALLEGRO_ALIGN_CENTER, msg_dificuldade);
}

void limpar_meteoros(Meteoro meteoros[]) {
    for (int i = 0; i < MAX_METEOROS; i++) {
        // Se o meteoro estiver ativo, desative-o
        if (meteoros[i].ativo) {
            meteoros[i].ativo = 0;
        }
    }
}

void tratar_dano_meteoro(Dinossauro *dino, Meteoro *meteoro) {
    int dano_base = 1; 
    int dano_real = dano_base; // Dano mínimo causado pelo dinossauro sem bônus.

    // 1. CALCULA O DANO REAL BASEADO NO BÔNUS
    
    // Se o meteoro for de Nível 1, o dano base de 1 já é suficiente (se vida_maxima = 1).
    // Apenas aplica bônus de dano para Níveis 2 e 3.
    if (meteoro->nivel > 1) { 
        if (dino->poder_bonus == 1) { // Bônus Amarelo
            // Reduz o número de cabeceios para Nível 2 e 3
            dano_real = 2; 
        } else if (dino->poder_bonus == 2) { // Bônus Vermelho
            // Destrói instantaneamente Nível 2 e 3
            dano_real = meteoro->vida_atual; // Causa dano igual à vida restante (kill shot)
        }
    }
    
    // 2. APLICA O BOUNCE (RECUA O METEORO)
    
    // O bounce deve ocorrer SOMENTE se o meteoro sobreviver ao dano.
    // Se a vida atual for maior que o dano, ele sobrevive e recua.
    if (meteoro->vida_atual > dano_real) {
        meteoro->velocidade_y = FORCA_IMPACTO_DESCENDENTE;
        // Aplica o Recuo Vertical. A constante DEVE ser um valor negativo alto (ex: -200.0)
        meteoro->y += METEORO_BOUNCE_DISPLACEMENT;
        //meteoro->velocidade_y = 2;
        
        printf("Meteoro Nível %d cabeceado. Restam %d hits.\n", meteoro->nivel, meteoro->vida_atual - dano_real);
    }

    // 3. APLICA O DANO E CHECA DESTRUIÇÃO
    
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
    // Se a vida for maior que o dano (meteoro sobreviveu), a mensagem já foi impressa no bloco "if (meteoro->vida_atual > dano_real)".
}

// -----------------------------------------------------------------------------
// FUNÇÃO DE INICIALIZAÇÃO E LOOP PRINCIPAL
// -----------------------------------------------------------------------------

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


    display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        fprintf(stderr, "Falha ao criar o display.\n");
        return 0;
    }

    /*/ Carregar fontes (necessário ter um arquivo .ttf, ex: 'arial.ttf')
    // Substitua 'arial.ttf' pelo caminho da sua fonte, ou use al_create_builtin_font()
    //fonte_titulo = al_load_font("arial.ttf", 36, 0); 
    f//onte_menu = al_load_font("arial.ttf", 24, 0);
    
    if (!fonte_titulo || !fonte_menu) {
        fprintf(stderr, "Aviso: Falha ao carregar fontes. Usando a fonte padrao.\n");
        // Se falhar, tenta usar a fonte embutida
        fonte_titulo = al_create_builtin_font();
        fonte_menu = al_create_builtin_font();
    }*/

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

// main.c (Ou onde você define suas funções de inicialização)

void inicializar_novo_jogo(Dinossauro *dino, Meteoro meteoros[], Item itens[]) {
    // 1. Inicializa o dinossauro (vida, posição, etc.)
    dinossauro_inicializar(dino, LARGURA_TELA / 4.0);
    
    // 2. Garante que os valores de score e fase estão zerados
    dino->score = 0;
    dino->fase_atual = 1;
    dino->tempo_fase = 0.0;
    dino->vida = 100;
    
    // 3. Reinicializa os elementos de jogo (assumindo que você tem essas funções)
    inicializar_meteoros(meteoros); 
    inicializar_itens(itens); 

    // 4. Garante que a próxima vez que voltar ao menu, pedirá o nome novamente.
    modo_edicao_nome = 1; // Opcional, mas útil se você quiser que o nome seja re-digitado
}

int main() {
    if (!inicializar_allegro()) {
        desinicializar_allegro();
        return -1;
    }

    // ... dentro da função main, declare estas variáveis:
    ALLEGRO_TIMER *timer = NULL;
    // ... e sua instância do dinossauro
    Dinossauro dino;

    Meteoro meteoros[MAX_METEOROS];

    // Inicializar Timer
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "Falha ao criar o timer.\n");
        // ... (tratar erro, desinicializar e retornar) ...
    }

    Item itens[MAX_ITENS]; // Array de itens
    double ultimo_tempo_item = 0.0; // Para controlar a geração periódica de itens
    // Inicializar Dinossauro
    dinossauro_inicializar(&dino, LARGURA_TELA / 4);

    // Inicializar Meteoros
    inicializar_meteoros(meteoros);
    inicializar_itens(itens);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    
    // 3. Registrar o Timer na Fila de Eventos (EVENT_QUEUE)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer)); // <-- REGISTRO DO TIMER

    // 4. Iniciar o Timer
    al_start_timer(timer);

    int rodando = 1;
    int redesenhar = 1; // Flag para controlar quando desenhar a tela
    ALLEGRO_EVENT ev;

    while (rodando) {
        al_wait_for_event(event_queue, &ev);

        if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            if (estado_atual == ESTADO_MELHORES_SCORES) {
                estado_atual = ESTADO_MENU_PRINCIPAL; 
                modo_edicao_nome = 1; // Permite re-entrada de nome
            }
        }

        // ----------------------------------------
        // A. Processamento de Eventos
        // ----------------------------------------
        
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
                        dino.dx = -VELOCIDADE_X; // Mover para a esquerda
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        dino.dx = VELOCIDADE_X; // Mover para a direita
                        break;
                }
            }
        } 
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) { // NOVO BLOCO: PARAR MOVIMENTO
            if (estado_atual == ESTADO_JOGO_ROLANDO) {
                // Zera o dx se a tecla de movimento for solta
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_LEFT:
                        // Só para se a velocidade estiver negativa (esquerda)
                        if (dino.dx < 0) dino.dx = 0;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        // Só para se a velocidade estiver positiva (direita)
                        if (dino.dx > 0) dino.dx = 0;
                        break;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (estado_atual == ESTADO_INPUT_NOME) { // Adicione esta verificação de estado
                gerenciar_input_menu(&ev, &dino, meteoros, itens);
            }

        } else if (ev.type == ALLEGRO_EVENT_TIMER) {
            // ----------------------------------------
            // B. Lógica de Atualização (ACIONADA PELO TIMER)
            // ----------------------------------------
            double tempo_atual = al_get_time();
            int score_limite;
            // Verifique se a condição abaixo está correta e se o tempo está passando.
            
            if (estado_atual == ESTADO_JOGO_ROLANDO) {
                score_limite = dino.fase_atual * PONTOS_POR_FASE;
                if (tempo_atual - ultimo_tempo_meteoro > (1.5 + (rand() % 150) / 100.0) ) { 
                    gerar_novo_meteoro(meteoros, dificuldade_fator);
                    ultimo_tempo_meteoro = tempo_atual;
                }
                if (dino.score >= score_limite) {
                    aumentar_dificuldade(&dino);
                }                
                // Chama a lógica de movimento do Dinossauro e aplica a gravidade
                // 1. Atualizar Dinossauro e Meteoros
                dinossauro_atualizar(&dino);
                atualizar_meteoros(meteoros);

                if (dino.vida <= 0) {                    
                    // 1. Salva o score e verifica se ele entra no Top 10
                    atualizar_e_salvar_score(nome_jogador, dino.score, dino.fase_atual);
                    
                    // 2. Carrega o ranking ATUALIZADO para exibição
                    carregar_scores(top_scores);
                    
                    // 3. Mude o estado para a tela de scores
                    estado_atual = ESTADO_MELHORES_SCORES;

                    redesenhar = 1;
                }

                if (dino.tempo_bonus > 0) {
                    dino.tempo_bonus -= (1.0 / FPS); // Decrementa o tempo restante
                    if (dino.tempo_bonus <= 0) {
                        dino.poder_bonus = 0; // Remove o bônus
                        printf("Bônus expirou. Dinossauro normal.\n");
                    }
                }

                atualizar_itens(itens);

                if (tempo_atual - ultimo_tempo_item > TEMPO_ENTRE_GERACAO_ITENS) {
                    // Decide qual bônus gerar (30% chance de Vida, 70% chance de Bônus de Poder)
                    TipoItem item_a_gerar;
                    int chance = rand() % 100;
                    
                    if (chance < 30) {
                        item_a_gerar = ITEM_VIDA;
                    } else if (chance < 70) {
                        item_a_gerar = ITEM_BONUS_1; // Amarelo
                    } else {
                        item_a_gerar = ITEM_BONUS_2; // Vermelho
                    }
                    
                    gerar_item(itens, item_a_gerar);
                    ultimo_tempo_item = tempo_atual;
                }

                // 4. Colisão de Coleta de Itens
                checar_coleta_item(&dino, itens);

                int indice_meteoro_atingido = checar_colisao_com_cabeca(&dino, meteoros);
                        
                if (indice_meteoro_atingido != -1) {
                    // Chama a função para calcular o dano do dinossauro
                    tratar_dano_meteoro(&dino, &meteoros[indice_meteoro_atingido]); 
                }

                // 2. Tratar Colisão com o Chão
                int dano_chao = checar_e_tratar_chao(meteoros);
                if (dano_chao > 0) {
                    dino.vida -= dano_chao;
                    if (dino.vida <= 0) {
                        // Tratar Game Over
                    }
                }

                score_limite = dino.fase_atual * PONTOS_POR_FASE;
                if (dino.score >= score_limite) {
                    aumentar_dificuldade(&dino);
                    redesenhar = 1; // Força a renderização imediata da tela de transição
                }
            } 
            
            else if (estado_atual == ESTADO_TRANSICAO_FASE) {
                // Diminui o timer pelo tempo do frame (1/FPS)
                transicao_timer -= 1.0 / FPS; 
                
                if (transicao_timer <= 0.0) {
                    estado_atual = ESTADO_JOGO_ROLANDO; // Transição acabou, volta ao jogo!
                }
            }
            redesenhar = 1; // Sinaliza que o desenho deve ocorrer
        } 

        // ----------------------------------------
        // C. Lógica de Renderização (Desenho)
        // ----------------------------------------
        
        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = 0; // Reseta a flag para esperar o próximo evento TIMER

            if (estado_atual == ESTADO_MENU_PRINCIPAL) {
                desenhar_menu_principal();
                
            } else if (estado_atual == ESTADO_INPUT_NOME) { // NOVO ESTADO
                desenhar_input_nome();
            }
            
            else if (estado_atual == ESTADO_JOGO_ROLANDO) {
                al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto
                
                // Desenha o chão e o dinossauro
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

                // 3. Nome do Jogador (Centro Superior)
                al_draw_textf(fonte_menu, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 10, ALLEGRO_ALIGN_CENTER, "JOGADOR: %s", nome_jogador);
                
            } 
            
            else if (estado_atual == ESTADO_TRANSICAO_FASE) {
                // Renderiza a sobreposição da tela de transição
                renderizar_transicao_fase(fonte_menu, dino.fase_atual, dificuldade_fator);
            }

            else if (estado_atual == ESTADO_MELHORES_SCORES) {
                al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto
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