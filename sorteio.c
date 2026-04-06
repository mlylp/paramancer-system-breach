#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sorteio.h"

static void inicializar_seed(void)
{
    static int seed_inicializada = 0;
    if (!seed_inicializada)
    {
        srand((unsigned int)time(NULL));
        seed_inicializada = 1;
    }
}

int sortearNumero(void)
{
    inicializar_seed();
    return (rand() % 100) + 1;
}

int sortearPergunta(void)
{
    inicializar_seed();

    FILE *arquivo = fopen("perguntas.csv", "r");
    if (arquivo == NULL)
        return -1;

    int total_linhas = 0;
    char linha[2048];

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        if (linha[0] != '\n' && linha[0] != '\r' && linha[0] != '\0')
        {
            total_linhas++;
        }
    }

    fclose(arquivo);

    if (total_linhas <= 0)
        return -1;

    return (rand() % total_linhas) + 1;
}
