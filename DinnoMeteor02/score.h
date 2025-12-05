// score.h

#ifndef SCORE_H
#define SCORE_H

#define MAX_NOME 30
#define MAX_SCORES 10

typedef struct {
    char nome[MAX_NOME];
    int score;
    int fase_final;
} ScoreEntry;

void carregar_scores(ScoreEntry top_scores[]);
void atualizar_e_salvar_score(const char *nome_jogador, int novo_score, int fase_final);

#endif // SCORE_H