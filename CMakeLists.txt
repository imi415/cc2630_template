cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR           arm-none-eabi-ar)
set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP      arm-none-eabi-objdump)
set(SIZE               arm-none-eabi-size)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/cc26xxware_modified/linker_files/cc26x0f128.lds)

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

set(TI_SOFTWARE_SRCS
        "cc26xxware_modified/startup_files/startup_gcc.c"
        "cc26xxware/startup_files/ccfg.c"
        "cc26xxware/driverlib/*.*"
)

set(TI_SOFTWARE_INCS
        cc26xxware
)

set(FREERTOS_SRCS
        "FreeRTOS/Source/tasks.c"
        "FreeRTOS/Source/queue.c"
        "FreeRTOS/Source/timers.c"
        "FreeRTOS/Source/event_groups.c"
        "FreeRTOS/Source/list.c"
        "FreeRTOS/Source/portable/GCC/ARM_CM3/port.c"
        "FreeRTOS/Source/portable/MemMang/heap_4.c"
)

set(FREERTOS_INCS
        FreeRTOS/Source/include
        FreeRTOS/Source/portable/GCC/ARM_CM3
)

file(GLOB_RECURSE SOURCES
        ${TI_SOFTWARE_SRCS}
        ${FREERTOS_SRCS}
        "src/*.*"
)

include_directories(
        ${TI_SOFTWARE_INCS}
        ${FREERTOS_INCS}
        include
)

add_definitions(
        -D__weak=__attribute__\(\(weak\)\)
        -D__packed=__attribute__\(\(__packed__\)\)
        # -DDRIVERLIB_NOROM
)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
