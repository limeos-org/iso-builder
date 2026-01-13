/**
 * This code is responsible for testing the GRUB branding configuration
 * functions for silent boot setup.
 */

#include "../../../all.h"

/** Test rootfs path for GRUB tests. */
static char test_rootfs[256];

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;

    // Create a unique test directory.
    snprintf(test_rootfs, sizeof(test_rootfs), "/tmp/iso-builder-test-grub-%d", getpid());
    mkdir_p(test_rootfs);

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

/** Verifies brand_grub() creates directory structure and valid config. */
static void test_brand_grub_creates_valid_config(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify grub.d directory exists.
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s/etc/default/grub.d", test_rootfs);
    assert_int_equal(1, file_exists(dir_path));

    // Read and verify config file.
    char cfg_path[512];
    snprintf(cfg_path, sizeof(cfg_path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);

    FILE *f = fopen(cfg_path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify all required config elements.
    assert_non_null(strstr(content, "GRUB_TIMEOUT=0"));
    assert_non_null(strstr(content, "GRUB_TIMEOUT_STYLE=hidden"));
    assert_non_null(strstr(content, "quiet splash"));
    assert_non_null(strstr(content, "GRUB_DISTRIBUTOR=\"" CONFIG_OS_NAME "\""));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // brand_grub tests.
        cmocka_unit_test_setup_teardown(test_brand_grub_creates_valid_config, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
