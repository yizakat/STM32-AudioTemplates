#
# We're using a specific tool chain for our build, so define the locations here
#
set(TOOLS_DIR "/opt/st/stm32cubeclt/GNU-tools-for-STM32/bin/")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(TOOLS_PREFIX "arm-none-eabi-")

set_property(TARGET ${TARGET} PROPERTY C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(FLAGS "-fdata-sections -ffunction-sections --specs=nano.specs -Wl,--gc-sections")
set(BSP_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard")
set(ASM_FLAGS "-x assembler-with-cpp")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Ofast")

set(CMAKE_C_COMPILER ${TOOLS_DIR}${TOOLS_PREFIX}gcc${TOOL_SUFFIX} ${FLAGS} ${BSP_FLAGS})
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} ${ASM_FLAGS})

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)