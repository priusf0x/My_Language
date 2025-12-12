SOURCES = common/tools.cpp\
	  	  common/my_string.cpp\
	  	  common/murmurhash.cpp\
	  	  recursive_decent/state_machine_generator/state_machine_functions.cpp\
 		  recursive_decent/recursive_decent.cpp\
		  vector/vector.cpp\
		  recursive_decent/buffer.cpp\
# 		  stack/stack.cpp \
# 		  tree/tree_dump.cpp
# 		  tree/tree.cpp
		  
SOURCE_MAIN_AST := ast_main.cpp
SOURCE_MAIN_GEN := recursive_decent/state_machine_generator/state_machine_main.cpp

OBJ_DIR = build
SOURCE_DIR = src

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

CFLAGS = -D NDEBUG -ggdb3 -std=c++17 -O2 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
CFLAGS += -I$(SOURCE_DIR) -I$(SOURCE_DIR)/recursive_decent -I$(SOURCE_DIR)/common -I$(SOURCE_DIR)/tree -I$(SOURCE_DIR)/stack -I$(SOURCE_DIR)/recursive_decent/state_machine_generator -I$(SOURCE_DIR)/front_end -I$(SOURCE_DIR)/vector
CFLAGS += -lm

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
