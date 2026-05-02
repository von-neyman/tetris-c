// tetris/brick_game/tetris/tetris.c
/**
 * @file tetris.c
 * @brief Реализация публичного API библиотеки Тетрис
 *
 * Содержит функции userInput() и updateCurrentState(), которые являются
 * точками входа для интерфейса. Эти функции делегируют выполнение
 * обработчикам конечного автомата и формируют структуру GameInfo_t.
 */

#include "tetris_fsm.h"

/**
 * @brief Передача действия пользователя в библиотеку
 * @param action Действие пользователя (Start, Pause, Terminate, Left, Right,
 * Down, Action)
 * @param hold Флаг зажатия клавиши (не используется)
 *
 * Вызывается из интерфейса при каждом нажатии клавиши.
 * Функция инициализирует игру (при необходимости), обрабатывает таймер,
 * а затем передает управление соответствующему обработчику состояния КА.
 */
void userInput(UserAction_t action, bool hold) {
  (void)hold;
  initiation_game();
  timer_shifting();
  switch (game.state) {
    case GameState_Start:
      start_state(action);
      break;
    case GameState_Spawn:
      spawn_state();
      break;
    case GameState_Moving:
      moving_state(action);
      break;
    case GameState_Shifting:
      shifting_state();
      break;
    case GameState_Attaching:
      attaching_state();
      break;
    case GameState_Pause:
      pause_state(action);
      break;
    case GameState_GameOver:
      gameover_state(action);
      break;
  }
}

/**
 * @brief Получение текущего состояния игры для отрисовки
 * @return Структура GameInfo_t, содержащая игровое поле, следующую фигуру, счет
 * и т.д.
 *
 * Вызывается из интерфейса с постоянной частотой (каждый кадр).
 * Функция:
 * 1. Гарантирует инициализацию игры (вызов initiation_game)
 * 2. Формирует временное поле, объединяющее упавшие блоки и текущую фигуру
 * 3. Создает массивы указателей для передачи во внешний интерфейс
 * 4. Кодирует состояние КА в поле pause структуры GameInfo_t
 *
 * @note static переменные используются для того, чтобы возвращаемые указатели
 *       оставались валидными после выхода из функции.
 */
GameInfo_t updateCurrentState() {
  initiation_game();
  // static чтобы данные не уничтожались после выхода из функции
  static GameInfo_t info;
  static int *field_rows[FIELD_HEIGHT];
  static int *next_rows[4];
  // Временное поле для отрисовки
  static int temp_field[FIELD_HEIGHT][FIELD_WIDTH];
  // Копируем game.field во временное поле
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      temp_field[y][x] = game.field[y][x];
    }
  }
  // Накладываем текущую фигуру поверх
  for (int y = 0; y < game.current.size; y++) {
    for (int x = 0; x < game.current.size; x++) {
      if (game.current.shape[y][x]) {
        int field_x = game.pos_x + x;
        int field_y = game.pos_y + y;
        // Проверяем границы (на всякий случай, это перестраховка)
        if (field_y >= 0 && field_y < FIELD_HEIGHT && field_x >= 0 &&
            field_x < FIELD_WIDTH) {
          temp_field[field_y][field_x] = 1;
        }
      }
    }
  }
  // Заполняем field_rows указателями на строки temp_field
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    field_rows[i] = temp_field[i];
  }
  // Заполняем next_rows указателями на строки game.next.shape
  for (int i = 0; i < 4; i++) {
    next_rows[i] = game.next.shape[i];
  }
  info.field = field_rows;
  info.next = next_rows;
  info.score = game.score;
  info.high_score = game.high_score;
  info.level = game.level;
  info.speed = game.speed;
  if (game.state == GameState_Start) {
    info.pause = Screen_Start;  // стартовый экран
  } else if (game.state == GameState_GameOver) {
    info.pause = Screen_GameOver;  // game over
  } else {
    info.pause = game.pause;  // обычная пауза
  }
  return info;
}