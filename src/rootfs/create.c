/**
 * This code is responsible for creating and stripping the root filesystem.
 */

#include "all.h"
#include <glob.h>

/**
 * Finds the first file matching a glob pattern.
 *
 * @param pattern The glob pattern to match.
 * @param out_path The buffer to store the matched path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates a match was found.
 * @return - `-1` - Indicates no match was found.
 */
static int find_first_glob(
    const char *pattern,
    char *out_path,
    size_t buffer_length
)
{
    glob_t glob_result;
    int ret;

    ret = glob(pattern, GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0)
    {
        globfree(&glob_result);
        return -1;
    }

    strncpy(out_path, glob_result.gl_pathv[0], buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    globfree(&glob_result);
    return 0;
}

int create_rootfs(const char *path)
{
    char command[MAX_COMMAND_LENGTH];
    char quoted_path[SHELL_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating rootfs at %s", path);

    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Run debootstrap to create a minimal Debian rootfs.
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CONFIG_DEBIAN_RELEASE, quoted_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: debootstrap");
        return -1;
    }

    // Install required packages into the rootfs.
    LOG_INFO("Installing required packages...");
    if (run_chroot(path, "apt-get update") != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -2;
    }

    // Install all packages needed by the installation environment.
    if (run_chroot(path,
        "apt-get install -y --no-install-recommends "
        "linux-image-amd64 systemd-sysv plymouth plymouth-themes live-boot libncurses6 parted") != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -3;
    }

    // Copy kernel to standard path for boot loaders.
    char kernel_pattern[MAX_PATH_LENGTH];
    char kernel_src[MAX_PATH_LENGTH];
    char kernel_dst[MAX_PATH_LENGTH];

    snprintf(kernel_pattern, sizeof(kernel_pattern), "%s/boot/vmlinuz-*", path);
    if (find_first_glob(kernel_pattern, kernel_src, sizeof(kernel_src)) != 0)
    {
        LOG_ERROR("Failed to find kernel");
        return -4;
    }

    snprintf(kernel_dst, sizeof(kernel_dst), "%s/boot/vmlinuz", path);
    if (copy_file(kernel_src, kernel_dst) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -4;
    }

    // Copy initrd to standard path for boot loaders.
    char initrd_pattern[MAX_PATH_LENGTH];
    char initrd_src[MAX_PATH_LENGTH];
    char initrd_dst[MAX_PATH_LENGTH];

    snprintf(initrd_pattern, sizeof(initrd_pattern), "%s/boot/initrd.img-*", path);
    if (find_first_glob(initrd_pattern, initrd_src, sizeof(initrd_src)) != 0)
    {
        LOG_ERROR("Failed to find initrd");
        return -5;
    }

    snprintf(initrd_dst, sizeof(initrd_dst), "%s/boot/initrd.img", path);
    if (copy_file(initrd_src, initrd_dst) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -5;
    }

    LOG_INFO("Rootfs created successfully");

    return 0;
}

int strip_rootfs(const char *path)
{
    char dir_path[MAX_PATH_LENGTH];

    // Log the start of rootfs stripping.
    LOG_INFO("Stripping rootfs at %s", path);

    // Remove documentation files.
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/doc", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove doc directory");
        return -1;
    }

    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/man", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove man directory");
        return -1;
    }

    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -1;
    }

    // Remove non-English locales.
    char command[MAX_COMMAND_LENGTH];
    char quoted_locale_dir[SHELL_QUOTED_MAX_LENGTH];

    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (shell_quote_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -2;
    }

    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -2;
    }

    // Remove apt cache.
    snprintf(dir_path, sizeof(dir_path), "%s/var/cache/apt", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt cache");
        return -3;
    }

    // Recreate apt cache directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt cache directory");
    }

    // Remove apt lists.
    snprintf(dir_path, sizeof(dir_path), "%s/var/lib/apt/lists", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt lists");
        return -4;
    }

    // Recreate apt lists directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt lists directory");
    }

    LOG_INFO("Rootfs stripped successfully");

    return 0;
}
