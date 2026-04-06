#ifndef ANALISE_H
#define ANALISE_H
#include "tipos.h"

Stats processar_historico(Session sessoes[], int n);
int soma_tentativas_recursiva(Session sessoes[], int n);
double soma_quadrados_recursiva(Session sessoes[], int n, double media);

#endif