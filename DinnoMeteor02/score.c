// score.c

#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// FUNÇÃO DE COMPARAÇÃO (Para ordenar do maior Score para o menor)
// -----------------------------------------------------------------------------

int comparar_scores(const void *a, const void *b) {
    ScoreEntry *scoreA = (ScoreEntry *)a;
    ScoreEntry *scoreB = (ScoreEntry *)b;
    
    // Retorna b->score - a->score para obter ordem decrescente (maior score primeiro)
    return scoreB->score - scoreA->score;
}

// -----------------------------------------------------------------------------
// 1. CARREGAR SCORES DO ARQUIVO
// -----------------------------------------------------------------------------

void carregar_scores(ScoreEntry top_scores[]) {
    // Tenta abrir o arquivo em modo binário de leitura
    FILE *file = fopen("scores.dat", "rb"); 
    
    if (file == NULL) {
        // Se o arquivo não existir ou não puder ser aberto, inicializa o ranking com zeros
        for (int i = 0; i < MAX_SCORES; i++) {
            strcpy(top_scores[i].nome, "---");
            top_scores[i].score = 0;
            top_scores[i].fase_final = 0;
        }
        return;
    }
    
    // Lê o array de estruturas do arquivo
    // Nota: O arquivo deve conter exatamente MAX_SCORES * sizeof(ScoreEntry) bytes
    fread(top_scores, sizeof(ScoreEntry), MAX_SCORES, file);
    fclose(file);
}

// -----------------------------------------------------------------------------
// 2. ATUALIZAR E SALVAR NOVO SCORE
// -----------------------------------------------------------------------------

void atualizar_e_salvar_score(const char *nome_jogador, int novo_score, int fase_final) {
    ScoreEntry ranking_atual[MAX_SCORES];
    
    // 1. Carrega o ranking atual para a memória
    carregar_scores(ranking_atual);

    // 2. Verifica se o novo score é maior que o menor score no ranking (a última posição)
    if (novo_score > ranking_atual[MAX_SCORES - 1].score) {
        
        // Insere o novo score na última posição (que será sobrescrita)
        strcpy(ranking_atual[MAX_SCORES - 1].nome, nome_jogador);
        ranking_atual[MAX_SCORES - 1].score = novo_score;
        ranking_atual[MAX_SCORES - 1].fase_final = fase_final;

        // 3. Reordena o ranking
        // A função qsort reordena todo o array do maior para o menor
        qsort(ranking_atual, MAX_SCORES, sizeof(ScoreEntry), comparar_scores);

        // 4. Salva o novo ranking ordenado no arquivo
        FILE *file = fopen("scores.dat", "wb");
        if (file != NULL) {
            fwrite(ranking_atual, sizeof(ScoreEntry), MAX_SCORES, file);
            fclose(file);
        } else {
            fprintf(stderr, "Erro ao salvar o arquivo de scores!\n");
        }
    }
}