/**
 * This code is responsible for domain-specific filesystem operations
 * for the ISO builder.
 */

#include "all.h"

int cleanup_apt_directories(const char *rootfs_path)
{
    char dir_path[COMMON_MAX_PATH_LENGTH];

    // Remove apt cache.
    snprintf(dir_path, sizeof(dir_path), "%s/var/cache/apt", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        return -1;
    }

    // Recreate apt cache directory (best-effort).
    mkdir_p(dir_path);

    // Remove apt lists.
    snprintf(dir_path, sizeof(dir_path), "%s/var/lib/apt/lists", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        return -2;
    }

    // Recreate apt lists directory (best-effort).
    mkdir_p(dir_path);

    return 0;
}

int copy_kernel_and_initrd(const char *rootfs_path)
{
    char pattern[COMMON_MAX_PATH_LENGTH];
    char src[COMMON_MAX_PATH_LENGTH];
    char dst[COMMON_MAX_PATH_LENGTH];

    // Copy kernel to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/vmlinuz-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        return -1;
    }
    snprintf(dst, sizeof(dst), "%s/boot/vmlinuz", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        return -2;
    }

    // Copy initrd to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/initrd.img-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        return -3;
    }
    snprintf(dst, sizeof(dst), "%s/boot/initrd.img", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        return -4;
    }

    return 0;
}

int cleanup_versioned_boot_files(const char *rootfs_path)
{
    // Construct the boot directory path.
    char boot_path[COMMON_MAX_PATH_LENGTH];
    snprintf(boot_path, sizeof(boot_path), "%s/boot", rootfs_path);

    // Quote the boot path for shell safety.
    char quoted_boot[COMMON_MAX_QUOTED_LENGTH];
    if (shell_escape_path(boot_path, quoted_boot, sizeof(quoted_boot)) != 0)
    {
        return -1;
    }

    // Remove versioned kernel, initrd, config, and System.map files.
    // These are created by the kernel package but not needed after
    // copying to generic names (vmlinuz, initrd.img).
    char command[COMMON_MAX_COMMAND_LENGTH];
    snprintf(
        command, sizeof(command),
        "find %s -maxdepth 1 \\( "
        "-name 'vmlinuz-*' -o "
        "-name 'initrd.img-*' -o "
        "-name 'config-*' -o "
        "-name 'System.map-*' "
        "\\) -type f -delete 2>/dev/null",
        quoted_boot
    );

    // Ignore errors since files may not exist.
    run_command(command);

    return 0;
}
