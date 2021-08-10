cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR           arm-none-eabi-ar)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP      arm-none-eabi-objdump)
set(SIZE               arm-none-eabi-size)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/cc26x0f128.lds)

set(COMMON_FLAGS
        "-march=armv7-m -mthumb -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections \
         --specs=nano.specs --specs=nosys.specs -fno-common -g -Og")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler")
set(CMAKE_EXE_LINKER_FLAGS
        "-Wl,--gc-sections,--print-memory-usage \
        -nostartfiles -T ${LINKER_SCRIPT} \
        -e ResetISR"
)

project(cc2630_template C ASM)
set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE APP_SOURCES
        "src/*.*"
)

set(FREERTOS_SRCS
        "FreeRTOS/tasks.c"
        "FreeRTOS/queue.c"
        "FreeRTOS/timers.c"
        "FreeRTOS/event_groups.c"
        "FreeRTOS/list.c"
        "FreeRTOS/portable/GCC/ARM_CM3/port.c"
        "FreeRTOS/portable/MemMang/heap_4.c"
)

set(INCLUDE_DIRS
        "cc13xx_cc26xxware/cc26xxware"
        "FreeRTOS/include"
        "FreeRTOS/portable/GCC/ARM_CM3"
        "include"
)

set(COMPILER_DEFS
        "-D__weak=__attribute__\(\(weak\)\)"
        "-D__packed=__attribute__\(\(__packed__\)\)"
        # "-DDRIVERLIB_NOROM"
)

set(SOURCES
        ${FREERTOS_SRCS}
        ${APP_SOURCES}
        "startup_files/startup_gcc.c"
        "startup_files/ccfg.c"
)

add_subdirectory(cc13xx_cc26xxware)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf "cc26xxware")
target_include_directories(${PROJECT_NAME}.elf PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE ${COMPILER_DEFS})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
