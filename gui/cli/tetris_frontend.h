// tetris/gui/cli/tetris_frontend.h
/**
 * @file tetris_frontend.h
 * @brief Заголовочный файл интерфейса игры (терминальный клиент)
 *
 * Содержит определения констант для отрисовки, кодов клавиш,
 * прототипы функций отрисовки и обработки ввода.
 */

#ifndef TETRIS_FRONTEND_H
#define TETRIS_FRONTEND_H

#include <ncurses.h>
#include <stdbool.h>

#include "../../brick_game/tetris/tetris.h"

/**
 * @defgroup FrontendConstants Константы интерфейса
 * @{
 */

/** @brief Ширина игрового поля (количество клеток) */
#define FIELD_WIDTH 10
/** @brief Высота игрового поля (количество клеток) */
#define FIELD_HEIGHT 20

/** @brief Ширина информационной панели (в символах) */
#define INFO_WIDTH 10

/** @brief Клавиша начала игры */
#define BUTTON_START 's'
/** @brief Клавиша паузы */
#define BUTTON_PAUSE 'p'
/** @brief Клавиша завершения игры */
#define BUTTON_TERMINATE 'q'
/** @brief Клавиша движения влево */
#define BUTTON_LEFT KEY_LEFT
/** @brief Клавиша движения вправо */
#define BUTTON_RIGHT KEY_RIGHT
/** @brief Клавиша движения вверх (не используется) */
#define BUTTON_UP KEY_UP
/** @brief Клавиша ускоренного падения */
#define BUTTON_DOWN KEY_DOWN
/** @brief Клавиша поворота фигуры */
#define BUTTON_ACTION ' '

/** @} */

/**
 * @brief Типы экранов, передаваемые через поле pause структуры GameInfo_t
 *
 * Значения кодируются в GameInfo_t.pause для передачи состояния из библиотеки в
 * интерфейс.
 */
typedef enum {
  Screen_Game = 0,     ///< Игровой процесс (не на паузе)
  Screen_Pause = 1,    ///< Игра на паузе
  Screen_Start = 2,    ///< Стартовый экран
  Screen_GameOver = 3  ///< Экран окончания игры
} Screen_t;

/**
 * @brief Инициализация библиотеки ncurses
 *
 * Устанавливает режимы работы: cbreak, noecho, keypad, timeout(0), скрывает
 * курсор.
 */
void initiation_ncurses();

/**
 * @brief Проверка размера терминала
 * @return true если терминал достаточно велик (минимум 40x25), иначе false
 */
bool terminal_size_is_correct();

/**
 * @brief Основной игровой цикл
 *
 * Обрабатывает ввод, вызывает API библиотеки и отрисовывает игровое состояние.
 */
void gameplay();

/**
 * @brief Получение действия пользователя из нажатой клавиши
 * @return Действие пользователя (UserAction_t) или -1 если клавиша не
 * соответствует управлению
 */
UserAction_t get_action();

/**
 * @brief Отрисовка игрового поля
 * @param game_window Указатель на окно ncurses для отрисовки
 * @param state Текущее состояние игры (поле, следующая фигура, счет, уровни и
 * т.д.)
 */
void draw_field(WINDOW *game_window, GameInfo_t state);

/**
 * @brief Отрисовка информационной панели
 * @param state Текущее состояние игры (счет, рекорд, уровень, следующая фигура)
 */
void draw_info(GameInfo_t state);

/**
 * @brief Отрисовка стартового экрана
 * @param game_window Указатель на окно ncurses
 */
void draw_game_start(WINDOW *game_window);

/**
 * @brief Отрисовка экрана окончания игры
 * @param game_window Указатель на окно ncurses
 */
void draw_game_over(WINDOW *game_window);

#endif