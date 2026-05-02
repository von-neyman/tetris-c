// tetris/brick_game/tetris/tetris_defines.h
/**
 * @file tetris_defines.h
 * @brief Константы игровой логики Тетриса
 *
 * Содержит все числовые и строковые константы, используемые в библиотеке.
 * Эти константы определяют размеры поля, механику начисления очков,
 * скорости и уровни.
 */

#ifndef TETRIS_DEFINES_H
#define TETRIS_DEFINES_H

/**
 * @defgroup GameConstants Игровые константы
 * @{
 */

/** @brief Ширина игрового поля (количество клеток) */
#define FIELD_WIDTH 10

/** @brief Высота игрового поля (количество клеток) */
#define FIELD_HEIGHT 20

/** @brief Количество типов фигур (тетрамино) */
#define NUM_SHAPES 7

/** @brief Начальная задержка падения фигуры (в кадрах) */
#define INITIAL_DELAY 20

/** @brief Максимальный уровень (скорость перестает расти после 10) */
#define MAX_LEVEL 10

/** @brief Путь к файлу для сохранения рекорда (относительно папки запуска) */
#define HIGH_SCORE_FILE "resources/high_score.txt"

/** @} */

/**
 * @defgroup Scoring Начисление очков
 * @{
 */

/** @brief Очки за 1 очищенную линию */
#define LINES_1_SCORE 100

/** @brief Очки за 2 очищенные линии */
#define LINES_2_SCORE 300

/** @brief Очки за 3 очищенные линии */
#define LINES_3_SCORE 700

/** @brief Очки за 4 очищенные линии (тетрис) */
#define LINES_4_SCORE 1500

/** @brief Количество очков для повышения уровня */
#define SCORE_PER_LEVEL 600

/** @} */

/**
 * @defgroup Speed Скорость падения
 * @{
 */

/** @brief Процент уменьшения задержки за каждый уровень (5% за уровень) */
#define DELAY_REDUCTION 5

/** @} */

#endif