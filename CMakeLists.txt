cmake_minimum_required(VERSION 4.1)
project(greedy_loot_optimizer C)

set(CMAKE_C_STANDARD 11)

add_executable(greedy_loot_optimizer main.c)

add_custom_target(run
        COMMAND greedy_loot_optimizer
        ${CMAKE_SOURCE_DIR}/entrada_jogo.txt
        ${CMAKE_SOURCE_DIR}/saida_jogo.txt
        DEPENDS greedy_loot_optimizer
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Executando greedy_loot_optimizer..."
)