#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "historico.h"

static void garantir_pasta_dados(void)
{
#ifdef _WIN32
    _mkdir("dados");
#else
    mkdir("dados", 0777);
#endif
}

void salvar_sessao(Session s)
{
    garantir_pasta_dados();
    // Modo "a" (append) abre para anexar. Se não existir, o C cria o arquivo. [cite: 133, 189]
    FILE *f = fopen(ARQUIVO_NOME, "a");
    if (f == NULL)
    {
        printf("Erro ao abrir arquivo para escrita!\n");
        return;
    }

    // Formato: timestamp;alvo;tentativas;baixos;altos;palpites_csv [cite: 123, 125]
    fprintf(f, "%s;%d;%d;%d;%d;", s.timestamp, s.target, s.attempts_count, s.low_count, s.high_count);

    for (int i = 0; i < s.attempts_count; i++)
    {
        fprintf(f, "%d%s", s.guesses[i], (i == s.attempts_count - 1) ? "" : ",");
    }
    fprintf(f, "\n");
    fclose(f);
}

int carregar_historico(Session sessoes[], int max_sessoes)
{
    garantir_pasta_dados();
    FILE *f = fopen(ARQUIVO_NOME, "r");
    if (f == NULL)
    {
        f = fopen(ARQUIVO_NOME, "a");
        if (f != NULL)
            fclose(f);
        return 0; // Se nao existe, apenas retorna 0 sessoes [cite: 188]
    }

    char linha[1024];
    int count = 0;

    while (fgets(linha, sizeof(linha), f) && count < max_sessoes)
    {
        Session *s = &sessoes[count];
        // Note: sscanf simplificado para o exemplo de estrutura CSV [cite: 133]
        char palpites_raw[512];
        sscanf(linha, "%[^;];%d;%d;%d;%d;%s",
               s->timestamp, &s->target, &s->attempts_count,
               &s->low_count, &s->high_count, palpites_raw);

        count++;
    }

    fclose(f);
    return count;
}