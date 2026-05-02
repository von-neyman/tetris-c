// tetris/gui/cli/tetris_frontend.c
/**
 * @file tetris_frontend.c
 * @brief Реализация терминального интерфейса игры
 *
 * Содержит функции инициализации ncurses, отрисовки игрового поля,
 * информационной панели, стартового экрана и экрана окончания игры.
 */

#include "tetris_frontend.h"

/**
 * @brief Инициализация библиотеки ncurses
 *
 * Устанавливает режимы:
 * - cbreak: отключение буферизации ввода (символы поступают сразу)
 * - noecho: не выводить нажатые клавиши на экран
 * - keypad: поддержка специальных клавиш (стрелки, F1 и т.д.)
 * - timeout(0): неблокирующий ввод
 * - curs_set(0): скрыть курсор
 */
void initiation_ncurses() {
  initscr();  // запуск curses-режима
  cbreak();  // отключает буферизацию ввода — символы поступают сразу
  noecho();  // не выводить нажатые клавиши на экран
  keypad(stdscr, TRUE);  // позволяет обрабатывать спецклавиши
  timeout(0);  // 0 = неблокирующий ввод (getch возвращает ERR если нет нажатия)
  curs_set(0);  // скрыть курсор
}

/**
 * @brief Проверка размера терминала
 * @return true если терминал достаточно велик (минимум 40x25), иначе false
 *
 * При несоответствии размера завершает curses-режим и выводит сообщение об
 * ошибке.
 */
bool terminal_size_is_correct() {
  bool result = true;
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);  // получить размеры экрана
  if (max_y < 25 || max_x < 40) {
    endwin();
    printf("Терминал слишком мал! Минимальный размер: 40x25\n");
    printf("Текущий размер: %dx%d\n", max_x, max_y);
    result = false;
  }
  return result;
}

/**
 * @brief Основной игровой цикл
 *
 * Создает окно для игрового поля, запускает цикл обработки ввода и отрисовки.
 * Цикл завершается при получении действия Terminate.
 */
void gameplay() {
  // Создание окна для игрового поля (20 + границы)
  WINDOW *game_window = newwin(22, 22, 1, 1);
  UserAction_t action = -1;
  GameInfo_t state = {0};
  // Главный игровой цикл
  while (action != Terminate) {
    action = get_action();
    userInput(action, false);
    state = updateCurrentState();
    if (state.pause == Screen_Start) {
      draw_game_start(game_window);
    } else if (state.pause == Screen_GameOver) {
      draw_game_over(game_window);
    } else {
      draw_field(game_window, state);
    }
    draw_info(state);
    refresh();  // обновляет экран
    napms(25);  // задержка 25 мс = 40 кадров/сек
  }
  delwin(game_window);  // удалить окно и освободить память
  endwin();             // завершить curses-режим
}

/**
 * @brief Получение действия пользователя
 * @return UserAction_t или -1 если клавиша не соответствует управлению
 *
 * Преобразует код нажатой клавиши в соответствующее действие пользователя.
 */
UserAction_t get_action() {
  UserAction_t action = -1;
  int ch = getch();
  switch (ch) {
    case BUTTON_START:
      action = Start;
      break;
    case BUTTON_PAUSE:
      action = Pause;
      break;
    case BUTTON_TERMINATE:
      action = Terminate;
      break;
    case BUTTON_LEFT:
      action = Left;
      break;
    case BUTTON_RIGHT:
      action = Right;
      break;
    case BUTTON_UP:
      action = Up;
      break;
    case BUTTON_DOWN:
      action = Down;
      break;
    case BUTTON_ACTION:
      action = Action;
      break;
    default:
      break;
  }
  return action;
}

/**
 * @brief Отрисовка игрового поля
 * @param game_window Указатель на окно ncurses
 * @param state Текущее состояние игры (поле, следующая фигура, счет, уровни и
 * т.д.)
 *
 * Очищает окно, рисует рамку, затем отображает все заполненные клетки.
 */
void draw_field(WINDOW *game_window, GameInfo_t state) {
  // Очищаем поле
  werase(game_window);  // очищаем виртуальное окно
  box(game_window, 0, 0);  // рисуем рамку в виртуальном окне
  // Отрисовка игрового поля с текущей фигурой в виртуальном окне
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (state.field[y][x]) {
        mvwprintw(game_window, y + 1, x * 2 + 1, "[]");
      }
    }
  }
  // Обновляем изменившиеся символы
  wrefresh(game_window);
}

/**
 * @brief Отрисовка информационной панели
 * @param state Текущее состояние игры
 *
 * Отображает Hi-score, Score, Level, Speed, следующую фигуру и индикатор паузы.
 */
void draw_info(GameInfo_t state) {
  // Очищаем область информации перед отрисовкой
  for (int y = 2; y <= 20; y++) {
    for (int x = 25; x < 25 + INFO_WIDTH; x++) {
      mvprintw(y, x, " ");
    }
  }
  int current_line = 2;
  // Hi-score надпись
  mvprintw(current_line, 25, "Hi-score");
  current_line += 2;
  // Hi-score значение
  mvprintw(current_line, 25, "%10d", state.high_score);
  current_line += 2;
  // Score надпись
  mvprintw(current_line, 25, "Score");
  current_line += 2;
  // Score значение
  mvprintw(current_line, 25, "%10d", state.score);
  current_line += 2;
  // Level надпись
  mvprintw(current_line, 25, "Level");
  // Level значение
  mvprintw(current_line, 30, "%5d", state.level);
  current_line += 2;
  // Speed надпись
  mvprintw(current_line, 25, "Speed");
  // Speed значение
  mvprintw(current_line, 30, "%5d", state.speed);
  current_line += 2;
  // Next надпись
  mvprintw(current_line, 25, "Next");
  current_line += 2;
  // Отрисовка следующей фигуры
  int offset = (INFO_WIDTH - 4 * 2) / 2;  // 4 - макс размер фигуры
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (state.next[y][x]) {
        mvprintw(current_line + y, 25 + offset + x * 2, "[]");
      }
    }
  }
  current_line += 4;
  // Надпись "Pause" если игра на паузе
  if (state.pause == Screen_Pause) {
    mvprintw(current_line, 25, "Pause");
  }
}

/**
 * @brief Отрисовка стартового экрана
 * @param game_window Указатель на окно ncurses
 *
 * Отображает логотип TETRIS и список управления.
 */
void draw_game_start(WINDOW *game_window) {
  // Очищаем поле
  werase(game_window);
  box(game_window, 0, 0);
  // Выводим TETRIS
  mvwprintw(game_window, 3, 5, "T E T R I S");
  // Выводим инструкцию
  mvwprintw(game_window, 7, 4, "Press start [s]");
  mvwprintw(game_window, 11, 7, "Control:");
  mvwprintw(game_window, 13, 2, "Start       [s]");
  mvwprintw(game_window, 14, 2, "Pause       [p]");
  mvwprintw(game_window, 15, 2, "Terminate   [q]");
  mvwprintw(game_window, 16, 2, "Left        [left]");
  mvwprintw(game_window, 17, 2, "Right       [right]");
  mvwprintw(game_window, 18, 2, "Down        [down]");
  mvwprintw(game_window, 19, 2, "Action      [ ]");
  // Обновляем изменившиеся символы
  wrefresh(game_window);
}

/**
 * @brief Отрисовка экрана окончания игры
 * @param game_window Указатель на окно ncurses
 *
 * Отображает сообщение "GAME OVER" и инструкцию по перезапуску.
 */
void draw_game_over(WINDOW *game_window) {
  // Очищаем поле
  werase(game_window);
  box(game_window, 0, 0);
  // Выводим GAME OVER
  mvwprintw(game_window, 9, 7, "GAME OVER");
  // Выводим инструкцию
  mvwprintw(game_window, 11, 5, "Press start [s]");
  mvwprintw(game_window, 12, 10, "or");
  mvwprintw(game_window, 13, 6, "terminate [q]");
  // Обновляем изменившиеся символы
  wrefresh(game_window);
}