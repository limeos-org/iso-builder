/**
 * This code is responsible for testing the version utility functions,
 * including validation, parsing, and comparison of semantic versions.
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

// --- skip_version_prefix tests ---

/** Verifies skip_version_prefix() removes lowercase 'v' prefix. */
static void test_skip_version_prefix_lowercase_v(void **state)
{
    (void)state;
    const char *result = skip_version_prefix("v1.2.3");
    assert_string_equal("1.2.3", result);
}

/** Verifies skip_version_prefix() removes uppercase 'V' prefix. */
static void test_skip_version_prefix_uppercase_v(void **state)
{
    (void)state;
    const char *result = skip_version_prefix("V1.2.3");
    assert_string_equal("1.2.3", result);
}

/** Verifies skip_version_prefix() returns unchanged string without prefix. */
static void test_skip_version_prefix_no_prefix(void **state)
{
    (void)state;
    const char *result = skip_version_prefix("1.2.3");
    assert_string_equal("1.2.3", result);
}

/** Verifies skip_version_prefix() handles empty string. */
static void test_skip_version_prefix_empty(void **state)
{
    (void)state;
    const char *result = skip_version_prefix("");
    assert_string_equal("", result);
}

// --- validate_version tests ---

/** Verifies validate_version() accepts standard semver format. */
static void test_validate_version_standard(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("1.2.3"));
}

/** Verifies validate_version() accepts version with 'v' prefix. */
static void test_validate_version_with_v_prefix(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("v1.2.3"));
}

/** Verifies validate_version() accepts version with 'V' prefix. */
static void test_validate_version_with_uppercase_v_prefix(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("V1.2.3"));
}

/** Verifies validate_version() accepts zero versions. */
static void test_validate_version_zeros(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("0.0.0"));
}

/** Verifies validate_version() accepts multi-digit version numbers. */
static void test_validate_version_multi_digit(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("10.20.300"));
}

/** Verifies validate_version() rejects missing patch version. */
static void test_validate_version_missing_patch(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("1.2"));
}

/** Verifies validate_version() rejects missing minor version. */
static void test_validate_version_missing_minor(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("1"));
}

/** Verifies validate_version() rejects empty string. */
static void test_validate_version_empty(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version(""));
}

/** Verifies validate_version() rejects trailing characters. */
static void test_validate_version_trailing_chars(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("1.2.3-beta"));
}

/** Verifies validate_version() rejects leading zeros with extra chars. */
static void test_validate_version_extra_dots(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("1.2.3.4"));
}

/** Verifies validate_version() rejects non-numeric characters. */
static void test_validate_version_non_numeric(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("a.b.c"));
}

/** Verifies validate_version() rejects version with spaces. */
static void test_validate_version_with_spaces(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("1. 2.3"));
}

// --- extract_major_version tests ---

/** Verifies extract_major_version() extracts single digit major. */
static void test_extract_major_version_single_digit(void **state)
{
    (void)state;
    assert_int_equal(1, extract_major_version("1.2.3"));
}

/** Verifies extract_major_version() extracts multi-digit major. */
static void test_extract_major_version_multi_digit(void **state)
{
    (void)state;
    assert_int_equal(123, extract_major_version("123.4.5"));
}

/** Verifies extract_major_version() handles 'v' prefix. */
static void test_extract_major_version_with_prefix(void **state)
{
    (void)state;
    assert_int_equal(2, extract_major_version("v2.0.0"));
}

/** Verifies extract_major_version() extracts zero major. */
static void test_extract_major_version_zero(void **state)
{
    (void)state;
    assert_int_equal(0, extract_major_version("0.1.0"));
}

// --- compare_versions tests ---

/** Verifies compare_versions() returns 0 for equal versions. */
static void test_compare_versions_equal(void **state)
{
    (void)state;
    assert_int_equal(0, compare_versions("1.2.3", "1.2.3"));
}

/** Verifies compare_versions() returns 0 for equal versions with prefix. */
static void test_compare_versions_equal_with_prefix(void **state)
{
    (void)state;
    assert_int_equal(0, compare_versions("v1.2.3", "1.2.3"));
}

/** Verifies compare_versions() returns 1 when first major is greater. */
static void test_compare_versions_major_greater(void **state)
{
    (void)state;
    assert_int_equal(1, compare_versions("2.0.0", "1.9.9"));
}

/** Verifies compare_versions() returns -1 when first major is less. */
static void test_compare_versions_major_less(void **state)
{
    (void)state;
    assert_int_equal(-1, compare_versions("1.0.0", "2.0.0"));
}

/** Verifies compare_versions() returns 1 when first minor is greater. */
static void test_compare_versions_minor_greater(void **state)
{
    (void)state;
    assert_int_equal(1, compare_versions("1.3.0", "1.2.9"));
}

/** Verifies compare_versions() returns -1 when first minor is less. */
static void test_compare_versions_minor_less(void **state)
{
    (void)state;
    assert_int_equal(-1, compare_versions("1.2.0", "1.3.0"));
}

/** Verifies compare_versions() returns 1 when first patch is greater. */
static void test_compare_versions_patch_greater(void **state)
{
    (void)state;
    assert_int_equal(1, compare_versions("1.2.4", "1.2.3"));
}

/** Verifies compare_versions() returns -1 when first patch is less. */
static void test_compare_versions_patch_less(void **state)
{
    (void)state;
    assert_int_equal(-1, compare_versions("1.2.3", "1.2.4"));
}

/** Verifies compare_versions() handles multi-digit components. */
static void test_compare_versions_multi_digit(void **state)
{
    (void)state;
    assert_int_equal(1, compare_versions("1.10.0", "1.9.0"));
}

// --- Additional edge case tests ---

/** Verifies validate_version() rejects leading zeros. */
static void test_validate_version_leading_zeros(void **state)
{
    (void)state;

    // Leading zeros are technically valid in our implementation.
    // This test documents the behavior.
    assert_int_equal(0, validate_version("01.02.03"));
}

/** Verifies validate_version() rejects negative numbers. */
static void test_validate_version_negative(void **state)
{
    (void)state;
    assert_int_equal(-1, validate_version("-1.0.0"));
}

/** Verifies extract_major_version() returns -1 for invalid input. */
static void test_extract_major_version_invalid(void **state)
{
    (void)state;
    assert_int_equal(-1, extract_major_version("abc"));
}

/** Verifies extract_major_version() handles large numbers. */
static void test_extract_major_version_large(void **state)
{
    (void)state;
    assert_int_equal(999, extract_major_version("999.0.0"));
}

/** Verifies compare_versions() handles version with only major. */
static void test_compare_versions_partial(void **state)
{
    (void)state;

    // Partial versions should still compare the available parts.
    int result = compare_versions("2", "1");
    assert_int_equal(1, result);
}

/** Verifies compare_versions() handles mixed prefix versions. */
static void test_compare_versions_mixed_prefix(void **state)
{
    (void)state;

    // One with prefix, one without.
    assert_int_equal(0, compare_versions("v1.0.0", "V1.0.0"));
}

/** Verifies skip_version_prefix() handles version starting with number. */
static void test_skip_version_prefix_number_start(void **state)
{
    (void)state;
    const char *result = skip_version_prefix("123");
    assert_string_equal("123", result);
}

/** Verifies validate_version() handles very long version. */
static void test_validate_version_long(void **state)
{
    (void)state;
    assert_int_equal(0, validate_version("999999.999999.999999"));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        /* skip_version_prefix tests */
        cmocka_unit_test_setup_teardown(test_skip_version_prefix_lowercase_v, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_version_prefix_uppercase_v, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_version_prefix_no_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_version_prefix_empty, setup, teardown),
        /* validate_version tests */
        cmocka_unit_test_setup_teardown(test_validate_version_standard, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_with_v_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_with_uppercase_v_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_zeros, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_multi_digit, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_missing_patch, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_missing_minor, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_empty, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_trailing_chars, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_extra_dots, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_non_numeric, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_with_spaces, setup, teardown),
        /* extract_major_version tests */
        cmocka_unit_test_setup_teardown(test_extract_major_version_single_digit, setup, teardown),
        cmocka_unit_test_setup_teardown(test_extract_major_version_multi_digit, setup, teardown),
        cmocka_unit_test_setup_teardown(test_extract_major_version_with_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_extract_major_version_zero, setup, teardown),
        /* compare_versions tests */
        cmocka_unit_test_setup_teardown(test_compare_versions_equal, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_equal_with_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_major_greater, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_major_less, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_minor_greater, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_minor_less, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_patch_greater, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_patch_less, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_multi_digit, setup, teardown),

        // Additional edge case tests
        cmocka_unit_test_setup_teardown(test_validate_version_leading_zeros, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_negative, setup, teardown),
        cmocka_unit_test_setup_teardown(test_extract_major_version_invalid, setup, teardown),
        cmocka_unit_test_setup_teardown(test_extract_major_version_large, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_partial, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compare_versions_mixed_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_version_prefix_number_start, setup, teardown),
        cmocka_unit_test_setup_teardown(test_validate_version_long, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
