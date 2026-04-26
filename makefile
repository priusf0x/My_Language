COMMON_SOURCES = \
		  common/buffer/buffer.cpp\
		  common/etc/tools.cpp\
	  	  common/string/my_string.cpp\
	  	  common/string/murmurhash.cpp\
		  common/vector/vector.cpp\
		  common/stack/stack.cpp \
		  common/tree/tree.cpp\
		  common/tree/tree_dump.cpp\
		  common/tree/tree_reader.cpp\
		  common/error_utils/error_handler.cpp

AST_SOURCES_ALT = \
 		  front_end/recursive_decent/recursive_decent.cpp\
		  front_end/state_machine_generator/state_machine_functions.cpp\
		  front_end/recursive_decent/tokenization.cpp\
		  front_end/recursive_decent/recursive_decent_defines.cpp\
		  front_end/name_space/name_space.cpp\
		  front_end/ast_main.cpp

GEN_SOURCES_ALT = \
	  	  front_end/state_machine_generator/state_machine_functions.cpp\
		  front_end/state_machine_generator/state_machine_main.cpp


COMP_SOURCES_ALT = \
		  back_end/compiler_ctor.cpp\
		  back_end/compiler_main.cpp\
 		  back_end/asm/asm_compiler.cpp


# 		  back_end/compiler.cpp

MIDDLE_END_SOURCES_ALT = \
		  middle_end/optimize_main.cpp\
		  middle_end/optimizer_ctor.cpp\
		  middle_end/optimize.cpp

INCLUDES_DIR =\
			back_end/\
			front_end/name_space\
			front_end/recursive_decent\
			front_end/state_machine_generator\
			front_end\
			common/buffer\
			common/etc\
			common/stack\
			common/string\
			common/tree\
			common/vector\
			common/error_utils\
			my_lang_lib/\

OBJ_DIR = obj
SOURCE_DIR = src
CACHE_DIR = cache

AST_SOURCES = $(COMMON_SOURCES) $(AST_SOURCES_ALT)
GEN_SOURCES = $(COMMON_SOURCES) $(GEN_SOURCES_ALT)
COMP_SOURCES = $(COMMON_SOURCES) $(COMP_SOURCES_ALT)
MIDDLE_END_SOURCES = $(COMMON_SOURCES) $(MIDDLE_END_SOURCES_ALT)

INCLUDES = $(addprefix -I$(SOURCE_DIR)/, $(INCLUDES_DIR))

#front_end part 
AST_OBJECTS := $(addprefix $(OBJ_DIR)/, $(AST_SOURCES:.cpp=.o))
AST_SOURCES := $(addprefix $(SOURCE_DIR)/, $(AST_SOURCES))
TARGET_AST = ast.out

#key words generator
GEN_OBJECTS := $(addprefix $(OBJ_DIR)/, $(GEN_SOURCES:.cpp=.o))
GEN_SOURCES := $(addprefix $(SOURCE_DIR)/, $(GEN_SOURCES))
TARGET_GEN = gen.out

#compiler part 
COMP_OBJECTS := $(addprefix $(OBJ_DIR)/, $(COMP_SOURCES:.cpp=.o))
COMP_SOURCES := $(addprefix $(SOURCE_DIR)/, $(COMP_SOURCES))
TARGET_COMP = compiler.out

#middle-end part 
MIDDLE_END_OBJECTS := $(addprefix $(OBJ_DIR)/, $(MIDDLE_END_SOURCES:.cpp=.o))
MIDDLE_END_SOURCES := $(addprefix $(SOURCE_DIR)/, $(MIDDLE_END_SOURCES))
TARGET_MD = middle_end.out

# c++/c compiler options
CC = clang++
CFLAGS =\
 		-D_DEBUG\
		-ggdb3\
		-std=c++20\
		-O2\
		-Wall\
		-Wextra\
		-Wpedantic\
		-Weffc++\
		-Wmissing-declarations\
		-Wcast-align\
		-Wcast-qual\
		-Wchar-subscripts\
		-Wconversion\
		-Wctor-dtor-privacy\
		-Wempty-body\
		-Wfloat-equal\
		-Wformat-nonliteral\
		-Wformat-security\
		-Wformat-signedness\
		-Wformat=2\
		-Winline\
		-Wnon-virtual-dtor\
		-Woverloaded-virtual\
		-Wpacked\
		-Wpointer-arith\
		-Winit-self\
		-Wredundant-decls\
		-Wshadow\
		-Wextra-semi\
		-Wsign-conversion\
		-Wsign-promo\
		-Wstrict-overflow=2\
		-Wsuggest-override\
		-Wswitch-default\
		-Wswitch-enum\
		-Wundef\
		-Wunreachable-code\
		-Wunused\
		-Wvariadic-macros\
		-Wno-missing-field-initializers\
		-Wfloat-conversion\
		-Wdouble-promotion\
		-Wno-narrowing\
		-Wno-old-style-cast\
		-Wno-varargs\
		-Wstack-protector\
		-fsized-deallocation\
		-fstack-protector\
		-fstrict-overflow\
		-fno-omit-frame-pointer\
		-fPIE\
		-Werror=vla\
		-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
CFLAGS += $(INCLUDES)

RELEASE_FLAGS = -O2 -D NDEBUG 
RELEASE_FLAGS += $(INCLUDES)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(CACHE_DIR)
	@mkdir -p logs
	@echo "Compiling" $<
	@$(CC) $(CFLAGS) -c $< -o $@ 	
	@echo "Compiled Successfully" $<

$(TARGET_AST): $(AST_OBJECTS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

$(TARGET_GEN): $(GEN_OBJECTS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"
	
$(TARGET_COMP): $(COMP_OBJECTS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

$(TARGET_MD): $(MIDDLE_END_OBJECTS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

ast: $(TARGET_AST)
	@./$(TARGET_AST)
gen: $(TARGET_GEN)
	@./$(TARGET_GEN)
comp: $(TARGET_COMP)
	@./$(TARGET_COMP)
md: $(TARGET_MD)
	@./$(TARGET_MD)
	
release: CFLAGS = $(RELEASE_FLAGS)
release:  $(TARGET_AST) $(TARGET_GEN) $(TARGET_COMP) $(TARGET_MD)

clean:
	@rm -rf logs/*
	@rm -rf $(OBJ_DIR)
	@rm -rf $(CACHE_DIR)
	@rm -f $(TARGET_AST)
	@rm -f $(TARGET_GEN)
	@rm -f $(TARGET_COMP)
	@rm -f $(TARGET_MD)
	@echo "Cleaned Successfully"
