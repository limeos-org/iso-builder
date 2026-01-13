/**
 * This code is responsible for testing the download utility functions,
 * including SHA256 checksum computation and file verification.
 */

#include "../../../all.h"

/** Length of SHA256 hash in hex format (64 chars + null). */
#define SHA256_HEX_LENGTH 65

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

// --- compute_file_sha256 tests ---

/** Verifies compute_file_sha256() computes correct hash for known content. */
static void test_compute_file_sha256_known_content(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256.txt";
    char hash[SHA256_HEX_LENGTH];

    // Create a test file with known content.
    // SHA256 of "hello\n" is well-known.
    write_file(test_path, "hello\n");

    // Compute the hash.
    int result = compute_file_sha256(test_path, hash, sizeof(hash));
    assert_int_equal(0, result);

    // Verify the hash matches expected value.
    // SHA256("hello\n") = 5891b5b522d5df086d0ff0b110fbd9d21bb4fc7163af34d08286a2e846f6be03
    assert_string_equal(
        "5891b5b522d5df086d0ff0b110fbd9d21bb4fc7163af34d08286a2e846f6be03",
        hash
    );

    // Clean up.
    remove(test_path);
}

/** Verifies compute_file_sha256() computes correct hash for empty file. */
static void test_compute_file_sha256_empty_file(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256-empty.txt";
    char hash[SHA256_HEX_LENGTH];

    // Create an empty test file.
    write_file(test_path, "");

    // Compute the hash.
    int result = compute_file_sha256(test_path, hash, sizeof(hash));
    assert_int_equal(0, result);

    // Verify the hash matches expected value for empty file.
    // SHA256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    assert_string_equal(
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        hash
    );

    // Clean up.
    remove(test_path);
}

/** Verifies compute_file_sha256() returns -1 for non-existent file. */
static void test_compute_file_sha256_nonexistent(void **state)
{
    (void)state;
    char hash[SHA256_HEX_LENGTH];

    // Try to compute hash of non-existent file.
    int result = compute_file_sha256(
        "/nonexistent_xyz123/file.txt", hash, sizeof(hash)
    );
    assert_int_equal(-1, result);
}

/** Verifies compute_file_sha256() returns -1 for buffer too small. */
static void test_compute_file_sha256_buffer_too_small(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256-small.txt";
    char hash[32];

    // Create a test file.
    write_file(test_path, "test");

    // Try with buffer too small.
    int result = compute_file_sha256(test_path, hash, sizeof(hash));
    assert_int_equal(-1, result);

    // Clean up.
    remove(test_path);
}

/** Verifies compute_file_sha256() produces consistent results. */
static void test_compute_file_sha256_consistent(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256-consistent.txt";
    char hash1[SHA256_HEX_LENGTH];
    char hash2[SHA256_HEX_LENGTH];

    // Create a test file.
    write_file(test_path, "consistent content for hashing");

    // Compute hash twice.
    int result1 = compute_file_sha256(test_path, hash1, sizeof(hash1));
    int result2 = compute_file_sha256(test_path, hash2, sizeof(hash2));

    // Verify both succeeded and match.
    assert_int_equal(0, result1);
    assert_int_equal(0, result2);
    assert_string_equal(hash1, hash2);

    // Clean up.
    remove(test_path);
}

/** Verifies compute_file_sha256() produces 64 hex characters. */
static void test_compute_file_sha256_hex_format(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256-hex.txt";
    char hash[SHA256_HEX_LENGTH];

    // Create a test file.
    write_file(test_path, "hex format test");

    // Compute the hash.
    int result = compute_file_sha256(test_path, hash, sizeof(hash));
    assert_int_equal(0, result);

    // Verify length is 64 characters.
    assert_int_equal(64, strlen(hash));

    // Verify all characters are valid hex.
    for (int i = 0; i < 64; i++)
    {
        char c = hash[i];
        int is_hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        assert_true(is_hex);
    }

    // Clean up.
    remove(test_path);
}

/** Verifies compute_file_sha256() handles binary content. */
static void test_compute_file_sha256_binary_content(void **state)
{
    (void)state;
    const char *test_path = "/tmp/iso-builder-test-sha256-binary.bin";
    char hash[SHA256_HEX_LENGTH];

    // Create a binary file with null bytes.
    FILE *f = fopen(test_path, "wb");
    assert_non_null(f);
    unsigned char binary_data[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x00, 0x00};
    fwrite(binary_data, 1, sizeof(binary_data), f);
    fclose(f);

    // Compute the hash.
    int result = compute_file_sha256(test_path, hash, sizeof(hash));
    assert_int_equal(0, result);

    // Verify hash is valid (64 hex chars).
    assert_int_equal(64, strlen(hash));

    // Clean up.
    remove(test_path);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // compute_file_sha256 tests
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_known_content, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_empty_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_nonexistent, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_buffer_too_small, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_consistent, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_hex_format, setup, teardown),
        cmocka_unit_test_setup_teardown(test_compute_file_sha256_binary_content, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
