// tetris/tests/test_tetris.c
#include <check.h>

#include "../brick_game/tetris/tetris.h"
#include "../brick_game/tetris/tetris_backend.h"

// ============================================
// Вспомогательные функции
// ============================================

// Заполнить линию
static void set_full_line(int y) {
  for (int x = 0; x < FIELD_WIDTH; x++) {
    game.field[y][x] = 1;
  }
}

// Обнулить файл рекорда
static void reset_high_score() {
  FILE *file = fopen(HIGH_SCORE_FILE, "w");
  if (file) {
    fprintf(file, "0");
    fclose(file);
  }
}

// ============================================
// Тесты
// ============================================

// Инициализация и проверка начального состояния игры
START_TEST(test_init) {
  initiation_game();
  ck_assert_int_eq(game.score, 0);
  ck_assert_int_eq(game.level, 1);
  ck_assert_int_eq(game.speed, 1);
  ck_assert_int_eq(game.drop_delay, INITIAL_DELAY);
  ck_assert_int_eq(game.drop_counter, 0);
  ck_assert_int_eq(game.pause, Screen_Game);
  ck_assert_int_eq(game.state, GameState_Start);
}
END_TEST

// Проверка движения фигуры влево
START_TEST(test_left) {
  userInput(Start, false);
  userInput(-1, false);
  int old_x = game.pos_x;
  userInput(Left, false);
  ck_assert_int_eq(game.pos_x, old_x - 1);
  game.state = GameState_Start;
}
END_TEST

// Проверка движения фигуры вправо
START_TEST(test_right) {
  userInput(Start, false);
  userInput(-1, false);
  int old_x = game.pos_x;
  userInput(Right, false);
  ck_assert_int_eq(game.pos_x, old_x + 1);
  game.state = GameState_Start;
}
END_TEST

// Проверка ускоренного падения фигуры
START_TEST(test_down) {
  userInput(Start, false);
  userInput(-1, false);
  int old_y = game.pos_y;
  userInput(Down, false);
  // Фигура должна упасть ниже, чем была
  ck_assert_int_gt(game.pos_y, old_y);
  // После падения должна быть зафиксирована
  ck_assert_int_eq(game.state, GameState_Attaching);
  game.state = GameState_Start;
}
END_TEST

// Проверка поворота фигуры
START_TEST(test_rotate) {
  userInput(Start, false);
  userInput(-1, false);
  Tetromino before = game.current;
  userInput(Action, false);
  userInput(-1, false);
  int changed = 0;
  for (int y = 0; y < 4 && !changed; y++) {
    for (int x = 0; x < 4 && !changed; x++) {
      if (before.shape[y][x] != game.current.shape[y][x]) changed = 1;
    }
  }
  ck_assert_int_eq(changed, 1);
  game.state = GameState_Start;
}
END_TEST

// Начисление очков - 100 очков
START_TEST(test_1_score) {
  userInput(Start, false);
  userInput(-1, false);
  set_full_line(FIELD_HEIGHT - 1);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.score, 100);
  game.state = GameState_Start;
}
END_TEST

// Начисление очков - 300 очков
START_TEST(test_2_score) {
  userInput(Start, false);
  userInput(-1, false);
  set_full_line(FIELD_HEIGHT - 1);
  set_full_line(FIELD_HEIGHT - 2);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.score, 300);
  game.state = GameState_Start;
}
END_TEST

// Начисление очков - 700 очков
START_TEST(test_3_score) {
  userInput(Start, false);
  userInput(-1, false);
  set_full_line(FIELD_HEIGHT - 1);
  set_full_line(FIELD_HEIGHT - 2);
  set_full_line(FIELD_HEIGHT - 3);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.score, 700);
  game.state = GameState_Start;
}
END_TEST

// Начисление очков - 1500 очков
START_TEST(test_4_score) {
  userInput(Start, false);
  userInput(-1, false);
  set_full_line(FIELD_HEIGHT - 1);
  set_full_line(FIELD_HEIGHT - 2);
  set_full_line(FIELD_HEIGHT - 3);
  set_full_line(FIELD_HEIGHT - 4);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.score, 1500);
  game.state = GameState_Start;
}
END_TEST

// Проверка повышения уровня (при 600 очков)
START_TEST(test_level_up) {
  userInput(Start, false);
  userInput(-1, false);
  game.score = 500;
  set_full_line(FIELD_HEIGHT - 1);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.level, 2);
  game.state = GameState_Start;
}
END_TEST

// Уровень не может превышать MAX_LEVEL (10)
START_TEST(test_level_max) {
  userInput(Start, false);
  userInput(-1, false);
  game.score = 999999;
  set_full_line(FIELD_HEIGHT - 1);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.level, MAX_LEVEL);
  game.state = GameState_Start;
}
END_TEST

// Проверка паузы
START_TEST(test_pause) {
  userInput(Start, false);
  userInput(-1, false);
  userInput(Pause, false);
  ck_assert_int_eq(game.pause, Screen_Pause);
  userInput(Pause, false);
  ck_assert_int_eq(game.pause, Screen_Game);
  game.state = GameState_Start;
  userInput(Start, false);
  userInput(-1, false);
  userInput(Pause, false);
  userInput(Start, false);
  ck_assert_int_eq(game.pause, Screen_Pause);
  userInput(Terminate, false);
  ck_assert_int_eq(game.state, GameState_GameOver);
  game.state = GameState_Start;
}
END_TEST

// Проверка завершения игры
START_TEST(test_terminate) {
  userInput(Start, false);
  userInput(-1, false);
  userInput(Terminate, false);
  ck_assert_int_eq(game.state, GameState_GameOver);
  userInput(Pause, false);
  ck_assert_int_eq(game.state, GameState_GameOver);
  userInput(Start, false);
  ck_assert_int_eq(game.state, GameState_Start);
  game.state = GameState_Start;
}
END_TEST

// Фигура достигает верха
START_TEST(test_game_over) {
  userInput(Start, false);
  // Заполняем поле до верха
  for (int y = 0; y < FIELD_HEIGHT - 1; y++) {
    set_full_line(y);
  }
  userInput(-1, false);
  ck_assert_int_eq(game.state, GameState_GameOver);
  game.state = GameState_Start;
}
END_TEST

// Сохранение рекорда при превышении
START_TEST(test_high_score) {
  reset_high_score();
  userInput(Start, false);
  userInput(-1, false);
  set_full_line(FIELD_HEIGHT - 1);
  userInput(Down, false);
  userInput(-1, false);
  ck_assert_int_eq(game.high_score, 100);
  game.state = GameState_Start;
  reset_high_score();
}
END_TEST

// Проверка updateCurrentState()
START_TEST(test_update_current_state) {
  GameInfo_t info = updateCurrentState();
  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);
  ck_assert_int_eq(info.score, 0);
  ck_assert_int_eq(info.high_score, 0);
  ck_assert_int_eq(info.level, 1);
  ck_assert_int_eq(info.speed, 1);
  ck_assert_int_eq(info.pause, Screen_Start);
  userInput(Start, false);
  userInput(-1, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, Screen_Game);
  userInput(Terminate, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, Screen_GameOver);
  game.state = GameState_Start;
}
END_TEST

// Проверка падения по таймеру
START_TEST(test_shifting) {
  userInput(Start, false);
  userInput(-1, false);
  game.drop_counter = game.drop_delay;
  userInput(-1, false);
  ck_assert_int_eq(game.state, GameState_Moving);
  game.state = GameState_Start;
}
END_TEST

// Проверка старта
START_TEST(test_start_terminate) {
  userInput(Down, false);
  ck_assert_int_eq(game.state, GameState_Start);
  userInput(Terminate, false);
  ck_assert_int_eq(game.state, GameState_GameOver);
  game.state = GameState_Start;
}
END_TEST

// Проверка случая, когда файл рекорда отсутствует
START_TEST(test_high_score_no_file) {
  remove(HIGH_SCORE_FILE);
  int high_score = load_high_score();
  ck_assert_int_eq(high_score, 0);
}
END_TEST

// ============================================
// Набор тестов (Suite)
// ============================================

// Функция создает и возвращает набор всех тестов
Suite *tetris_suite() {
  // Создаем набор
  Suite *s = suite_create("Tetris");
  // Создаем группу тестов
  TCase *tc_core = tcase_create("Core");
  // Добавляем тесты в группу
  tcase_add_test(tc_core, test_init);
  tcase_add_test(tc_core, test_left);
  tcase_add_test(tc_core, test_right);
  tcase_add_test(tc_core, test_down);
  tcase_add_test(tc_core, test_rotate);
  tcase_add_test(tc_core, test_1_score);
  tcase_add_test(tc_core, test_2_score);
  tcase_add_test(tc_core, test_3_score);
  tcase_add_test(tc_core, test_4_score);
  tcase_add_test(tc_core, test_level_up);
  tcase_add_test(tc_core, test_level_max);
  tcase_add_test(tc_core, test_pause);
  tcase_add_test(tc_core, test_terminate);
  tcase_add_test(tc_core, test_game_over);
  tcase_add_test(tc_core, test_high_score);
  tcase_add_test(tc_core, test_update_current_state);
  tcase_add_test(tc_core, test_shifting);
  tcase_add_test(tc_core, test_start_terminate);
  tcase_add_test(tc_core, test_high_score_no_file);
  // Добавляем группу в набор
  suite_add_tcase(s, tc_core);
  return s;
}

// ============================================
// Точка входа
// ============================================

int main() {
  // Создаем набор тестов
  Suite *s = tetris_suite();
  // Создаем runner для выполнения тестов
  SRunner *sr = srunner_create(s);
  // Запускаем все тесты
  // CK_VERBOSE — подробный вывод
  // CK_NORMAL — обычный вывод
  // CK_MINIMAL — минимальный вывод
  srunner_run_all(sr, CK_VERBOSE);
  // Получаем количество проваленных тестов
  int failed = srunner_ntests_failed(sr);
  // Освобождаем память
  srunner_free(sr);
  // Возвращаем 0, если все тесты прошли успешно
  return (failed == 0) ? 0 : 1;
}