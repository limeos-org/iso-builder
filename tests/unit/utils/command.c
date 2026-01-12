/**
 * This code is responsible for testing shell quoting, command execution,
 * and file operation helpers in src/utils/command.c.
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

// TODO: Implement unit tests.
static void test_placeholder(void **state)
{
    (void)state;
    skip();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_placeholder, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
