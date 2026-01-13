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

/** Verifies setup_grub() creates boot/grub directory. */
static void test_setup_grub_creates_grub_dir(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify grub directory exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies setup_grub() creates grub.cfg file. */
static void test_setup_grub_creates_grub_cfg(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Verify grub.cfg exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies setup_grub() sets timeout to 0. */
static void test_setup_grub_sets_timeout_zero(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read grub.cfg.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify timeout is 0.
    assert_non_null(strstr(content, "set timeout=0"));
}

/** Verifies setup_grub() sets timeout_style to hidden. */
static void test_setup_grub_sets_timeout_style_hidden(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read grub.cfg.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify timeout_style is hidden.
    assert_non_null(strstr(content, "set timeout_style=hidden"));
}

/** Verifies setup_grub() includes kernel path. */
static void test_setup_grub_includes_kernel_path(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read grub.cfg.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify kernel path is present.
    assert_non_null(strstr(content, CONFIG_BOOT_KERNEL_PATH));
}

/** Verifies setup_grub() includes initrd path. */
static void test_setup_grub_includes_initrd_path(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read grub.cfg.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify initrd path is present.
    assert_non_null(strstr(content, CONFIG_BOOT_INITRD_PATH));
}

/** Verifies setup_grub() includes menu entry name. */
static void test_setup_grub_includes_menu_entry(void **state)
{
    (void)state;

    // Set up GRUB.
    int result = setup_grub(test_rootfs);
    assert_int_equal(0, result);

    // Read grub.cfg.
    char path[512];
    snprintf(path, sizeof(path), "%s/boot/grub/grub.cfg", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[2048];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify menu entry name is present.
    assert_non_null(strstr(content, CONFIG_GRUB_MENU_ENTRY_NAME));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // setup_grub tests
        cmocka_unit_test_setup_teardown(test_setup_grub_creates_grub_dir, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_creates_grub_cfg, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_sets_timeout_zero, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_sets_timeout_style_hidden, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_includes_kernel_path, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_includes_initrd_path, setup, teardown),
        cmocka_unit_test_setup_teardown(test_setup_grub_includes_menu_entry, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
