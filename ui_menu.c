#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <synchapi.h>

#include "ui_menu.h"
#include "jogo.h"
#include "analise.h"
#include "historico.h"
#include "text_utils.h"

enum MenuOption
{
    MENU_PLAY = 0,
    MENU_HISTORY,
    MENU_EXIT,
    MENU_COUNT
};

static const char *MENU_ITEMS[MENU_COUNT] = {
    "Jogar",
    "Historico",
    "Sair"};

static const int CONSOLE_WIDTH = 120;
static const int MENU_ITEM_WIDTH = 24;
static const int MENU_Y = 8;
static const int MAX_SESSOES = 200;
static const int MAX_TABLE_ROWS = 8;
static const int TABLE_GAP = 2;

static const int KEY_UP = -1;
static const int KEY_DOWN = 1;
static const int KEY_ENTER = 0;
static const int KEY_NONE = 99;

static void print_cell(const char *text, int width, int alignment, const char *color)
{
    char buf[128];
    const char *src = text ? text : "";
    size_t len = strlen(src);

    if ((int)len > width)
    {
        int copy_len = width < (int)sizeof(buf) - 1 ? width : (int)sizeof(buf) - 1;
        memcpy(buf, src, (size_t)copy_len);
        buf[copy_len] = '\0';
        src = buf;
    }

    if (!color)
        color = WHITE;
    fputs(color, stdout);
    if (alignment == ALIGN_RIGHT)
        printf("%*s", width, src);
    else
        printf("%-*s", width, src);
    fputs(RESET, stdout);
}

static void print_hyphen_row(int cols, const int widths[], const char *color)
{
    if (!color)
        color = DARK_GRAY;
    fputs(color, stdout);
    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < widths[i]; ++j)
            putchar('-');
        if (i < cols - 1)
        {
            for (int k = 0; k < TABLE_GAP; ++k)
                putchar(' ');
        }
    }
    fputs(RESET, stdout);
}

static void draw_hyphen_table(int x, int y, int cols,
                              const char *headers[],
                              const int widths[],
                              int rows,
                              const char *data[rows][cols],
                              const int *alignments)
{
    gotoxy(x, y);
    for (int i = 0; i < cols; ++i)
    {
        int align = alignments ? alignments[i] : ALIGN_LEFT;
        print_cell(headers[i], widths[i], align, BOLD_WHITE);
        if (i < cols - 1)
            printf("%*s", TABLE_GAP, "");
    }

    gotoxy(x, y + 1);
    print_hyphen_row(cols, widths, DARK_GRAY);

    for (int r = 0; r < rows; ++r)
    {
        gotoxy(x, y + 2 + r);
        for (int c = 0; c < cols; ++c)
        {
            int align = alignments ? alignments[c] : ALIGN_LEFT;
            print_cell(data[r][c], widths[c], align, WHITE);
            if (c < cols - 1)
                printf("%*s", TABLE_GAP, "");
        }
    }
}

static int read_key_nonblocking(void)
{
    if (!_kbhit())
        return KEY_NONE;

    int ch = _getch();
    if (ch == 224 || ch == 0)
    {
        int ext = _getch();
        if (ext == 72)
            return KEY_UP;
        if (ext == 80)
            return KEY_DOWN;
    }

    if (ch == '\r')
        return KEY_ENTER;

    return KEY_NONE;
}

static void draw_menu_frame(void)
{
    int menu_x = (CONSOLE_WIDTH - MENU_ITEM_WIDTH) / 2;
    const char *hint = "Use as setas para navegar e Enter para selecionar.";
    int hint_x = (CONSOLE_WIDTH - (int)strlen(hint)) / 2;

    clear_screen();
    print_title_bar("LuckyGuess - Menu Principal", BOLD_BLUE, BOLD_WHITE);

    for (int i = 0; i < MENU_COUNT; ++i)
    {
        gotoxy(menu_x, MENU_Y + i * 2);
        printf("  %-20s  ", MENU_ITEMS[i]);
    }

    gotoxy(hint_x, MENU_Y + MENU_COUNT * 2 + 2);
    print_colored(hint, DARK_GRAY);
    fflush(stdout);
}

static void draw_menu_options(int selected)
{
    int menu_x = (CONSOLE_WIDTH - MENU_ITEM_WIDTH) / 2;

    for (int i = 0; i < MENU_COUNT; ++i)
    {
        gotoxy(menu_x, MENU_Y + i * 2);
        if (i == selected)
        {
            printf(BACKGROUND_WHITE BOLD_BLUE "  %-20s  " RESET, MENU_ITEMS[i]);
        }
        else
        {
            printf("  %-20s  ", MENU_ITEMS[i]);
        }
    }
    fflush(stdout);
}

static void wait_for_key(const char *message)
{
    if (!message)
        message = "Pressione qualquer tecla para voltar.";
    print_status_bar(message, BOLD_WHITE);
    _getch();
}

static void show_session_summary(Session s)
{
    clear_screen();
    print_title_bar("LuckyGuess - Resultado", BOLD_BLUE, BOLD_WHITE);

    const int cols = 5;
    const char *headers[] = {"Data/Hora", "Alvo", "Tentativas", "Baixo", "Alto"};
    const int alignments[] = {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_RIGHT, ALIGN_RIGHT, ALIGN_RIGHT};
    const int widths[] = {19, 6, 10, 6, 6};

    char buf_target[16];
    char buf_attempts[16];
    char buf_low[16];
    char buf_high[16];

    snprintf(buf_target, sizeof(buf_target), "%d", s.target);
    snprintf(buf_attempts, sizeof(buf_attempts), "%d", s.attempts_count);
    snprintf(buf_low, sizeof(buf_low), "%d", s.low_count);
    snprintf(buf_high, sizeof(buf_high), "%d", s.high_count);

    const char *data[1][5] = {{s.timestamp, buf_target, buf_attempts, buf_low, buf_high}};

    int table_width = 0;
    for (int i = 0; i < cols; ++i)
        table_width += widths[i];
    table_width += (cols - 1) * TABLE_GAP;

    int table_x = (CONSOLE_WIDTH - table_width) / 2 + 1;
    int table_y = 7;

    draw_hyphen_table(table_x, table_y, cols, headers, widths, 1, data, alignments);

    wait_for_key(NULL);
    clear_screen();
}

static void show_history_screen(void)
{
    clear_screen();
    print_title_bar("LuckyGuess - Historico", BOLD_BLUE, BOLD_WHITE);

    Session sessoes[MAX_SESSOES];
    int n = carregar_historico(sessoes, MAX_SESSOES);
    if (n <= 0)
    {
        wait_for_key("Nenhuma sessao encontrada.");
        clear_screen();
        return;
    }

    Stats st = processar_historico(sessoes, n);

    char info[128];
    snprintf(info, sizeof(info), "Sessoes: %d | Media: %.2f | Desvio: %.2f",
             n, st.average_attempts, st.stddev_attempts);
    gotoxy(4, 5);
    print_colored(info, BOLD_WHITE);

    const int cols = 5;
    const char *headers[] = {"Data/Hora", "Alvo", "Tentativas", "Baixo", "Alto"};
    const int alignments[] = {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_RIGHT, ALIGN_RIGHT, ALIGN_RIGHT};
    const int widths[] = {19, 6, 10, 6, 6};

    int rows = n < MAX_TABLE_ROWS ? n : MAX_TABLE_ROWS;
    int start = n - rows;

    const char *data[rows][5];
    char cells[rows][5][32];

    for (int i = 0; i < rows; ++i)
    {
        Session *s = &sessoes[start + i];
        snprintf(cells[i][0], sizeof(cells[i][0]), "%s", s->timestamp);
        snprintf(cells[i][1], sizeof(cells[i][1]), "%d", s->target);
        snprintf(cells[i][2], sizeof(cells[i][2]), "%d", s->attempts_count);
        snprintf(cells[i][3], sizeof(cells[i][3]), "%d", s->low_count);
        snprintf(cells[i][4], sizeof(cells[i][4]), "%d", s->high_count);

        for (int c = 0; c < cols; ++c)
        {
            data[i][c] = cells[i][c];
        }
    }

    int table_width = 0;
    for (int i = 0; i < cols; ++i)
        table_width += widths[i];
    table_width += (cols - 1) * TABLE_GAP;

    int table_x = (CONSOLE_WIDTH - table_width) / 2 + 1;
    int table_y = 8;

    draw_hyphen_table(table_x, table_y, cols, headers, widths, rows, data, alignments);

    wait_for_key(NULL);
    clear_screen();
}

int run_main_menu(void)
{
    int selected = 0;
    int last_selected = -1;

    draw_menu_frame();
    draw_menu_options(selected);

    while (1)
    {
        int key = read_key_nonblocking();
        if (key == KEY_UP)
        {
            selected = (selected - 1 + MENU_COUNT) % MENU_COUNT;
        }
        else if (key == KEY_DOWN)
        {
            selected = (selected + 1) % MENU_COUNT;
        }
        else if (key == KEY_ENTER)
        {
            if (selected == MENU_PLAY)
            {
                clear_screen();
                Session s = executar_partida();
                show_session_summary(s);
                draw_menu_frame();
                last_selected = -1;
            }
            else if (selected == MENU_HISTORY)
            {
                show_history_screen();
                draw_menu_frame();
                last_selected = -1;
            }
            else if (selected == MENU_EXIT)
            {
                clear_screen();
                return 0;
            }
        }

        if (selected != last_selected)
        {
            draw_menu_options(selected);
            last_selected = selected;
        }

        Sleep(16);
    }
}
