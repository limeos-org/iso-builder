/**
 * This code is responsible for testing the signal handling utility functions,
 * including interrupt checking and cleanup directory management.
 */

#include "../../all.h"

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;

    // Clear any previous cleanup directory.
    clear_cleanup_dir();
    return 0;
}

/** Cleans up the test environment after each test. */
static int teardown(void **state)
{
    (void)state;

    // Reset signal handlers and cleanup state.
    clear_cleanup_dir();
    return 0;
}

// --- check_interrupted tests ---

/** Verifies check_interrupted() returns 0 when not interrupted. */
static void test_check_interrupted_not_interrupted(void **state)
{
    (void)state;

    // Without sending a signal, check_interrupted should return 0.
    // Note: We can't easily test the interrupted case without sending signals.
    int result = check_interrupted();

    // Should be 0 since no signal was sent.
    assert_int_equal(0, result);
}

// --- clear_cleanup_dir tests ---

/** Verifies clear_cleanup_dir() clears the cleanup directory. */
static void test_clear_cleanup_dir_clears(void **state)
{
    (void)state;

    // Install signal handlers with a cleanup directory.
    install_signal_handlers("/tmp/test-cleanup");

    // Clear the cleanup directory.
    clear_cleanup_dir();

    // Verify check_interrupted doesn't try to clean up anything.
    // (This is an indirect test - if cleanup dir was set, it would try to rm_rf)
    int result = check_interrupted();
    assert_int_equal(0, result);
}

// --- install_signal_handlers tests ---

/** Verifies install_signal_handlers() accepts NULL cleanup directory. */
static void test_install_signal_handlers_null_dir(void **state)
{
    (void)state;

    // Should not crash with NULL.
    install_signal_handlers(NULL);

    // Verify check_interrupted still works.
    int result = check_interrupted();
    assert_int_equal(0, result);
}

/** Verifies install_signal_handlers() accepts valid directory. */
static void test_install_signal_handlers_valid_dir(void **state)
{
    (void)state;

    // Install with a valid directory path.
    install_signal_handlers("/tmp/test-signal-cleanup");

    // Verify check_interrupted works.
    int result = check_interrupted();
    assert_int_equal(0, result);

    // Clean up.
    clear_cleanup_dir();
}

/** Verifies install_signal_handlers() handles long paths. */
static void test_install_signal_handlers_long_path(void **state)
{
    (void)state;

    // Create a long but valid path.
    char long_path[512];
    memset(long_path, 'a', sizeof(long_path) - 1);
    long_path[0] = '/';
    long_path[sizeof(long_path) - 1] = '\0';

    // Should handle long paths without crashing.
    install_signal_handlers(long_path);

    // Verify check_interrupted works.
    int result = check_interrupted();
    assert_int_equal(0, result);

    // Clean up.
    clear_cleanup_dir();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // check_interrupted tests
        cmocka_unit_test_setup_teardown(test_check_interrupted_not_interrupted, setup, teardown),

        // clear_cleanup_dir tests
        cmocka_unit_test_setup_teardown(test_clear_cleanup_dir_clears, setup, teardown),

        // install_signal_handlers tests
        cmocka_unit_test_setup_teardown(test_install_signal_handlers_null_dir, setup, teardown),
        cmocka_unit_test_setup_teardown(test_install_signal_handlers_valid_dir, setup, teardown),
        cmocka_unit_test_setup_teardown(test_install_signal_handlers_long_path, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
