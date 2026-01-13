/**
 * This code is responsible for testing the base rootfs caching functions,
 * including cache key computation and cache directory resolution.
 */

#include "../../../all.h"

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

// --- compute_cache_key tests ---

/** Verifies compute_cache_key() produces a 64-character hex string. */
static void test_compute_cache_key_length(void **state)
{
    (void)state;
    char key[65];

    // Compute the cache key.
    int result = compute_cache_key(key, sizeof(key));

    // Verify success and correct length.
    assert_int_equal(0, result);
    assert_int_equal(64, (int)strlen(key));
}

/** Verifies compute_cache_key() produces consistent output. */
static void test_compute_cache_key_consistent(void **state)
{
    (void)state;
    char key1[65];
    char key2[65];

    // Compute the cache key twice.
    int result1 = compute_cache_key(key1, sizeof(key1));
    int result2 = compute_cache_key(key2, sizeof(key2));

    // Verify both succeed and produce identical output.
    assert_int_equal(0, result1);
    assert_int_equal(0, result2);
    assert_string_equal(key1, key2);
}

/** Verifies compute_cache_key() only contains hex characters. */
static void test_compute_cache_key_hex_chars(void **state)
{
    (void)state;
    char key[65];

    // Compute the cache key.
    int result = compute_cache_key(key, sizeof(key));
    assert_int_equal(0, result);

    // Verify each character is a valid hex digit.
    for (int i = 0; key[i] != '\0'; i++)
    {
        int is_hex = (key[i] >= '0' && key[i] <= '9') ||
                     (key[i] >= 'a' && key[i] <= 'f') ||
                     (key[i] >= 'A' && key[i] <= 'F');
        assert_true(is_hex);
    }
}

/** Verifies compute_cache_key() fails with buffer too small. */
static void test_compute_cache_key_buffer_too_small(void **state)
{
    (void)state;
    char key[32];

    // Attempt to compute with insufficient buffer.
    int result = compute_cache_key(key, sizeof(key));

    // Verify failure.
    assert_int_equal(-1, result);
}

/** Verifies compute_cache_key() works with exact buffer size. */
static void test_compute_cache_key_exact_buffer(void **state)
{
    (void)state;
    char key[65];

    // Compute with exactly 64 chars + null terminator.
    int result = compute_cache_key(key, sizeof(key));

    // Verify success.
    assert_int_equal(0, result);
}

// --- get_cache_dir tests ---

/** Verifies get_cache_dir() uses XDG_CACHE_HOME when set. */
static void test_get_cache_dir_xdg(void **state)
{
    (void)state;
    char path[256];

    // Save original environment value.
    char *original = getenv("XDG_CACHE_HOME");
    char *saved = original ? strdup(original) : NULL;

    // Set XDG_CACHE_HOME to a custom path.
    setenv("XDG_CACHE_HOME", "/custom/cache", 1);

    // Get the cache directory.
    int result = get_cache_dir(path, sizeof(path));

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

    // Verify the result uses XDG path.
    assert_int_equal(0, result);
    assert_string_equal("/custom/cache/limeos", path);
}

/** Verifies get_cache_dir() falls back to HOME/.cache when XDG not set. */
static void test_get_cache_dir_home_fallback(void **state)
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

    // Get the cache directory.
    int result = get_cache_dir(path, sizeof(path));

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

    // Verify the result uses HOME fallback.
    assert_int_equal(0, result);
    assert_string_equal("/home/testuser/.cache/limeos", path);
}

/** Verifies get_cache_dir() ignores empty XDG_CACHE_HOME. */
static void test_get_cache_dir_empty_xdg(void **state)
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

    // Get the cache directory.
    int result = get_cache_dir(path, sizeof(path));

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

    // Verify the result falls back to HOME.
    assert_int_equal(0, result);
    assert_string_equal("/home/testuser/.cache/limeos", path);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        /* compute_cache_key tests */
        cmocka_unit_test_setup_teardown(test_compute_cache_key_length, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_cache_key_consistent, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_cache_key_hex_chars, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_cache_key_buffer_too_small, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_cache_key_exact_buffer, setup, teardown),
        /* get_cache_dir tests */
        cmocka_unit_test_setup_teardown(test_get_cache_dir_xdg, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_cache_dir_home_fallback, setup, teardown),
        cmocka_unit_test_setup_teardown(test_get_cache_dir_empty_xdg, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
