cmake_minimum_required(VERSION 3.0.0)
project(vcf2fas VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
enable_testing()

include_directories(source)
file(GLOB SOURCES "source/*cpp")

add_executable(vcf2fas ${SOURCES})

set_property(TARGET vcf2fas PROPERTY CXX_STANDARD 17)
