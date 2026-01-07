/**
 * This code is responsible for creating and stripping the root filesystem.
 */

#include "all.h"

/** The Debian release to use for the base rootfs. */
#define CREATE_DEBIAN_RELEASE "bookworm"

int create_rootfs(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    LOG_INFO("Creating rootfs at %s", path);

    // Run debootstrap to create a minimal Debian rootfs.
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CREATE_DEBIAN_RELEASE, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Install required packages into the rootfs.
    LOG_INFO("Installing required packages...");
    snprintf(
        command, sizeof(command),
        "chroot %s apt-get update", path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -1;
    }

    // Install all packages needed by the installation environment.
    snprintf(
        command, sizeof(command),
        "chroot %s apt-get install -y --no-install-recommends "
        "linux-image-amd64 systemd-sysv plymouth plymouth-themes live-boot libncurses6 parted",
        path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -1;
    }

    // Copy kernel to standard path for boot loaders.
    snprintf(
        command, sizeof(command),
        "cp $(ls %s/boot/vmlinuz-* | head -1) %s/boot/vmlinuz",
        path, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy initrd to standard path for boot loaders.
    snprintf(
        command, sizeof(command),
        "cp $(ls %s/boot/initrd.img-* | head -1) %s/boot/initrd.img",
        path, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -1;
    }

    LOG_INFO("Rootfs created successfully");

    return 0;
}

int strip_rootfs(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    // Log the start of rootfs stripping.
    LOG_INFO("Stripping rootfs at %s", path);

    // Remove documentation files.
    snprintf(
        command, sizeof(command),
        "rm -rf %s/usr/share/doc %s/usr/share/man %s/usr/share/info",
        path, path, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove non-English locales.
    snprintf(
        command, sizeof(command),
        "find %s/usr/share/locale -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove apt cache.
    snprintf(command, sizeof(command), "rm -rf %s/var/cache/apt/*", path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove apt lists.
    snprintf(command, sizeof(command), "rm -rf %s/var/lib/apt/lists/*", path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    LOG_INFO("Rootfs stripped successfully");

    return 0;
}
