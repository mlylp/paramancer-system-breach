#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "jogo.h"
#include "historico.h"
#include "sorteio.h"
#include "perguntas.h"
#include "text_utils.h"

static const int CONSOLE_WIDTH = 120;

static void clear_line(int y)
{
    gotoxy(1, y);
    for (int i = 0; i < CONSOLE_WIDTH; ++i)
        putchar(' ');
}

static void wait_for_enter(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
    ch = getchar();
    while (ch != '\n' && ch != EOF)
        ch = getchar();
}

static void clear_question_area(void)
{
    for (int y = 12; y <= 19; ++y)
        clear_line(y);
}

static int perguntar_para_dica(void)
{
    Pergunta p = {0};
    int id = sortearPergunta();
    if (id <= 0 || !carregar_pergunta_por_id(id, &p))
    {
        clear_line(12);
        gotoxy(4, 12);
        print_colored("Nao foi possivel carregar perguntas.", YELLOW);
        return 0;
    }

    clear_question_area();
    gotoxy(4, 12);
    print_colored(p.enunciado, WHITE);
    gotoxy(6, 14);
    printf("A) %s", p.alt_a);
    gotoxy(6, 15);
    printf("B) %s", p.alt_b);
    gotoxy(6, 16);
    printf("C) %s", p.alt_c);
    gotoxy(6, 17);
    printf("D) %s", p.alt_d);

    clear_line(19);
    gotoxy(4, 19);
    printf("Resposta (a/b/c/d): ");

    char resposta = '\0';
    if (scanf(" %c", &resposta) != 1)
        return 0;

    resposta = (char)tolower((unsigned char)resposta);
    char correta = (char)tolower((unsigned char)p.correta);
    return resposta == correta;
}

Session executar_partida()
{
    Session s = {0};
    s.target = sortearNumero(); // RF01 [cite: 19]

    clear_screen();
    print_title_bar("LuckyGuess - Jogo", BOLD_BLUE, BOLD_WHITE);

    // Simulação de timestamp simples [cite: 30]
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(s.timestamp, sizeof(s.timestamp), "%Y-%m-%d %H:%M:%S", &tm);

    gotoxy(4, 5);
    print_colored("Novo jogo iniciado! Tente adivinhar o numero.", WHITE);
    gotoxy(4, 6);
    print_colored("Digite um palpite entre 1 e 100.", DARK_GRAY);

    while (s.attempts_count < MAX_GUESSES)
    {
        int palpite;
        clear_line(8);
        gotoxy(4, 8);
        printf("Palpite (%d/%d): ", s.attempts_count + 1, MAX_GUESSES);
        scanf("%d", &palpite);

        if (palpite < 1 || palpite > 100)
        {
            clear_line(10);
            gotoxy(4, 10);
            print_colored("Palpite invalido. Tente novamente.", YELLOW);
            clear_question_area();
            continue; // RF02 [cite: 21]
        }

        s.guesses[s.attempts_count++] = palpite;

        if (palpite == s.target)
        {
            clear_line(10);
            gotoxy(4, 10);
            printf("Voce acertou, depois de %d tentativas.", s.attempts_count);
            clear_line(12);
            gotoxy(4, 12);
            print_colored("Pressione ENTER para ver as estatisticas...", DARK_GRAY);
            wait_for_enter();
            clear_question_area();
            break;
        }

        int acertou = perguntar_para_dica();
        clear_line(10);
        gotoxy(4, 10);
        if (acertou)
        {
            if (palpite < s.target)
            {
                print_colored("Muito baixo!", BRIGHT_CYAN);
                s.low_count++;
            }
            else
            {
                print_colored("Muito alto!", BRIGHT_MAGENTA);
                s.high_count++;
            }
        }
        else
        {
            print_colored("Resposta incorreta. Sem dica.", YELLOW);
        }

        clear_question_area();
    }

    salvar_sessao(s); // RF03 [cite: 27]
    return s;
}