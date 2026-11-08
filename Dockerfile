FROM ubuntu:26.04 AS base

RUN apt-get update && apt-get install -y acl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Стадия разработки
FROM base AS development

RUN apt-get update && apt-get install -y \
    build-essential \
    libncurses5-dev \
    libncursesw5-dev \
    check \
    clang-format-18 \
    lcov \
    valgrind \
    doxygen \
    && ln -s /usr/bin/clang-format-18 /usr/bin/clang-format \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN useradd -m -s /bin/bash developer \
    && setfacl -R -m u:developer:rwx /app

USER developer
CMD ["bash"]

# Стадия компиляции
FROM development AS compile
RUN make

# Стадия проверки стиля
FROM development AS checkstyle
RUN make format_check

# Стадия тестов
FROM compile AS test
RUN make test

# Стадия продакшена
FROM base AS production

RUN apt-get update && apt-get install -y \
    libncurses6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=compile /app/tetris .

RUN useradd -m -s /bin/bash player \
    && setfacl -R -m u:player:rwx /app

USER player
CMD ["./tetris"]

# dockle --ignore CIS-DI-0005 --ignore CIS-DI-0006 --ignore CIS-DI-0008