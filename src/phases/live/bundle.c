/**
 * This code is responsible for downloading and bundling bootloader .deb
 * packages (with dependencies) into the live rootfs APT cache for the
 * installer to use.
 */

#include "all.h"

/**
 * Downloads packages using apt-get download.
 *
 * Packages are downloaded to the current directory, so we cd to the
 * APT cache first.
 */
static int download_packages(const char *rootfs, const char *packages)
{
    char command[COMMON_MAX_COMMAND_LENGTH];
    snprintf(
        command, sizeof(command),
        "cd " CONFIG_APT_CACHE_DIR " && apt-get download %s",
        packages
    );
    return common.run_chroot_indented(rootfs, command);
}

int bundle_live_packages(const char *live_rootfs_path)
{
    LOG_INFO("Bundling bootloader packages into live APT cache...");

    // Construct the APT cache directory path in the live rootfs.
    char apt_cache_dir[COMMON_MAX_PATH_LENGTH];
    snprintf(apt_cache_dir, sizeof(apt_cache_dir),
        "%s" CONFIG_APT_CACHE_DIR, live_rootfs_path);

    // Ensure the APT cache directory exists.
    if (common.mkdir_p(apt_cache_dir) != 0)
    {
        LOG_ERROR("Failed to create APT cache directory");
        return -1;
    }

    // Update package lists (needed after cleanup_apt_directories removes them).
    LOG_INFO("Updating package lists...");
    if (common.run_chroot_indented(live_rootfs_path, "apt-get update") != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -2;
    }

    // Download BIOS bootloader packages.
    LOG_INFO("Downloading BIOS bootloader packages...");
    if (download_packages(live_rootfs_path, CONFIG_BIOS_PACKAGES) != 0)
    {
        LOG_ERROR("Failed to download BIOS bootloader packages");
        return -2;
    }

    // Download EFI bootloader packages.
    LOG_INFO("Downloading EFI bootloader packages...");
    if (download_packages(live_rootfs_path, CONFIG_EFI_PACKAGES) != 0)
    {
        LOG_ERROR("Failed to download EFI bootloader packages");
        return -2;
    }

    // Clean up apt lists and cache files to reduce image size.
    // Keep only the downloaded .deb files in /var/cache/apt/archives/.
    // These cleanup operations are non-critical; failures are only logged.
    if (common.run_chroot(live_rootfs_path, "rm -rf /var/lib/apt/lists/*") != 0)
    {
        LOG_WARNING("Failed to remove APT lists (non-critical)");
    }
    if (common.run_chroot(live_rootfs_path, "rm -f /var/cache/apt/*.bin") != 0)
    {
        LOG_WARNING("Failed to remove APT cache binaries (non-critical)");
    }

    LOG_INFO("Bootloader packages bundled successfully");

    return 0;
}
