#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int comparar_scores(const void *a, const void *b) {
    ScoreEntry *scoreA = (ScoreEntry *)a;
    ScoreEntry *scoreB = (ScoreEntry *)b;

    return scoreB->score - scoreA->score;
}

void carregar_scores(ScoreEntry top_scores[]) {
    FILE *file = fopen("scores.dat", "rb"); 
    
    if (file == NULL) {
        for (int i = 0; i < MAX_SCORES; i++) {
            strcpy(top_scores[i].nome, "---");
            top_scores[i].score = 0;
            top_scores[i].fase_final = 0;
        }
        return;
    }

    fread(top_scores, sizeof(ScoreEntry), MAX_SCORES, file);
    fclose(file);
}

void atualizar_e_salvar_score(const char *nome_jogador, int novo_score, int fase_final) {
    ScoreEntry ranking_atual[MAX_SCORES];
    
    carregar_scores(ranking_atual);

    if (novo_score > ranking_atual[MAX_SCORES - 1].score) {
        strcpy(ranking_atual[MAX_SCORES - 1].nome, nome_jogador);
        ranking_atual[MAX_SCORES - 1].score = novo_score;
        ranking_atual[MAX_SCORES - 1].fase_final = fase_final;

        qsort(ranking_atual, MAX_SCORES, sizeof(ScoreEntry), comparar_scores);

        FILE *file = fopen("scores.dat", "wb");
        if (file != NULL) {
            fwrite(ranking_atual, sizeof(ScoreEntry), MAX_SCORES, file);
            fclose(file);
        } else {
            fprintf(stderr, "Erro ao salvar o arquivo de scores!\n");
        }
    }
}