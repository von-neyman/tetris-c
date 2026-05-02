// tetris/brick_game/tetris/tetris_fsm.h
/**
 * @file tetris_fsm.h
 * @brief Заголовочный файл конечного автомата игры
 *
 * Содержит прототипы функций-обработчиков для каждого состояния
 * конечного автомата, а также функцию таймера автопадения.
 */

#ifndef TETRIS_FSM_H
#define TETRIS_FSM_H

#include "tetris.h"
#include "tetris_backend.h"

/**
 * @brief Таймер автопадения
 *
 * Увеличивает drop_counter в состоянии Moving.
 * При достижении drop_delay переводит игру в состояние Shifting.
 * В остальных состояниях сбрасывает drop_counter.
 */
void timer_shifting();

/**
 * @brief Обработчик состояния START
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в стартовом состоянии:
 * - Start → переход в состояние Spawn
 * - Terminate → переход в состояние GameOver
 */
void start_state(UserAction_t action);

/**
 * @brief Обработчик состояния SPAWN
 *
 * Создает новую фигуру (generation_tetromino).
 * При коллизии (фигура сразу упирается вверх) переходит в GameOver,
 * иначе — в состояние Moving.
 */
void spawn_state();

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
void moving_state(UserAction_t action);

/**
 * @brief Обработчик состояния SHIFTING (сдвиг по таймеру)
 *
 * Пытается сдвинуть фигуру вниз (move_down).
 * Если фигура зафиксировалась → переход в Attaching,
 * иначе → возврат в Moving.
 */
void shifting_state();

/**
 * @brief Обработчик состояния ATTACHING (проверка линий после фиксации)
 *
 * Выполняет проверку и удаление заполненных линий (check_lines),
 * начисляет очки, обновляет рекорд, уровень и скорость.
 * После обработки переходит в состояние Spawn.
 */
void attaching_state();

/**
 * @brief Обработчик состояния PAUSE
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в состоянии паузы:
 * - Pause: снятие паузы, переход в Moving
 * - Terminate: переход в GameOver
 */
void pause_state(UserAction_t action);

/**
 * @brief Обработчик состояния GAMEOVER
 * @param action Действие пользователя
 *
 * Обрабатывает нажатия в состоянии окончания игры:
 * - Start: переход в Start (ожидание новой игры)
 * - Terminate: завершение программы (обрабатывается в main)
 */
void gameover_state(UserAction_t action);

#endif