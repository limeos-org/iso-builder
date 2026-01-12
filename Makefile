CC = clang
CFLAGS = -Wall -Wextra -g
LIBS = -lcurl -ljson-c -lcrypto

# ---
# Build Configuration
# ---

SRCDIR = src
OBJDIR = obj
BINDIR = bin

TARGET = $(BINDIR)/limeos-iso-builder

SOURCES = $(shell find $(SRCDIR) -name '*.c')
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

INCLUDES = $(shell find $(SRCDIR) -type d -exec printf "-I{} " \;)
CFLAGS += $(INCLUDES)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

# ---
# Tests Configuration
# ---

TESTDIR = tests/unit
TESTOBJDIR = obj/tests
TESTBINDIR = bin/tests
TESTSRCOBJDIR = obj/tests-src

TEST_SOURCES = $(shell find $(TESTDIR) -name '*.c')
TEST_OBJS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(TESTOBJDIR)/%.o)
TEST_BINARIES = $(TEST_SOURCES:$(TESTDIR)/%.c=$(TESTBINDIR)/%)
TEST_SRC_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(TESTSRCOBJDIR)/%.o)
TEST_SRC_OBJECTS_NO_MAIN = $(filter-out $(TESTSRCOBJDIR)/main.o,$(TEST_SRC_OBJECTS))

TEST_CFLAGS = -Wall -Wextra -g $(INCLUDES) -Itests -DTESTING
TEST_LIBS = $(LIBS) -lcmocka

$(TESTSRCOBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TESTOBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TESTBINDIR)/%: $(TESTOBJDIR)/%.o $(TEST_SRC_OBJECTS_NO_MAIN)
	@mkdir -p $(dir $@)
	$(CC) $< $(TEST_SRC_OBJECTS_NO_MAIN) -o $@ $(TEST_LIBS)

test: $(TEST_BINARIES)
	@failed=0; \
	for t in $(TEST_BINARIES); do \
		echo ""; \
		echo "Running tests from \"$(notdir $(TESTDIR))/$${t#$(TESTBINDIR)/}\":"; \
		echo ""; \
		$$t || failed=1; \
	done; \
	echo ""; \
	if [ $$failed -eq 0 ]; then \
		echo "All tests passed."; \
	else \
		echo "Some tests failed."; \
		exit 1; \
	fi

test-clean:
	rm -rf $(TESTOBJDIR) $(TESTBINDIR) $(TESTSRCOBJDIR)

# ---
# Special Directives
# ---

.PRECIOUS: $(TEST_OBJS) $(TEST_SRC_OBJECTS)
.PHONY: all clean test test-clean
