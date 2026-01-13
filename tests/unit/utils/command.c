/**
 * This code is responsible for testing the command utility functions,
 * including shell quoting and path validation.
 */

#include "../../all.h"

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;
    return 0;
}

/** Cleans up the test environment after each test. */
static int teardown(void **state)
{
    (void)state;
    return 0;
}

// --- shell_quote tests ---

/** Verifies shell_quote() wraps simple string in single quotes. */
static void test_shell_quote_simple(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("hello", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'hello'", buffer);
}

/** Verifies shell_quote() handles empty string. */
static void test_shell_quote_empty(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("''", buffer);
}

/** Verifies shell_quote() escapes single quotes correctly. */
static void test_shell_quote_with_single_quote(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("it's", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'it'\\''s'", buffer);
}

/** Verifies shell_quote() handles multiple single quotes. */
static void test_shell_quote_multiple_single_quotes(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("a'b'c", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'a'\\''b'\\''c'", buffer);
}

/** Verifies shell_quote() preserves spaces inside quotes. */
static void test_shell_quote_with_spaces(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("hello world", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'hello world'", buffer);
}

/** Verifies shell_quote() preserves special shell characters. */
static void test_shell_quote_special_chars(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("$HOME; rm -rf /", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'$HOME; rm -rf /'", buffer);
}

/** Verifies shell_quote() handles double quotes (no escaping needed). */
static void test_shell_quote_double_quotes(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("say \"hello\"", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'say \"hello\"'", buffer);
}

/** Verifies shell_quote() handles backticks (no escaping needed). */
static void test_shell_quote_backticks(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote("`whoami`", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'`whoami`'", buffer);
}

/** Verifies shell_quote() returns -1 for buffer too small. */
static void test_shell_quote_buffer_too_small(void **state)
{
    (void)state;
    char buffer[4];
    int result = shell_quote("hello", buffer, sizeof(buffer));
    assert_int_equal(-1, result);
}

/** Verifies shell_quote() returns -2 for NULL input. */
static void test_shell_quote_null_input(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote(NULL, buffer, sizeof(buffer));
    assert_int_equal(-2, result);
}

/** Verifies shell_quote() returns -2 for NULL output. */
static void test_shell_quote_null_output(void **state)
{
    (void)state;
    int result = shell_quote("hello", NULL, 64);
    assert_int_equal(-2, result);
}

// --- shell_quote_path tests ---

/** Verifies shell_quote_path() quotes simple path. */
static void test_shell_quote_path_simple(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote_path("/usr/bin", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'/usr/bin'", buffer);
}

/** Verifies shell_quote_path() handles path with spaces. */
static void test_shell_quote_path_with_spaces(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote_path("/path/to/my file", buffer, sizeof(buffer));
    assert_int_equal(0, result);
    assert_string_equal("'/path/to/my file'", buffer);
}

/** Verifies shell_quote_path() rejects empty path. */
static void test_shell_quote_path_empty(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote_path("", buffer, sizeof(buffer));
    assert_int_equal(-2, result);
}

/** Verifies shell_quote_path() rejects NULL path. */
static void test_shell_quote_path_null(void **state)
{
    (void)state;
    char buffer[64];
    int result = shell_quote_path(NULL, buffer, sizeof(buffer));
    assert_int_equal(-2, result);
}

// --- file_exists tests ---

/** Verifies file_exists() returns 1 for existing file. */
static void test_file_exists_existing(void **state)
{
    (void)state;

    // Use the test binary itself as a guaranteed existing file.
    assert_int_equal(1, file_exists("./bin/tests/utils/command"));
}

/** Verifies file_exists() returns 0 for non-existing file. */
static void test_file_exists_nonexistent(void **state)
{
    (void)state;

    // Use a path with random UUID to guarantee non-existence.
    assert_int_equal(0, file_exists("/nonexistent_a1b2c3d4e5f6/path/to/file"));
}

/** Verifies file_exists() returns 1 for existing directory. */
static void test_file_exists_directory(void **state)
{
    (void)state;

    // Use current directory which must exist for tests to run.
    assert_int_equal(1, file_exists("."));
}

// --- write_file tests ---

/** Verifies write_file() creates a new file with content. */
static void test_write_file_creates_file(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-write.txt";
    const char *content = "test content\n";

    // Write the test file.
    int result = write_file(test_path, content);
    assert_int_equal(0, result);

    // Verify the file exists.
    assert_int_equal(1, file_exists(test_path));

    // Clean up.
    remove(test_path);
}

/** Verifies write_file() overwrites existing file. */
static void test_write_file_overwrites(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-overwrite.txt";

    // Write initial content.
    write_file(test_path, "initial");

    // Overwrite with new content.
    int result = write_file(test_path, "new");
    assert_int_equal(0, result);

    // Read and verify content.
    FILE *f = fopen(test_path, "r");
    assert_non_null(f);
    char buffer[64];
    fgets(buffer, sizeof(buffer), f);
    fclose(f);
    assert_string_equal("new", buffer);

    // Clean up.
    remove(test_path);
}

/** Verifies write_file() returns -1 for invalid path. */
static void test_write_file_invalid_path(void **state)
{
    (void)state;

    // Try to write to a non-existent directory.
    int result = write_file("/nonexistent_dir_xyz/file.txt", "content");
    assert_int_equal(-1, result);
}

// --- chmod_file tests ---

/** Verifies chmod_file() applies numeric mode correctly. */
static void test_chmod_file_numeric_mode(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-chmod.txt";

    // Create a test file.
    write_file(test_path, "test");

    // Apply 644 mode.
    int result = chmod_file("644", test_path);
    assert_int_equal(0, result);

    // Verify mode was applied.
    struct stat st;
    stat(test_path, &st);
    assert_int_equal(0644, st.st_mode & 0777);

    // Clean up.
    remove(test_path);
}

/** Verifies chmod_file() applies +x mode correctly. */
static void test_chmod_file_plus_x(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-chmod-x.txt";

    // Create a test file with no execute permission.
    write_file(test_path, "test");
    chmod(test_path, 0644);

    // Apply +x mode.
    int result = chmod_file("+x", test_path);
    assert_int_equal(0, result);

    // Verify execute bits were added.
    struct stat st;
    stat(test_path, &st);
    assert_true((st.st_mode & S_IXUSR) != 0);
    assert_true((st.st_mode & S_IXGRP) != 0);
    assert_true((st.st_mode & S_IXOTH) != 0);

    // Clean up.
    remove(test_path);
}

/** Verifies chmod_file() rejects invalid mode string. */
static void test_chmod_file_invalid_mode(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-chmod-inv.txt";

    // Create a test file.
    write_file(test_path, "test");

    // Try invalid mode.
    int result = chmod_file("invalid", test_path);
    assert_int_equal(-2, result);

    // Clean up.
    remove(test_path);
}

/** Verifies chmod_file() returns -2 for NULL inputs. */
static void test_chmod_file_null_inputs(void **state)
{
    (void)state;

    // Test NULL mode.
    assert_int_equal(-2, chmod_file(NULL, "/tmp/test"));

    // Test NULL path.
    assert_int_equal(-2, chmod_file("644", NULL));
}

/** Verifies chmod_file() returns -1 for non-existent file. */
static void test_chmod_file_nonexistent(void **state)
{
    (void)state;

    // Try to chmod non-existent file.
    int result = chmod_file("+x", "/nonexistent_xyz/file.txt");
    assert_int_equal(-1, result);
}

// --- find_first_glob tests ---

/** Verifies find_first_glob() finds matching file. */
static void test_find_first_glob_finds_match(void **state)
{
    (void)state;
    char result[256];

    // Find any .c file in tests directory.
    int ret = find_first_glob("./tests/unit/utils/*.c", result, sizeof(result));
    assert_int_equal(0, ret);

    // Verify result ends with .c.
    size_t len = strlen(result);
    assert_true(len > 2);
    assert_string_equal(".c", result + len - 2);
}

/** Verifies find_first_glob() returns -1 for no match. */
static void test_find_first_glob_no_match(void **state)
{
    (void)state;
    char result[256];

    // Try to find a non-existent pattern.
    int ret = find_first_glob("/nonexistent/*.xyz123", result, sizeof(result));
    assert_int_equal(-1, ret);
}

/** Verifies find_first_glob() handles buffer correctly. */
static void test_find_first_glob_buffer_size(void **state)
{
    (void)state;
    char result[256];

    // Find Makefile which should exist.
    int ret = find_first_glob("./Makefile", result, sizeof(result));
    assert_int_equal(0, ret);
    assert_string_equal("./Makefile", result);
}

// --- mkdir_p tests ---

/** Verifies mkdir_p() creates single directory. */
static void test_mkdir_p_single_dir(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-mkdir-single";

    // Remove if exists from previous test.
    rmdir(test_path);

    // Create directory.
    int result = mkdir_p(test_path);
    assert_int_equal(0, result);

    // Verify directory exists.
    assert_int_equal(1, file_exists(test_path));

    // Clean up.
    rmdir(test_path);
}

/** Verifies mkdir_p() creates nested directories. */
static void test_mkdir_p_nested_dirs(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-mkdir/nested/deep";

    // Create nested directories.
    int result = mkdir_p(test_path);
    assert_int_equal(0, result);

    // Verify directory exists.
    assert_int_equal(1, file_exists(test_path));

    // Clean up.
    rm_rf("/tmp/iso-builder-test-mkdir");
}

/** Verifies mkdir_p() succeeds on existing directory. */
static void test_mkdir_p_existing_dir(void **state)
{
    (void)state;

    // Current directory always exists.
    int result = mkdir_p(".");
    assert_int_equal(0, result);
}

// --- rm_rf tests ---

/** Verifies rm_rf() removes file. */
static void test_rm_rf_removes_file(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-rm-file.txt";

    // Create a test file.
    write_file(test_path, "test");
    assert_int_equal(1, file_exists(test_path));

    // Remove it.
    int result = rm_rf(test_path);
    assert_int_equal(0, result);

    // Verify it's gone.
    assert_int_equal(0, file_exists(test_path));
}

/** Verifies rm_rf() removes directory recursively. */
static void test_rm_rf_removes_dir_recursive(void **state)
{
    (void)state;
    const char *test_dir = "/tmp/iso-builder-test-rm-dir";
    char nested_file[256];

    // Create nested structure.
    mkdir_p(test_dir);
    snprintf(nested_file, sizeof(nested_file), "%s/subdir/file.txt", test_dir);
    mkdir_p("/tmp/iso-builder-test-rm-dir/subdir");
    write_file(nested_file, "test");

    // Remove recursively.
    int result = rm_rf(test_dir);
    assert_int_equal(0, result);

    // Verify it's gone.
    assert_int_equal(0, file_exists(test_dir));
}

/** Verifies rm_rf() succeeds on non-existent path. */
static void test_rm_rf_nonexistent(void **state)
{
    (void)state;

    // Should succeed (rm -rf doesn't fail on missing files).
    int result = rm_rf("/tmp/iso-builder-nonexistent-xyz123");
    assert_int_equal(0, result);
}

// --- copy_file tests ---

/** Verifies copy_file() copies file content. */
static void test_copy_file_copies_content(void **state)
{
    (void)state;
    const char *src = "/tmp/iso-builder-test-copy-src.txt";
    const char *dst = "/tmp/iso-builder-test-copy-dst.txt";
    const char *content = "copy test content";

    // Create source file.
    write_file(src, content);

    // Copy it.
    int result = copy_file(src, dst);
    assert_int_equal(0, result);

    // Verify destination has same content.
    FILE *f = fopen(dst, "r");
    assert_non_null(f);
    char buffer[64];
    fgets(buffer, sizeof(buffer), f);
    fclose(f);
    assert_string_equal(content, buffer);

    // Clean up.
    remove(src);
    remove(dst);
}

/** Verifies copy_file() returns -1 for non-existent source. */
static void test_copy_file_nonexistent_src(void **state)
{
    (void)state;

    // Try to copy non-existent file.
    int result = copy_file("/nonexistent_xyz/src.txt", "/tmp/dst.txt");
    assert_int_not_equal(0, result);
}

// --- symlink_file tests ---

/** Verifies symlink_file() creates symbolic link. */
static void test_symlink_file_creates_link(void **state)
{
    (void)state;
    const char *target = "/tmp/iso-builder-test-symlink-target.txt";
    const char *link_path = "/tmp/iso-builder-test-symlink-link";

    // Create target file.
    write_file(target, "target content");

    // Remove link if exists.
    remove(link_path);

    // Create symlink.
    int result = symlink_file(target, link_path);
    assert_int_equal(0, result);

    // Verify link exists.
    struct stat st;
    assert_int_equal(0, lstat(link_path, &st));
    assert_true(S_ISLNK(st.st_mode));

    // Clean up.
    remove(link_path);
    remove(target);
}

/** Verifies symlink_file() overwrites existing link. */
static void test_symlink_file_overwrites(void **state)
{
    (void)state;
    const char *target1 = "/tmp/iso-builder-test-sym-t1.txt";
    const char *target2 = "/tmp/iso-builder-test-sym-t2.txt";
    const char *link_path = "/tmp/iso-builder-test-sym-link";

    // Create target files.
    write_file(target1, "target1");
    write_file(target2, "target2");

    // Create initial symlink.
    remove(link_path);
    symlink_file(target1, link_path);

    // Overwrite with new target.
    int result = symlink_file(target2, link_path);
    assert_int_equal(0, result);

    // Verify link points to new target.
    char resolved[256];
    ssize_t len = readlink(link_path, resolved, sizeof(resolved) - 1);
    assert_true(len > 0);
    resolved[len] = '\0';
    assert_string_equal(target2, resolved);

    // Clean up.
    remove(link_path);
    remove(target1);
    remove(target2);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // shell_quote tests
        cmocka_unit_test_setup_teardown(test_shell_quote_simple, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_empty, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_with_single_quote, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_multiple_single_quotes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_with_spaces, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_special_chars, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_double_quotes, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_backticks, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_buffer_too_small, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_null_input, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_null_output, setup, teardown),

        // shell_quote_path tests
        cmocka_unit_test_setup_teardown(test_shell_quote_path_simple, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_path_with_spaces, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_path_empty, setup, teardown),
        cmocka_unit_test_setup_teardown(test_shell_quote_path_null, setup, teardown),

        // file_exists tests
        cmocka_unit_test_setup_teardown(test_file_exists_existing, setup, teardown),
        cmocka_unit_test_setup_teardown(test_file_exists_nonexistent, setup, teardown),
        cmocka_unit_test_setup_teardown(test_file_exists_directory, setup, teardown),

        // write_file tests
        cmocka_unit_test_setup_teardown(test_write_file_creates_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_write_file_overwrites, setup, teardown),
        cmocka_unit_test_setup_teardown(test_write_file_invalid_path, setup, teardown),

        // chmod_file tests
        cmocka_unit_test_setup_teardown(test_chmod_file_numeric_mode, setup, teardown),
        cmocka_unit_test_setup_teardown(test_chmod_file_plus_x, setup, teardown),
        cmocka_unit_test_setup_teardown(test_chmod_file_invalid_mode, setup, teardown),
        cmocka_unit_test_setup_teardown(test_chmod_file_null_inputs, setup, teardown),
        cmocka_unit_test_setup_teardown(test_chmod_file_nonexistent, setup, teardown),

        // find_first_glob tests
        cmocka_unit_test_setup_teardown(test_find_first_glob_finds_match, setup, teardown),
        cmocka_unit_test_setup_teardown(test_find_first_glob_no_match, setup, teardown),
        cmocka_unit_test_setup_teardown(test_find_first_glob_buffer_size, setup, teardown),

        // mkdir_p tests
        cmocka_unit_test_setup_teardown(test_mkdir_p_single_dir, setup, teardown),
        cmocka_unit_test_setup_teardown(test_mkdir_p_nested_dirs, setup, teardown),
        cmocka_unit_test_setup_teardown(test_mkdir_p_existing_dir, setup, teardown),

        // rm_rf tests
        cmocka_unit_test_setup_teardown(test_rm_rf_removes_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_rm_rf_removes_dir_recursive, setup, teardown),
        cmocka_unit_test_setup_teardown(test_rm_rf_nonexistent, setup, teardown),

        // copy_file tests
        cmocka_unit_test_setup_teardown(test_copy_file_copies_content, setup, teardown),
        cmocka_unit_test_setup_teardown(test_copy_file_nonexistent_src, setup, teardown),

        // symlink_file tests
        cmocka_unit_test_setup_teardown(test_symlink_file_creates_link, setup, teardown),
        cmocka_unit_test_setup_teardown(test_symlink_file_overwrites, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
