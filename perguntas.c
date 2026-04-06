#include <stdio.h>
#include <string.h>
#include "perguntas.h"

static void copy_text(char *dst, size_t dst_size, const char *src)
{
    if (!dst || dst_size == 0)
        return;
    if (!src)
        src = "";
    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    {
        s[len - 1] = '\0';
        len--;
    }
}

int carregar_pergunta_por_id(int id, Pergunta *out)
{
    if (id <= 0 || !out)
        return 0;

    FILE *arquivo = fopen("perguntas.csv", "r");
    if (!arquivo)
        return 0;

    char linha[2048];
    int linha_atual = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        if (linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0')
            continue;

        linha_atual++;
        if (linha_atual != id)
            continue;

        char *fields[6];
        int count = 0;
        char *token = strtok(linha, ",");
        while (token && count < 6)
        {
            fields[count++] = token;
            token = strtok(NULL, ",");
        }

        fclose(arquivo);

        if (count < 6)
            return 0;

        trim_newline(fields[5]);
        copy_text(out->enunciado, sizeof(out->enunciado), fields[0]);
        copy_text(out->alt_a, sizeof(out->alt_a), fields[1]);
        copy_text(out->alt_b, sizeof(out->alt_b), fields[2]);
        copy_text(out->alt_c, sizeof(out->alt_c), fields[3]);
        copy_text(out->alt_d, sizeof(out->alt_d), fields[4]);
        out->correta = fields[5][0];
        return 1;
    }

    fclose(arquivo);
    return 0;
}
