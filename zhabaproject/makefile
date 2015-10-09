all: build_scanners compile_source

ZHABACORE_DIR := zhabacore
ZHABAEXEC_DIR := zhabaexec
ZHABATEST_DIR := test
ZHABACORE_FILES := $(wildcard $(ZHABACORE_DIR)/*.c)
ifeq ($(wildcard $(ZHABACORE_DIR)/lexer.c),) 
    ZHABACORE_FILES += $(ZHABACORE_DIR)/lexer.c
endif
ifeq ($(wildcard $(ZHABACORE_DIR)/parser.tab.c),) 
    ZHABACORE_FILES += $(ZHABACORE_DIR)/parser.tab.c
endif

build_scanners:
	flex -o $(ZHABACORE_DIR)/lexer.c grammar/lexer.l
	bison -d -o $(ZHABACORE_DIR)/parser.tab.c grammar/parser.y
	

compile_source:
	mkdir -p bin
	gcc -g -o bin/zhaba $(ZHABACORE_FILES) $(ZHABAEXEC_DIR)/zhabaexec.c -lfl
	
tests: 
	gcc -g -o bin/test $(ZHABACORE_FILES) $(ZHABATEST_DIR)/test.c -lfl
