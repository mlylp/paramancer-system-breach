#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "jogo.h"
#include "historico.h"
#include "sorteio.h"
#include "perguntas.h"
#include "text_utils.h"

static const int CONSOLE_WIDTH = 120;
static const int INITIAL_GUESSES = 3;

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

static void print_centered_line(int y, const char *text, const char *color)
{
    if (!text)
        return;

    int len = (int)strlen(text);
    int x = (CONSOLE_WIDTH - len) / 2 + 1;
    if (x < 1)
        x = 1;

    gotoxy(x, y);
    print_colored(text, color);
}

static void draw_centered_banner(const char *text, const char *color)
{
    if (!text)
        return;

    int len = (int)strlen(text);
    int box_width = len + 12;
    if (box_width < 40)
        box_width = 40;
    if (box_width > CONSOLE_WIDTH - 2)
        box_width = CONSOLE_WIDTH - 2;

    int x = (CONSOLE_WIDTH - box_width) / 2 + 1;
    int y = 8;

    draw_double_box(x, y, box_width, 5, color);
    gotoxy(x + (box_width - len) / 2, y + 2);
    print_colored(text, color);
}

static void show_end_screen(const Session *s, int venceu)
{
    clear_screen();
    print_title_bar("LuckyGuess - Resultado", BOLD_BLUE, BOLD_WHITE);

    if (venceu)
    {
        draw_centered_banner("VOCE VENCEU", BOLD_YELLOW);
        char msg[96];
        snprintf(msg, sizeof(msg), "Voce acertou, depois de %d tentativas.", s->attempts_count);
        print_centered_line(14, msg, WHITE);
    }
    else
    {
        draw_centered_banner("GAME OVER", BOLD_RED);
        print_centered_line(14, "Voce esgotou suas chances de palpite.", WHITE);
    }

    print_centered_line(16, "Pressione ENTER para ver as estatisticas...", DARK_GRAY);
    wait_for_enter();
    clear_question_area();
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
    int remaining_guesses = INITIAL_GUESSES;
    int venceu = 0;
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

    while (remaining_guesses > 0 && s.attempts_count < MAX_GUESSES)
    {
        int palpite;
        clear_line(8);
        gotoxy(4, 8);
        printf("Palpite (tentativa %d, chances %d): ", s.attempts_count + 1, remaining_guesses);
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
        remaining_guesses--;

        if (palpite == s.target)
        {
            venceu = 1;
            break;
        }

        int acertou = perguntar_para_dica();
        clear_line(10);
        gotoxy(4, 10);

        if (palpite < s.target)
            s.low_count++;
        else
            s.high_count++;

        if (acertou)
        {
            remaining_guesses++;
            if (palpite < s.target)
                print_colored("Muito baixo!", BRIGHT_CYAN);
            else
                print_colored("Muito alto!", BRIGHT_MAGENTA);
        }
        else
        {
            print_colored("Resposta incorreta. Sem dica.", YELLOW);
        }

        clear_question_area();
    }

    show_end_screen(&s, venceu);

    salvar_sessao(s); // RF03 [cite: 27]
    return s;
}