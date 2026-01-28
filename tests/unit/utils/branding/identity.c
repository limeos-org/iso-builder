/**
 * This code is responsible for testing the OS identity functions.
 */

#include "../../../all.h"

/** Test rootfs path for identity tests. */
static char test_rootfs[256];

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;

    // Create a unique test directory.
    snprintf(
        test_rootfs, sizeof(test_rootfs),
        "/tmp/iso-builder-test-identity-%d",
        getpid()
    );
    mkdir_p(test_rootfs);

    // Create the /etc directory.
    char etc_path[512];
    snprintf(etc_path, sizeof(etc_path), "%s/etc", test_rootfs);
    mkdir_p(etc_path);

    return 0;
}

/** Cleans up the test environment after each test. */
static int teardown(void **state)
{
    (void)state;

    // Remove the test directory.
    rm_rf(test_rootfs);
    return 0;
}

/** Verifies write_os_identity() strips v prefix from version. */
static void test_write_os_identity_strips_v_prefix(void **state)
{
    (void)state;

    // Write identity files with v prefix.
    int result = write_os_identity(test_rootfs, "v2.0.0");
    assert_int_equal(0, result);

    // Read os-release and verify version doesn't have v prefix.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/os-release", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify VERSION_ID doesn't have v prefix.
    assert_non_null(strstr(content, "VERSION_ID=\"2.0.0\""));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_write_os_identity_strips_v_prefix, setup, teardown
        ),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
