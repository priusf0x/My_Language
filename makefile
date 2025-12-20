SOURCES = \
		  common/buffer/buffer.cpp\
		  common/etc/tools.cpp\
	  	  common/string/my_string.cpp\
	  	  common/string/murmurhash.cpp\
		  common/vector/vector.cpp\
		  common/stack/stack.cpp \
		  common/tree/tree.cpp\
		  common/tree/tree_dump.cpp\
	  	  front_end/state_machine_generator/state_machine_functions.cpp\
 		  front_end/recursive_decent/recursive_decent.cpp\
		  front_end/recursive_decent/tokenization.cpp\
		  front_end/recursive_decent/recursive_decent_defines.cpp\
		  front_end/name_space/name_space.cpp
INCLUDES_DIR =\
			front_end/name_space\
			front_end/recursive_decent\
			front_end/state_machine_generator\
			front_end\
			front_end\
			common/buffer\
			common/etc\
			common/stack\
			common/string\
			common/tree\
			common/vector\
		  
SOURCE_MAIN_AST := front_end/ast_main.cpp
SOURCE_MAIN_GEN := front_end/state_machine_generator/state_machine_main.cpp

OBJ_DIR = build
SOURCE_DIR = src

INCLUDES = $(addprefix -I$(SOURCE_DIR)/, $(INCLUDES_DIR))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
OBJECT_MAIN_AST := $(addprefix $(OBJ_DIR)/, $(SOURCE_MAIN_AST:.cpp=.o))
OBJECT_MAIN_GEN := $(addprefix $(OBJ_DIR)/, $(SOURCE_MAIN_GEN:.cpp=.o))
SOURCES := $(addprefix $(SOURCE_DIR)/, $(SOURCES))
SOURCE_MAIN_AST := $(addprefix $(SOURCE_DIR)/, $(SOURCE_MAIN_AST))
SOURCE_MAIN_GEN := $(addprefix $(SOURCE_DIR)/, $(SOURCE_MAIN_GEN))

HEADERS = $(wildcard $(SOURCE_DIR)/*.h)

TARGET_AST = ast.out
TARGET_GEN = gen.out

CC = g++ 

CFLAGS =  -D _DEBUG -ggdb3 -std=c++17 -O2 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
CFLAGS += -lm
CFLAGS += $(INCLUDES)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
		@mkdir -p $(dir $@)
		@mkdir -p logs
		@echo "Compiling" $<
		@$(CC) $(CFLAGS) -c $< -o $@ 	
		@echo "Compiled Successfully" $<

$(TARGET_AST): $(OBJECTS) $(OBJECT_MAIN_AST)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

$(TARGET_GEN): $(OBJECTS) $(OBJECT_MAIN_GEN)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

ast: $(TARGET_AST)
gen: $(TARGET_GEN)

test:
	@./list_sester.out

all: $(TARGET) 

logclean:
	@rm -rf logs/*
	@echo "Cleaned Logs Successfully"

clean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(TARGET_AST)
	@rm -f $(TARGET_GEN)
	@rm -rf logs
	@echo "Cleaned Successfully"
