TARGET_DIR := target
SRC_DIR := src

TARGET = ${TARGET_DIR}/derelict-ps1
TYPE = ps-exe

SRCS += \
	src/main.cpp \
	src/DerelictApplication.cpp \
	src/GameScene.cpp \
	src/Angel.cpp \
	src/Engine.cpp


	include third_party/nugget/psyqo/psyqo.mk


clean-game:
	rm -rf target/*
	rm src/*.dep
	rm src/*.o
	rm src/*.dep
