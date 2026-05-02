// tetris/brick_game/tetris/tetris_backend.c
/**
 * @file tetris_backend.c
 * @brief Реализация игровой логики Тетриса
 *
 * Содержит все функции, отвечающие за механику игры:
 * движение фигур, проверка коллизий, удаление линий,
 * начисление очков, управление уровнем скорости.
 * Также хранит глобальное состояние игры и массив фигур.
 */

#include "tetris_backend.h"

/**
 * @brief Глобальная переменная состояния игры (определение)
 *
 * Содержит полную информацию о текущем состоянии игры:
 * поле, фигуры, счет, уровень, состояние КА и т.д.
 */
GameSession_t game;

/**
 * @brief Флаг, указывающий, была ли уже выполнена инициализация
 *
 * Используется в initiation_game() для однократной инициализации.
 */
static bool is_game_active = false;

/**
 * @brief Массив всех фигур (тетрамино)
 *
 * Содержит 7 стандартных фигур: I, J, L, O, S, T, Z.
 * Каждая фигура представлена матрицей 4x4 и фактическим размером.
 */
static Tetromino shapes[NUM_SHAPES] = {
    // I-фигура (палка)
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 4},
    // J-фигура
    {{{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 3},
    // L-фигура
    {{{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 3},
    // O-фигура (квадрат)
    {{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 2},
    // S-фигура
    {{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 3},
    // T-фигура
    {{{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 3},
    // Z-фигура
    {{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, 3}};

/**
 * @brief Инициализация игры (вызывается один раз при первом запуске)
 *
 * Устанавливает начальные значения: поле очищено, счет 0, уровень 1,
 * загружает рекорд из файла, состояние GameState_Start.
 */
void initiation_game() {
  if (!is_game_active) {
    clean_field();
    srand(time(NULL));
    game.high_score = load_high_score();
    game.score = 0;
    game.level = 1;
    game.speed = 1;
    game.drop_delay = INITIAL_DELAY;
    game.drop_counter = 0;
    game.pause = Screen_Game;
    game.state = GameState_Start;
    is_game_active = true;
  }
}

/**
 * @brief Старт новой игры
 *
 * Очищает поле, генерирует первую следующую фигуру,
 * сбрасывает счет, уровень и скорость, переводит игру в состояние Spawn.
 */
void start_game() {
  clean_field();
  game.next = shapes[rand() % NUM_SHAPES];
  game.score = 0;
  game.level = 1;
  game.speed = 1;
  game.drop_delay = INITIAL_DELAY;
  game.drop_counter = 0;
  game.pause = Screen_Game;
  game.state = GameState_Spawn;
}

/**
 * @brief Очистка игрового поля
 *
 * Заполняет все клетки поля нулями.
 */
void clean_field() {
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      game.field[y][x] = 0;
    }
  }
}

/**
 * @brief Движение фигуры влево
 *
 * Уменьшает pos_x на 1. При коллизии позиция восстанавливается.
 */
void move_left() {
  game.pos_x--;
  if (check_collision()) {
    game.pos_x++;  // Откат при коллизии
  }
}

/**
 * @brief Движение фигуры вправо
 *
 * Увеличивает pos_x на 1. При коллизии позиция восстанавливается.
 */
void move_right() {
  game.pos_x++;
  if (check_collision()) {
    game.pos_x--;  // Откат при коллизии
  }
}

/**
 * @brief Движение фигуры вниз (один шаг)
 * @return 1 если фигура зафиксировалась, иначе 0
 *
 * Увеличивает pos_y на 1. При коллизии откатывает позицию и фиксирует фигуру.
 */
int move_down() {
  int lock = 0;
  game.pos_y++;
  if (check_collision()) {
    // Если есть коллизия, откатываем движение
    game.pos_y--;
    // Фиксируем фигуру на поле
    lock_tetromino();
    lock = 1;
  }
  return lock;
}

/**
 * @brief Проверка столкновения текущей фигуры с полем или границами
 * @return 1 если есть коллизия, 0 если нет
 */
int check_collision() {
  int collision = 0;
  for (int y = 0; y < game.current.size && !collision; y++) {
    for (int x = 0; x < game.current.size && !collision; x++) {
      if (game.current.shape[y][x]) {
        // Вычисляем позицию в игровом поле
        int field_x = game.pos_x + x;
        int field_y = game.pos_y + y;
        // Проверяем выход за границы
        if (field_x < 0 || field_x >= FIELD_WIDTH || field_y >= FIELD_HEIGHT) {
          collision = 1;
        }
        // Проверяем пересечение с другими фигурами
        if (field_y >= 0 && game.field[field_y][field_x]) {
          collision = 1;
        }
      }
    }
  }
  return collision;
}

/**
 * @brief Поворот текущей фигуры по часовой стрелке
 *
 * Выполняет поворот матрицы shape. Если после поворота возникает коллизия,
 * поворот отменяется.
 */
void rotate_tetromino() {
  // Создаем временную копию фигуры
  Tetromino temp = game.current;
  // Выполняем поворот матрицы на 90 градусов по часовой стрелке
  for (int y = 0; y < game.current.size; y++) {
    for (int x = 0; x < game.current.size; x++) {
      game.current.shape[x][game.current.size - 1 - y] = temp.shape[y][x];
    }
  }
  // Проверяем коллизию после поворота
  if (check_collision()) {
    // Если есть коллизия, отменяем поворот
    game.current = temp;
  }
}

/**
 * @brief Фиксация текущей фигуры на поле
 *
 * Копирует блоки текущей фигуры в массив field.
 */
void lock_tetromino() {
  for (int y = 0; y < game.current.size; y++) {
    for (int x = 0; x < game.current.size; x++) {
      if (game.current.shape[y][x]) {
        int field_x = game.pos_x + x;
        int field_y = game.pos_y + y;
        // Записываем фигуру в поле
        if (field_y >= 0) {
          game.field[field_y][field_x] = 1;
        }
      }
    }
  }
}

/**
 * @brief Генерация новой текущей фигуры
 *
 * Текущая фигура заменяется на следующую, следующая генерируется случайным
 * образом. Позиция фигуры устанавливается в верхней части поля по центру.
 */
void generation_tetromino() {
  // Текущая фигура становится следующей
  game.current = game.next;
  // Генерируем новую следующую фигуру
  game.next = shapes[rand() % NUM_SHAPES];
  // Устанавливаем начальную позицию
  game.pos_x = FIELD_WIDTH / 2 - game.current.size / 2;
  game.pos_y = 0;
}

/**
 * @brief Проверка и удаление заполненных линий
 * @return Количество удаленных линий (0-4)
 *
 * Сканирует поле снизу вверх. При обнаружении полностью заполненной линии
 * удаляет её и сдвигает вышележащие линии вниз.
 */
int check_lines() {
  int lines_cleared = 0;
  for (int y = FIELD_HEIGHT - 1; y >= 0; y--) {
    int line_full = 1;
    // Проверяем, заполнена ли линия
    for (int x = 0; line_full && x < FIELD_WIDTH; x++) {
      if (game.field[y][x] == 0) {
        line_full = 0;
      }
    }
    // Если линия заполнена
    if (line_full) {
      lines_cleared++;
      // Сдвигаем все линии выше текущей вниз
      for (int new_y = y; new_y > 0; new_y--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          game.field[new_y][x] = game.field[new_y - 1][x];
        }
      }
      // Очищаем самую верхнюю линию
      for (int x = 0; x < FIELD_WIDTH; x++) {
        game.field[0][x] = 0;
      }
      // После сдвига нужно проверить эту же позицию снова
      y++;
    }
  }
  return lines_cleared;
}

/**
 * @brief Загрузка рекорда из файла
 * @return Значение рекорда (0 если файл не существует)
 *
 * При отсутствии файла создает его с записью 0.
 */
int load_high_score() {
  int high_score = 0;
  FILE *file = fopen(HIGH_SCORE_FILE, "r");
  if (file == NULL) {
    // Файл не существует, создаем с нулевым рекордом
    file = fopen(HIGH_SCORE_FILE, "w");
    if (file != NULL) {
      fprintf(file, "0");
      fclose(file);
    }
  } else {
    fscanf(file, "%d", &high_score);
    fclose(file);
  }
  return high_score;
}

/**
 * @brief Сохранение рекорда в файл
 * @param high_score Новое значение рекорда
 */
void save_high_score(int high_score) {
  FILE *file = fopen(HIGH_SCORE_FILE, "w");
  if (file == NULL) {
    // Не удалось открыть файл для записи
    return;
  }
  fprintf(file, "%d", high_score);
  fclose(file);
}