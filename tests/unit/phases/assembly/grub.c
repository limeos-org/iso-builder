/**
 * This code is responsible for testing the GRUB UEFI boot configuration
 * functions for ISO assembly.
 */

#include "../../../all.h"

/** Test rootfs path for assembly GRUB tests. */
static char test_rootfs[256];

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;

    // Create a unique test directory.
    snprintf(test_rootfs, sizeof(test_rootfs), "/tmp/iso-builder-test-asm-grub-%d", getpid());
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

// --- setup_grub tests ---

/** Verifies setup_grub() creates directory structure and valid config. */
static void test_setup_grub_creates_valid_config(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify grub directory exists.
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s/boot/grub", test_rootfs);
    assert_int_equal(1, file_exists(dir_path));

    // Read and verify grub.cfg.
    char cfg_path[512];
    snprintf(cfg_path, sizeof(cfg_path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(cfg_path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify all required config elements.
    assert_non_null(strstr(content, "set timeout=0"));
    assert_non_null(strstr(content, "set timeout_style=hidden"));
    assert_non_null(strstr(content, CONFIG_BOOT_KERNEL_PATH));
    assert_non_null(strstr(content, CONFIG_BOOT_INITRD_PATH));
    assert_non_null(strstr(content, CONFIG_GRUB_MENU_ENTRY_NAME));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // setup_grub tests
        cmocka_unit_test_setup_teardown(test_setup_grub_creates_valid_config, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
