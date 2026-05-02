// tetris/brick_game/tetris/tetris_fsm.c
/**
 * @file tetris_fsm.c
 * @brief Реализация конечного автомата игры
 *
 * Содержит обработчики для каждого состояния:
 * - timer_shifting: управление автоматическим падением по таймеру
 * - start_state: ожидание начала игры
 * - spawn_state: создание новой фигуры
 * - moving_state: обработка ввода во время движения
 * - shifting_state: сдвиг фигуры по таймеру
 * - attaching_state: фиксация фигуры и проверка линий
 * - pause_state: управление паузой
 * - gameover_state: завершение игры
 */

#include "tetris_fsm.h"

/**
 * @brief Таймер автопадения
 *
 * Увеличивает drop_counter в состоянии Moving.
 * При достижении drop_delay переводит игру в состояние Shifting.
 * В остальных состояниях сбрасывает drop_counter.
 */
void timer_shifting() {
  if (game.state == GameState_Moving) {
    game.drop_counter++;
    if (game.drop_counter >= game.drop_delay) {
      game.drop_counter = 0;
      game.state = GameState_Shifting;
    }
  } else {
    game.drop_counter = 0;
  }
}

/**
 * @brief Обработчик состояния START
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в стартовом состоянии:
 * - Start → переход в состояние Spawn
 * - Terminate → переход в состояние GameOver
 */
void start_state(UserAction_t action) {
  switch (action) {
    case Start:
      start_game();
      break;
    case Terminate:
      game.state = GameState_GameOver;
      break;
    default:
      break;
  }
}

/**
 * @brief Обработчик состояния SPAWN
 *
 * Создает новую фигуру (generation_tetromino).
 * При коллизии (фигура сразу упирается вверх) переходит в GameOver,
 * иначе — в состояние Moving.
 */
void spawn_state() {
  // Создаем новую фигуру
  generation_tetromino();
  // Проверяем, не закончилась ли игра
  if (check_collision()) {
    game.state = GameState_GameOver;
  } else {
    game.state = GameState_Moving;
  }
}

/**
 * @brief Обработчик состояния MOVING
 * @param action Действие пользователя
 *
 * Обрабатывает ввод в основном игровом состоянии:
 * - Left/Right: движение фигуры
 * - Down: ускоренное падение до упора, переход в Attaching
 * - Action: поворот фигуры
 * - Pause: переход в состояние Pause
 * - Terminate: переход в состояние GameOver
 */
void moving_state(UserAction_t action) {
  switch (action) {
    case Left:
      move_left();
      break;
    case Right:
      move_right();
      break;
    case Down:
      while (!move_down()) {
      }
      game.state = GameState_Attaching;
      break;
    case Action:
      rotate_tetromino();
      break;
    case Pause:
      game.pause = Screen_Pause;
      game.state = GameState_Pause;
      break;
    case Terminate:
      game.state = GameState_GameOver;
      break;
    default:
      break;
  }
}

/**
 * @brief Обработчик состояния SHIFTING (сдвиг по таймеру)
 *
 * Пытается сдвинуть фигуру вниз (move_down).
 * Если фигура зафиксировалась → переход в Attaching,
 * иначе → возврат в Moving.
 */
void shifting_state() {
  // Пытаемся сдвинуть фигуру вниз
  if (move_down()) {
    // Фигура зафиксировалась - переходим в Attaching
    game.state = GameState_Attaching;
  } else {
    // Успешно сдвинулись - возвращаемся в Moving
    game.state = GameState_Moving;
  }
}

/**
 * @brief Обработчик состояния ATTACHING (проверка линий после фиксации)
 *
 * Выполняет проверку и удаление заполненных линий (check_lines),
 * начисляет очки, обновляет рекорд, уровень и скорость.
 * После обработки переходит в состояние Spawn.
 */
void attaching_state() {
  int lines_cleared = check_lines();
  if (lines_cleared > 0) {
    // Начисление очков
    switch (lines_cleared) {
      case 1:
        game.score += LINES_1_SCORE;
        break;
      case 2:
        game.score += LINES_2_SCORE;
        break;
      case 3:
        game.score += LINES_3_SCORE;
        break;
      case 4:
        game.score += LINES_4_SCORE;
        break;
    }
    // Обновляем рекорд если нужно
    if (game.score > game.high_score) {
      game.high_score = game.score;
      save_high_score(game.high_score);
    }
    // Увеличиваем уровень и скорость, но не более MAX_LEVEL
    if (game.level < MAX_LEVEL) {
      int new_level = 1 + game.score / SCORE_PER_LEVEL;
      if (new_level > MAX_LEVEL) new_level = MAX_LEVEL;
      if (new_level > game.level) {
        game.level = new_level;
        game.speed = game.level;
        // Увеличиваем скорость с увеличением уровня (5% за уровень)
        game.drop_delay =
            INITIAL_DELAY * (100 - (game.speed - 1) * DELAY_REDUCTION) / 100;
      }
    }
  }
  game.state = GameState_Spawn;
}

/**
 * @brief Обработчик состояния PAUSE
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в состоянии паузы:
 * - Pause: снятие паузы, переход в Moving
 * - Terminate: переход в GameOver
 */
void pause_state(UserAction_t action) {
  switch (action) {
    case Pause:
      game.pause = Screen_Game;
      game.state = GameState_Moving;
      break;
    case Terminate:
      game.state = GameState_GameOver;
      break;
    default:
      break;
  }
}

/**
 * @brief Обработчик состояния GAMEOVER
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в состоянии окончания игры:
 * - Start: переход в Start (ожидание новой игры)
 */
void gameover_state(UserAction_t action) {
  switch (action) {
    case Start:
      game.state = GameState_Start;
      break;
    default:
      break;
  }
}