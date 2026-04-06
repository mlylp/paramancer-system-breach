#ifndef HISTORICO_H
#define HISTORICO_H

#include "tipos.h"

#define ARQUIVO_NOME "dados/historico.csv"

// RF03 - Grava uma sessão no arquivo em formato CSV [cite: 27, 133]
void salvar_sessao(Session s);

// RF04 - Lê todas as sessões do arquivo para análise [cite: 35]
int carregar_historico(Session sessoes[], int max_sessoes);

#endif