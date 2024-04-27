#
# This defines the targets and build dependencies for the Oxide synthesiser.
#
set(TARGET "STM32F411-Discovery")

# This lists the dependencies of the target
add_executable(${TARGET}

    # app source files
    app/main.c
    
  
    # Board support files
    bsp/audio.c
    bsp/board.c    

    # The startup vector init (asm file)
    startup/startup_stm32f411ceux.s
)

# Include directories to search for header files
target_include_directories(${TARGET} PRIVATE    
    bsp    
    drivers/CMSIS/Core/Include
    drivers/STM32F4xx/Device/Include
    drivers/HAL_LL/inc
) 

# Compiler definitions, passed with the -D flag to the compiler
target_compile_definitions(${TARGET} PRIVATE

    # These STM defines are used by various headers    
    STM32F411xE
    STM32
    STM32F4
    STM32F411VET6

    # The frequency of the external high speed clock (xtal)
    HSE_VALUE=8000000    
)

# Compiler options
target_compile_options(${TARGET} PRIVATE
    -g3
    -O0
    -mcpu=cortex-m4
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

target_link_options(${TARGET} PRIVATE
    -mcpu=cortex-m4
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -T ${PROJECT_SOURCE_DIR}/source/STM32F411CEUX_FLASH.ld
)

# This is a pseudo target to optionally flash the device after a build.  Turn this on
# in the top-level CMakeLists file.
if(FLASH_BUILD)

    # STM programmer tool and params
    set(STM_PROGRAMMER STM32_Programmer_CLI)
    set(STM_PROGRAMMER_ARGS "-c port=SWD mode=UR reset=HWrst  -w" ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.elf "-g")

    message(STATUS "##> Configured to auto-flash board.")

    add_custom_target(flash ALL DEPENDS ${TARGET}
        COMMAND ${STM_PROGRAMMER} ${STM_PROGRAMMER_ARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Flashing board with" ${TARGET}
        VERBATIM)
endif()

    
    




