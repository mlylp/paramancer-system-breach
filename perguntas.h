#ifndef PERGUNTAS_H
#define PERGUNTAS_H

#define MAX_TEXTO_PERGUNTA 256
#define MAX_TEXTO_ALTERNATIVA 128

typedef struct
{
    char enunciado[MAX_TEXTO_PERGUNTA];
    char alt_a[MAX_TEXTO_ALTERNATIVA];
    char alt_b[MAX_TEXTO_ALTERNATIVA];
    char alt_c[MAX_TEXTO_ALTERNATIVA];
    char alt_d[MAX_TEXTO_ALTERNATIVA];
    char correta;
} Pergunta;

int carregar_pergunta_por_id(int id, Pergunta *out);

#endif
