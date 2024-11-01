cmake_minimum_required(VERSION 3.20)
project(toylox)
set(CMAKE_BUILD_TYPE Debug)

add_executable(toylox 
	src/Lexer.cpp 
	src/Lexer.h 
	src/Parser.cpp 
	src/Parser.h 
	src/main.cpp 
	src/PrettyPrinter.h
	src/PrettyPrinter.cpp)
