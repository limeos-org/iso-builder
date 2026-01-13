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

// --- brand_grub tests ---

/** Verifies brand_grub() creates grub.d directory. */
static void test_brand_grub_creates_grub_d_dir(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify grub.d directory exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies brand_grub() creates distributor.cfg file. */
static void test_brand_grub_creates_config_file(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify config file exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies brand_grub() sets GRUB_TIMEOUT to 0. */
static void test_brand_grub_sets_timeout_zero(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read config file.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify GRUB_TIMEOUT=0.
    assert_non_null(strstr(content, "GRUB_TIMEOUT=0"));
}

/** Verifies brand_grub() sets GRUB_TIMEOUT_STYLE to hidden. */
static void test_brand_grub_sets_timeout_style_hidden(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read config file.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify GRUB_TIMEOUT_STYLE=hidden.
    assert_non_null(strstr(content, "GRUB_TIMEOUT_STYLE=hidden"));
}

/** Verifies brand_grub() sets quiet splash in cmdline. */
static void test_brand_grub_sets_quiet_splash(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read config file.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify quiet splash is in cmdline.
    assert_non_null(strstr(content, "quiet splash"));
}

/** Verifies brand_grub() sets GRUB_DISTRIBUTOR. */
static void test_brand_grub_sets_distributor(void **state)
{
    (void)state;

    // Apply GRUB branding.
    int result = brand_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read config file.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/default/grub.d/distributor.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify GRUB_DISTRIBUTOR is set.
    assert_non_null(strstr(content, "GRUB_DISTRIBUTOR=\"" CONFIG_OS_NAME "\""));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // brand_grub tests
        cmocka_unit_test_setup_teardown(test_brand_grub_creates_grub_d_dir, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_grub_creates_config_file, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_grub_sets_timeout_zero, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_grub_sets_timeout_style_hidden, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_grub_sets_quiet_splash, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_grub_sets_distributor, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
