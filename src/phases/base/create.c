/**
 * This code is responsible for creating a minimal base rootfs that both
 * target and live environments will be derived from.
 */

#include "all.h"

int create_base_rootfs(const char *path)
{
    LOG_INFO("Creating base rootfs at %s", path);

    // Quote the path for shell safety.
    char quoted_path[COMMON_MAX_QUOTED_LENGTH];
    if (shell_escape_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Run debootstrap to create a minimal Debian rootfs.
    char command[COMMON_MAX_COMMAND_LENGTH];
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CONFIG_DEBIAN_RELEASE, quoted_path
    );
    if (run_command_indented(command) != 0)
    {
        LOG_ERROR("Command failed: debootstrap");
        return -2;
    }

    // Enable Debian's non-free-firmware section.
    // Required for modern hardware (e.g. GPU and Wi-Fi) to function, as Debian
    // ships essential firmware separately from main starting with Debian 12.
    LOG_INFO("Configuring apt sources...");
    char sources_content[256];
    char sources_path[COMMON_MAX_PATH_LENGTH];
    snprintf(
        sources_content, sizeof(sources_content),
        "deb http://deb.debian.org/debian %s main non-free-firmware\n",
        CONFIG_DEBIAN_RELEASE
    );
    snprintf(sources_path, sizeof(sources_path), "%s/etc/apt/sources.list", path);
    if (write_file(sources_path, sources_content) != 0)
    {
        LOG_ERROR("Failed to configure apt sources");
        return -3;
    }

    // Update package lists for later package installation.
    LOG_INFO("Updating package lists...");
    if (run_chroot_indented(path, "apt-get update") != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -4;
    }

    // Pre-create initramfs configuration before installing packages. When
    // linux-image-* is installed, it triggers update-initramfs, which reads
    // /etc/initramfs-tools/ to decide what to include. Our config must exist
    // BEFORE apt-get install runs. We use conf.d/ drop-ins because they
    // survive package installation; files like /etc/initramfs-tools/modules
    // are dpkg conffiles that get replaced when initramfs-tools installs.
    LOG_INFO("Pre-configuring initramfs for hardware support...");
    char initramfs_conf_dir[COMMON_MAX_PATH_LENGTH];
    snprintf(
        initramfs_conf_dir, sizeof(initramfs_conf_dir),
        "%s/etc/initramfs-tools/conf.d", path
    );
    if (mkdir_p(initramfs_conf_dir) != 0)
    {
        LOG_ERROR("Failed to create initramfs-tools directory");
        return -5;
    }

    // Set MODULES=most to include drivers for hardware not on the build host
    // (VMs, NVMe, USB). The default MODULES=dep only includes drivers for
    // detected hardware, which breaks when the ISO boots elsewhere.
    char driver_policy_path[COMMON_MAX_PATH_LENGTH];
    snprintf(
        driver_policy_path, sizeof(driver_policy_path),
        "%s/etc/initramfs-tools/conf.d/driver-policy.conf", path
    );
    if (write_file(driver_policy_path, "MODULES=most\n") != 0)
    {
        LOG_ERROR("Failed to create initramfs conf.d");
        return -6;
    }

    LOG_INFO("Base rootfs created successfully");

    return 0;
}
