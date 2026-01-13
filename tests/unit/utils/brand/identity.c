/**
 * This code is responsible for testing the OS identity branding functions,
 * including os-release and issue file generation.
 */

#include "../../../all.h"

/** Test rootfs path for branding tests. */
static char test_rootfs[256];

/** Sets up the test environment before each test. */
static int setup(void **state)
{
    (void)state;

    // Create a unique test directory.
    snprintf(test_rootfs, sizeof(test_rootfs), "/tmp/iso-builder-test-brand-%d", getpid());
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

// --- brand_os_identity tests ---

/** Verifies brand_os_identity() creates os-release file. */
static void test_brand_os_identity_creates_os_release(void **state)
{
    (void)state;

    // Apply branding.
    int result = brand_os_identity(test_rootfs, "1.0.0");
    assert_int_equal(0, result);

    // Verify os-release exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/os-release", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies brand_os_identity() creates issue file. */
static void test_brand_os_identity_creates_issue(void **state)
{
    (void)state;

    // Apply branding.
    int result = brand_os_identity(test_rootfs, "1.0.0");
    assert_int_equal(0, result);

    // Verify issue exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/issue", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies brand_os_identity() creates issue.net file. */
static void test_brand_os_identity_creates_issue_net(void **state)
{
    (void)state;

    // Apply branding.
    int result = brand_os_identity(test_rootfs, "1.0.0");
    assert_int_equal(0, result);

    // Verify issue.net exists.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/issue.net", test_rootfs);
    assert_int_equal(1, file_exists(path));
}

/** Verifies brand_os_identity() strips v prefix from version. */
static void test_brand_os_identity_strips_v_prefix(void **state)
{
    (void)state;

    // Apply branding with v prefix.
    int result = brand_os_identity(test_rootfs, "v2.0.0");
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

/** Verifies brand_os_identity() includes OS name in os-release. */
static void test_brand_os_identity_includes_os_name(void **state)
{
    (void)state;

    // Apply branding.
    int result = brand_os_identity(test_rootfs, "1.0.0");
    assert_int_equal(0, result);

    // Read os-release.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/os-release", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify NAME field is present.
    assert_non_null(strstr(content, "NAME=\"" CONFIG_OS_NAME "\""));
}

/** Verifies brand_os_identity() includes ID_LIKE in os-release. */
static void test_brand_os_identity_includes_id_like(void **state)
{
    (void)state;

    // Apply branding.
    int result = brand_os_identity(test_rootfs, "1.0.0");
    assert_int_equal(0, result);

    // Read os-release.
    char path[512];
    snprintf(path, sizeof(path), "%s/etc/os-release", test_rootfs);

    FILE *f = fopen(path, "r");
    assert_non_null(f);

    char content[1024];
    size_t bytes = fread(content, 1, sizeof(content) - 1, f);
    content[bytes] = '\0';
    fclose(f);

    // Verify ID_LIKE field is present.
    assert_non_null(strstr(content, "ID_LIKE=" CONFIG_OS_BASE_ID));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // brand_os_identity tests
        cmocka_unit_test_setup_teardown(test_brand_os_identity_creates_os_release, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_os_identity_creates_issue, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_os_identity_creates_issue_net, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_os_identity_strips_v_prefix, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_os_identity_includes_os_name, setup, teardown),
        cmocka_unit_test_setup_teardown(test_brand_os_identity_includes_id_like, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
