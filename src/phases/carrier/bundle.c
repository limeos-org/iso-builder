/**
 * This code is responsible for downloading and bundling .deb packages
 * into the carrier rootfs for the installer to use.
 */

#include "all.h"

int bundle_packages(const char *carrier_rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    LOG_INFO("Bundling bootloader packages into carrier rootfs...");

    // Create the BIOS packages directory.
    snprintf(dir_path, sizeof(dir_path), "%s" CONFIG_PACKAGES_BIOS_DIR, carrier_rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_ERROR("Failed to create BIOS packages directory");
        return -1;
    }

    // Create the EFI packages directory.
    snprintf(dir_path, sizeof(dir_path), "%s" CONFIG_PACKAGES_EFI_DIR, carrier_rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_ERROR("Failed to create EFI packages directory");
        return -1;
    }

    // Download BIOS bootloader packages.
    LOG_INFO("Downloading BIOS packages...");
    snprintf(
        command, sizeof(command),
        "cd \"%s\" && apt-get download %s",
        CONFIG_PACKAGES_BIOS_DIR, CONFIG_BIOS_PACKAGES
    );
    if (run_chroot(carrier_rootfs_path, command) != 0)
    {
        LOG_ERROR("Failed to download BIOS packages");
        return -2;
    }

    // Download EFI bootloader packages.
    LOG_INFO("Downloading EFI packages...");
    snprintf(
        command, sizeof(command),
        "cd \"%s\" && apt-get download %s",
        CONFIG_PACKAGES_EFI_DIR, CONFIG_EFI_PACKAGES
    );
    if (run_chroot(carrier_rootfs_path, command) != 0)
    {
        LOG_ERROR("Failed to download EFI packages");
        return -2;
    }

    LOG_INFO("Bootloader packages bundled successfully");

    return 0;
}
