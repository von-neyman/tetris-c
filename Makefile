# Компилятор (CC - C Compiler)
CC = gcc
# Флаги компиляции (CFLAGS - C FLAGS)
CFLAGS = -std=c11 -Wall -Werror -Wextra -pedantic
# Флаги линковки (LDFLAGS - Linker Director FLAGS)
LDFLAGS = -lncurses
# Флаги для check
CHECK_FLAGS = -lcheck -lsubunit -lm
# Флаги для gcov
GCOV_FLAGS = --coverage -fprofile-arcs -ftest-coverage

# Куда складываем объектные файлы и библиотеку
BUILD_DIR = output
# Куда устанавливаем игру по умолчанию (?= - условное присваивание), путь можно также указать вручную во время установки (make install INSTALL_DIR= другой путь)
INSTALL_DIR ?= games
# Где лежит библиотека
BACKEND_DIR = brick_game/tetris
# Где лежит интерфейс
FRONTEND_DIR = gui/cli
# Дистрибутив
DIST_DIR = distribution/tetris
DIST_ARCHIVE = distribution/tetris.tar.gz

# Объектные файлы библиотеки
BACKEND_OBJS = $(BUILD_DIR)/tetris_backend.o $(BUILD_DIR)/tetris_fsm.o $(BUILD_DIR)/tetris.o
# Статическая библиотека
LIBRARY = $(BUILD_DIR)/libtetris.a
# Объектные файлы интерфейса
FRONTEND_OBJS = $(BUILD_DIR)/tetris_frontend.o $(BUILD_DIR)/main.o

# Объектные файлы для тестовой библиотеки с gcov
BACKEND_OBJS_GCOV = $(BUILD_DIR)/tetris_backend_gcov.o $(BUILD_DIR)/tetris_fsm_gcov.o $(BUILD_DIR)/tetris_gcov.o
# Статическая библиотека для тестов с gcov
LIBRARY_TEST = $(BUILD_DIR)/libtetris_test.a

# Исполняемый файл (EXECutable)
EXEC = tetris

# Пути для include
INC_BACKEND = -I$(BACKEND_DIR)
INC_FRONTEND = -I$(FRONTEND_DIR)

# Цели, которые не создают файлы с таким именем
.PHONY: all clean install uninstall test gcov_report dvi dist rebuild format_check format_fix valgrind

# Сборка всего
all: $(EXEC)

# Сборка статической библиотеки. ar - архиватор (archive). rcs - replace, create, index (создать архив с индексацией)
$(LIBRARY): $(BACKEND_OBJS)
	ar rcs $@ $^

# Компиляция объектных файлов библиотеки
$(BUILD_DIR)/%.o: $(BACKEND_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_BACKEND) -c $< -o $@

# Компиляция файлов интерфейса
$(BUILD_DIR)/%.o: $(FRONTEND_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_FRONTEND) -c $< -o $@

# Сборка исполняемого файла
$(EXEC): $(LIBRARY) $(FRONTEND_OBJS)
	$(CC) $(FRONTEND_OBJS) -L$(BUILD_DIR) -ltetris -o $@ $(LDFLAGS)
	mkdir -p resources

# Компиляция объектных файлов библиотеки для тестов с gcov
$(BUILD_DIR)/tetris_backend_gcov.o: $(BACKEND_DIR)/tetris_backend.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INC_BACKEND) -c $< -o $@

$(BUILD_DIR)/tetris_fsm_gcov.o: $(BACKEND_DIR)/tetris_fsm.c
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INC_BACKEND) -c $< -o $@

$(BUILD_DIR)/tetris_gcov.o: $(BACKEND_DIR)/tetris.c
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(INC_BACKEND) -c $< -o $@

# Сборка статической библиотеки для тестов с gcov
$(LIBRARY_TEST): $(BACKEND_OBJS_GCOV)
	ar rcs $@ $^

# Тестирование
test: $(LIBRARY_TEST)
	mkdir -p resources
	$(CC) $(CFLAGS) -I$(BACKEND_DIR) tests/test_tetris.c $(LIBRARY_TEST) -o tests/test_tetris $(CHECK_FLAGS) -lgcov
	./tests/test_tetris

# Покрытие
gcov_report: test
	mkdir -p tests/gcov_report
	lcov -c -d $(BUILD_DIR) -o tests/gcov_report/coverage.info
	genhtml -o tests/gcov_report tests/gcov_report/coverage.info
	@echo "Отчет о покрытии: tests/gcov_report/index.html"

# Документация
dvi:
	doxygen Doxyfile
	@echo "Документация: documentation/html/index.html"

# Дистрибутив
dist: clean
	mkdir -p $(DIST_DIR)
	cp -r brick_game gui tests Makefile Doxyfile README.md documentation images $(DIST_DIR)/
	tar -czvf $(DIST_ARCHIVE) $(DIST_DIR)/
	rm -rf $(DIST_DIR)
	@echo "Дистрибутив: $(DIST_ARCHIVE)"

# Пересборка
rebuild: clean all

# Проверка стиля
format_check:
	clang-format -style=Google -n brick_game/tetris/*.c brick_game/tetris/*.h gui/cli/*.c gui/cli/*.h tests/*.c

# Автоматическое форматирование
format_fix:
	clang-format -style=Google -i brick_game/tetris/*.c brick_game/tetris/*.h gui/cli/*.c gui/cli/*.h tests/*.c

# Проверка утечек памяти
valgrind: test
	valgrind --leak-check=yes ./tests/test_tetris

# Установка игры
install: $(EXEC)
	mkdir -p $(INSTALL_DIR)
	mkdir -p resources
	cp $(EXEC) $(INSTALL_DIR)/

# Запуск игры
game:
	./games/tetris

# Удаление установленной игры
uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC)

# Очистка временных файлов
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(EXEC) $(BUILD_DIR)/*.gcda $(BUILD_DIR)/*.gcno tests/test_tetris
	rm -rf tests/gcov_report documentation/html

# --------------------------------------------------
# $@	-	имя цели (target)
# $<	-	имя первой зависимости
# $^	-	все зависимости
# -L	-	путь для поиска библиотек (Linker)
# -l	-	имя библиотеки (link)
# -I	-	путь для поиска заголовков (Include)
# -c	-	только компиляция, без линковки
# -o	-	output — имя выходного файла
# --------------------------------------------------