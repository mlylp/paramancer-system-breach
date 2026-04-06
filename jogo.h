#ifndef JOGO_H
#define JOGO_H

#include "tipos.h"

/* * Inicia uma nova sessão de jogo.
 * Sorteia o número alvo, processa os palpites do utilizador e
 * retorna uma estrutura Session preenchida com os dados da partida.
 */
Session executar_partida();

/* * Valida se o palpite está dentro do intervalo permitido (1 a 100).
 * Retorna 1 se for válido, 0 caso contrário.
 */
int validar_palpite(int palpite);

/* * Exibe o resumo visual da sessão após o acerto (RF03).
 */
void exibir_resumo_sessao(Session s);

#endif