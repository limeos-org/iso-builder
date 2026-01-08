/**
 * This code is responsible for creating a minimal Debian root filesystem.
 */

#include "all.h"

int create_rootfs(const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating rootfs at %s", path);

    // Quote the path to prevent shell injection.
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
        "linux-image-amd64 systemd-sysv plymouth plymouth-themes live-boot "
        "libncurses6 parted dosfstools") != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -3;
    }

    // Copy kernel to standard path for boot loaders.
    char kernel_pattern[COMMAND_PATH_MAX_LENGTH];
    char kernel_src[COMMAND_PATH_MAX_LENGTH];
    char kernel_dst[COMMAND_PATH_MAX_LENGTH];
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
    char initrd_pattern[COMMAND_PATH_MAX_LENGTH];
    char initrd_src[COMMAND_PATH_MAX_LENGTH];
    char initrd_dst[COMMAND_PATH_MAX_LENGTH];
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
