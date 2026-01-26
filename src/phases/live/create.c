/**
 * This code is responsible for creating the live rootfs by copying
 * from the base rootfs and installing live-specific packages.
 */

#include "all.h"

int create_live_rootfs(const char *base_path, const char *path)
{
    LOG_INFO("Creating live rootfs at %s", path);

    // Quote the base path for shell safety.
    char quoted_base[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(base_path, quoted_base, sizeof(quoted_base)) != 0)
    {
        LOG_ERROR("Failed to quote base path");
        return -1;
    }

    // Quote the destination path for shell safety.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Copy the base rootfs.
    LOG_INFO("Copying base rootfs...");
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "cp -a %s %s", quoted_base, quoted_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy base rootfs");
        return -1;
    }

    // Install live-specific packages.
    LOG_INFO("Installing live environment packages...");
    int install_result = run_chroot(path,
        "apt-get install -y --no-install-recommends " CONFIG_LIVE_PACKAGES);

    // Check if package installation succeeded.
    if (install_result != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -2;
    }

    // Add GPU drivers for early KMS initialization. Must be done AFTER package 
    // install because `dpkg` overwrites pre-seeded files. `brand_live_rootfs()`
    // will call update-initramfs later.
    if (run_chroot(path,
        "printf 'amdgpu\\ni915\\nnouveau\\nradeon\\n' >> /etc/initramfs-tools/modules") != 0)
    {
        LOG_ERROR("Failed to add GPU drivers to initramfs modules");
        return -3;
    }

    // Clean APT cache to remove downloaded .deb files.
    // Bootloader packages will be downloaded later by bundle_live_packages.
    if (run_chroot(path, "apt-get clean") != 0)
    {
        LOG_ERROR("Failed to clean APT cache");
        return -3;
    }

    // Copy kernel and initrd to standard paths for boot loaders.
    if (copy_kernel_and_initrd(path) != 0)
    {
        return -4;
    }

    LOG_INFO("Live rootfs created successfully");

    return 0;
}
