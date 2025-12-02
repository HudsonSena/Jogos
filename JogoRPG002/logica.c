// ===================================
// ARQUIVO: logica.c
// ===================================

#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include "dados.h" 

// --- DEFINIÇÃO/INICIALIZAÇÃO CONCRETA DE VARIÁVEIS GLOBAIS ---
// Definidas aqui. Declaradas 'extern' no dados.h.
Jogador jogador_principal;
JOGO_ESTADO estado_atual = ESTADO_MENU;
ALLEGRO_FONT *font_padrao = NULL; // É inicializada em main, mas declarada aqui
ALLEGRO_DISPLAY *display = NULL; 
ALLEGRO_EVENT_QUEUE *event_queue = NULL;

// Definições de Itens
Arma todas_as_armas[MAX_ARMAS] = {
    {1, "Espada de Treino", 10}, {2, "Cajado Magico", 25}, {3, "Machado de Guerra", 40}, {4, "Lanca Lendaria", 60}, {5, "Espada Final", 100}
};
Gema todas_as_gemas[MAX_GEMAS] = {
    {1, 20}, {2, 30}, {3, 50}, {4, 80}, {5, 120}
};

// --- TABULEIRO LINEAR (14 CASAS) ---
Casa tabuleiro[TAMANHO_TABULEIRO] = {
    {CASA_VAZIA, "Inicio da Aventura.", 0, {1}, 1, 0, 0},
    {CASA_BATALHA, "Goblin Fraco.", 15, {2}, 1, 0, 0}, 
    {CASA_BATALHA, "Orc Guerreiro.", 30, {3}, 1, 0, 0}, 
    {CASA_GEMA, "Encontrou 100 Moedas.", 100, {4}, 1, 0, 0},  
    {CASA_ARMA, "Encontrou Espada de Treino!", 0, {5}, 1, 1, 0}, 
    {CASA_BATALHA, "Troll de Rocha.", 50, {6}, 1, 0, 0}, 
    {CASA_VAZIA, "Descanso. Recupera 100 HP.", 100, {7}, 1, 0, 0},
    {CASA_ARMA, "Encontrou Cajado Magico!", 0, {8}, 1, 2, 0}, 
    {CASA_BATALHA, "Dragao Pequeno.", 90, {9}, 1, 0, 0}, 
    {CASA_ARMA, "Encontrou Escudo!", 0, {10}, 1, 3, 0}, 
    {CASA_GEMA, "Jackpot! 250 Moedas.", 250, {11}, 1, 0, 0}, 
    {CASA_BATALHA, "Chefe Final! Grande Demonio.", 120, {12}, 1, 0, 0}, 
    {CASA_VAZIA, "Ultimo Ponto de Descanço.", 0, {13}, 1, 0, 0},
    {CASA_CHEGADA, "VITORIA! Voce zerou o jogo.", 0, {-1}, 0, 0, 0} 
};

Objetivo objetivos[MAX_OBJETIVOS] = {
    {"Coletar 3 Armas", 3, 0, false},
    {"Acumular 3 Gemas", 3, 0, false}, 
    {"Chegar na Casa 14", 13, 0, false}
};


// --- IMPLEMENTAÇÕES DE FUNÇÕES DE LÓGICA ---

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

    if (pos >= TAMANHO_TABULEIRO - 1) { return; }
    
    jogador_principal.posicao_atual = casa->id_proximas_casas[0];
    
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
                printf("VITORIA! Ganha +5 de poder, 50 moedas.\n");
                jogador_principal.poder_ganho_batalha += 5;
                jogador_principal.vida += 20; if (jogador_principal.vida > MAX_VIDA) jogador_principal.vida = MAX_VIDA;
                jogador_principal.moedas += 50;
                
                int id_proxima_gema = jogador_principal.num_gemas_inventario + 1;
                if (id_proxima_gema <= MAX_GEMAS) {
                    jogador_principal.gemas_inventario[jogador_principal.num_gemas_inventario] = todas_as_gemas[id_proxima_gema - 1];
                    jogador_principal.num_gemas_inventario++;
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
            printf("GEMA COLETADA! Ganhou %d moedas.\n", casa->valor_evento);
            jogador_principal.posicao_atual++; 
            break;
        
        case CASA_ARMA: {
            int id_nova_arma = casa->id_arma_recompensa;
            bool ja_possui = false;
            for (int i = 0; i < jogador_principal.num_armas_inventario; i++) {
                if (jogador_principal.armas_inventario[i].id == id_nova_arma) { ja_possui = true; break; }
            }
            if (!ja_possui) {
                Arma nova_arma = todas_as_armas[id_nova_arma - 1];
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
             jogador_principal.vida += casa->valor_evento;
             if (jogador_principal.vida > MAX_VIDA) jogador_principal.vida = MAX_VIDA;
             jogador_principal.posicao_atual++;
             break;
            
        case CASA_CHEGADA:
            printf("--- VITORIA FINAL ---\n");
            estado_atual = ESTADO_SAIR;
            break;
    }
    atualizar_objetivos();
}

// --- Implementação de funções de I/O de arquivo (salvar_jogo, carregar_jogo, carregar_high_scores) ---
// (Estas funções de I/O devem ser implementadas aqui)
void carregar_high_scores(HighScore scores[]) { /* ... */ }
void salvar_jogo(const JogoSalvo *jogo, int slot_index) { /* ... */ }
int carregar_jogo(JogoSalvo *jogo, int slot_index) { return 0; /* ... */ }