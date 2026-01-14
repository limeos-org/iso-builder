/**
 * Tests for the APT package bind mount caching.
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

/** Verifies get_package_cache_dir() uses XDG_CACHE_HOME when set. */
static void test_get_package_cache_dir_xdg(void **state)
{
    (void)state;
    char path[256];

    // Save original environment value.
    char *original = getenv("XDG_CACHE_HOME");
    char *saved = original ? strdup(original) : NULL;

    // Set XDG_CACHE_HOME to a custom path.
    setenv("XDG_CACHE_HOME", "/custom/cache", 1);

    // Get the package cache directory.
    int result = get_package_cache_dir(path, sizeof(path));

    // Restore original environment value.
    if (saved)
    {
        setenv("XDG_CACHE_HOME", saved, 1);
        free(saved);
    }
    else
    {
        unsetenv("XDG_CACHE_HOME");
    }

    // Verify the result uses XDG path with packages subdirectory.
    assert_int_equal(0, result);
    assert_string_equal("/custom/cache/limeos-iso-builder/packages/apt", path);
}

/** Verifies get_package_cache_dir() falls back to HOME/.cache when XDG not set. */
static void test_get_package_cache_dir_home_fallback(void **state)
{
    (void)state;
    char path[256];

    // Save original environment values.
    char *original_xdg = getenv("XDG_CACHE_HOME");
    char *saved_xdg = original_xdg ? strdup(original_xdg) : NULL;
    char *original_home = getenv("HOME");
    char *saved_home = original_home ? strdup(original_home) : NULL;

    // Unset XDG_CACHE_HOME and set HOME.
    unsetenv("XDG_CACHE_HOME");
    setenv("HOME", "/home/testuser", 1);

    // Get the package cache directory.
    int result = get_package_cache_dir(path, sizeof(path));

    // Restore original environment values.
    if (saved_xdg)
    {
        setenv("XDG_CACHE_HOME", saved_xdg, 1);
        free(saved_xdg);
    }
    if (saved_home)
    {
        setenv("HOME", saved_home, 1);
        free(saved_home);
    }

    // Verify the result uses HOME fallback with packages subdirectory.
    assert_int_equal(0, result);
    assert_string_equal("/home/testuser/.cache/limeos-iso-builder/packages/apt", path);
}

/** Verifies get_package_cache_dir() ignores empty XDG_CACHE_HOME. */
static void test_get_package_cache_dir_empty_xdg(void **state)
{
    (void)state;
    char path[256];

    // Save original environment values.
    char *original_xdg = getenv("XDG_CACHE_HOME");
    char *saved_xdg = original_xdg ? strdup(original_xdg) : NULL;
    char *original_home = getenv("HOME");
    char *saved_home = original_home ? strdup(original_home) : NULL;

    // Set empty XDG_CACHE_HOME and valid HOME.
    setenv("XDG_CACHE_HOME", "", 1);
    setenv("HOME", "/home/testuser", 1);

    // Get the package cache directory.
    int result = get_package_cache_dir(path, sizeof(path));

    // Restore original environment values.
    if (saved_xdg)
    {
        setenv("XDG_CACHE_HOME", saved_xdg, 1);
        free(saved_xdg);
    }
    else
    {
        unsetenv("XDG_CACHE_HOME");
    }
    if (saved_home)
    {
        setenv("HOME", saved_home, 1);
        free(saved_home);
    }

    // Verify the result falls back to HOME with packages subdirectory.
    assert_int_equal(0, result);
    assert_string_equal("/home/testuser/.cache/limeos-iso-builder/packages/apt", path);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_get_package_cache_dir_xdg, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_package_cache_dir_home_fallback, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_package_cache_dir_empty_xdg, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
