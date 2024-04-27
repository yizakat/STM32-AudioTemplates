# Main build file for Oxide
cmake_minimum_required(VERSION 3.20)

# What is the project and what tooling are we using.
project("STM32F411-Blackpill" C ASM)

# Set this option to ON if you want the build automatically flashed to the device
option(FLASH_BUILD,"Flash binary after build" OFF)

# Add project source and library folders here.
add_subdirectory(source)

