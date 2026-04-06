#include "analise.h"
#include <math.h>

// Soma recursiva: Caso base n == 0 [cite: 51, 179]
int soma_tentativas_recursiva(Session sessoes[], int n)
{
    if (n <= 0)
        return 0;
    return sessoes[n - 1].attempts_count + soma_tentativas_recursiva(sessoes, n - 1);
}

// Soma dos quadrados para o Desvio Padrão [cite: 161, 182]
double soma_quadrados_recursiva(Session sessoes[], int n, double media)
{
    if (n <= 0)
        return 0.0;
    double diff = sessoes[n - 1].attempts_count - media;
    return (diff * diff) + soma_quadrados_recursiva(sessoes, n - 1, media);
}

Stats processar_historico(Session sessoes[], int n)
{
    Stats st = {0};
    if (n <= 0)
        return st;

    st.average_attempts = (double)soma_tentativas_recursiva(sessoes, n) / n;
    double soma_q = soma_quadrados_recursiva(sessoes, n, st.average_attempts);
    st.stddev_attempts = sqrt(soma_q / n);
    return st;
}