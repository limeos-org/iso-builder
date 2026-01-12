/**
 * This code is responsible for downloading and bundling .deb packages
 * into the carrier rootfs for the installer to use.
 */

#include "all.h"

/** Cache subdirectory for BIOS packages. */
#define CACHE_BIOS_DIR "bios-packages"

/** Cache subdirectory for EFI packages. */
#define CACHE_EFI_DIR "efi-packages"

static int cache_has_packages(const char *cache_dir, const char *subdir)
{
    char path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    snprintf(path, sizeof(path), "%s/%s", cache_dir, subdir);

    // Check if directory exists and has .deb files.
    snprintf(command, sizeof(command), "ls %s/*.deb >/dev/null 2>&1", path);
    return (system(command) == 0);
}

static int copy_cached_packages(
    const char *cache_dir, const char *subdir,
    const char *carrier_rootfs_path, const char *dest_dir
)
{
    char src_path[COMMAND_PATH_MAX_LENGTH];
    char dest_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    snprintf(src_path, sizeof(src_path), "%s/%s", cache_dir, subdir);
    snprintf(dest_path, sizeof(dest_path), "%s%s", carrier_rootfs_path, dest_dir);

    snprintf(command, sizeof(command), "cp %s/*.deb %s/", src_path, dest_path);
    return run_command(command);
}

static int save_packages_to_cache(
    const char *carrier_rootfs_path, const char *src_dir,
    const char *cache_dir, const char *subdir
)
{
    char src_path[COMMAND_PATH_MAX_LENGTH];
    char dest_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];

    snprintf(src_path, sizeof(src_path), "%s%s", carrier_rootfs_path, src_dir);
    snprintf(dest_path, sizeof(dest_path), "%s/%s", cache_dir, subdir);

    // Create cache subdirectory.
    if (mkdir_p(dest_path) != 0)
    {
        return -1;
    }

    snprintf(command, sizeof(command), "cp %s/*.deb %s/", src_path, dest_path);
    return run_command(command);
}

int bundle_packages_with_cache(const char *carrier_rootfs_path, const char *cache_dir)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    char command[COMMAND_MAX_LENGTH];
    int bios_cached = 0;
    int efi_cached = 0;

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

    // Check for cached BIOS packages.
    if (cache_dir && cache_has_packages(cache_dir, CACHE_BIOS_DIR))
    {
        LOG_INFO("Using cached BIOS packages...");
        if (copy_cached_packages(cache_dir, CACHE_BIOS_DIR, carrier_rootfs_path, CONFIG_PACKAGES_BIOS_DIR) == 0)
        {
            bios_cached = 1;
        }
        else
        {
            LOG_WARNING("Failed to copy cached BIOS packages, downloading...");
        }
    }

    // Download BIOS bootloader packages if not cached.
    if (!bios_cached)
    {
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

        // Cache the downloaded packages.
        if (cache_dir)
        {
            if (save_packages_to_cache(carrier_rootfs_path, CONFIG_PACKAGES_BIOS_DIR, cache_dir, CACHE_BIOS_DIR) != 0)
            {
                LOG_WARNING("Failed to cache BIOS packages");
            }
        }
    }

    // Check for cached EFI packages.
    if (cache_dir && cache_has_packages(cache_dir, CACHE_EFI_DIR))
    {
        LOG_INFO("Using cached EFI packages...");
        if (copy_cached_packages(cache_dir, CACHE_EFI_DIR, carrier_rootfs_path, CONFIG_PACKAGES_EFI_DIR) == 0)
        {
            efi_cached = 1;
        }
        else
        {
            LOG_WARNING("Failed to copy cached EFI packages, downloading...");
        }
    }

    // Download EFI bootloader packages if not cached.
    if (!efi_cached)
    {
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

        // Cache the downloaded packages.
        if (cache_dir)
        {
            if (save_packages_to_cache(carrier_rootfs_path, CONFIG_PACKAGES_EFI_DIR, cache_dir, CACHE_EFI_DIR) != 0)
            {
                LOG_WARNING("Failed to cache EFI packages");
            }
        }
    }

    LOG_INFO("Bootloader packages bundled successfully");

    return 0;
}

int bundle_packages(const char *carrier_rootfs_path)
{
    // Legacy function without caching support.
    return bundle_packages_with_cache(carrier_rootfs_path, NULL);
}
