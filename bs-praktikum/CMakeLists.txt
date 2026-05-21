cmake_minimum_required(VERSION 3.7.2)
project(kvserver C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Alle .c-Dateien aus src/ werden zum server-Binary gelinkt
add_executable(server
    src/main.c
    src/kvstore.c
    src/pubsub.c
)

# include/ Ordner bekannt machen damit #include "kvstore.h" funktioniert
target_include_directories(server PRIVATE include)

# Warnungen an — der Compiler hilft Fehler zu finden
target_compile_options(server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -g
)

# Optionaler Selbsttest fuer Aryan (cmake --build build --target test_kv)
add_executable(test_kv
    tests/test_kvstore.c
    src/kvstore.c
)
target_include_directories(test_kv PRIVATE include)
