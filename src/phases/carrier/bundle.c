/**
 * This code is responsible for downloading and bundling .deb packages
 * into the carrier rootfs for the installer to use.
 */

#include "all.h"

/**
 * Copies .deb files from source directory to destination directory.
 */
static int copy_debs(const char *src_dir, const char *dst_dir)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];

    // Quote paths for shell safety.
    if (shell_quote_path(src_dir, quoted_src, sizeof(quoted_src)) != 0)
    {
        return -1;
    }
    if (shell_quote_path(dst_dir, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        return -1;
    }

    // Copy .deb files.
    snprintf(command, sizeof(command), "cp %s/*.deb %s/", quoted_src, quoted_dst);
    return run_command(command);
}

/**
 * Checks if cached bundles exist for a given type (bios or efi).
 */
static int bundle_cache_exists(const char *type)
{
    // Get cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return 0;
    }

    // Construct bundle directory path.
    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/%s", cache_dir, type);

    // Check if directory exists and has .deb files.
    char pattern[COMMAND_PATH_MAX_LENGTH];
    snprintf(pattern, sizeof(pattern), "%s/*.deb", bundle_dir);

    // Look for the first matching .deb file.
    char first_match[COMMAND_PATH_MAX_LENGTH];
    return find_first_glob(pattern, first_match, sizeof(first_match)) == 0;
}

/**
 * Restores bundled packages from cache.
 */
static int restore_bundles(const char *type, const char *dst_dir)
{
    // Get cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return -1;
    }

    // Construct bundle directory path.
    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/%s", cache_dir, type);

    // Copy .deb files from cache to destination.
    LOG_INFO("Restoring %s packages from cache...", type);
    return copy_debs(bundle_dir, dst_dir);
}

/**
 * Saves bundled packages to cache.
 */
static int save_bundles(const char *type, const char *src_dir)
{
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        LOG_WARNING("Failed to cache %s packages: cannot determine cache directory", type);
        return -1;
    }

    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/%s", cache_dir, type);

    if (mkdir_p(bundle_dir) != 0)
    {
        LOG_WARNING("Failed to cache %s packages: cannot create directory", type);
        return -1;
    }

    if (copy_debs(src_dir, bundle_dir) != 0)
    {
        LOG_WARNING("Failed to cache %s packages: copy failed", type);
        return -1;
    }

    return 0;
}

/**
 * Downloads packages using apt-get download in chroot.
 */
static int download_packages(const char *rootfs, const char *dest_dir, const char *packages)
{
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "cd \"%s\" && apt-get download %s", dest_dir, packages);
    return run_chroot(rootfs, command);
}

int bundle_packages(const char *carrier_rootfs_path, int use_cache)
{
    LOG_INFO("Bundling bootloader packages into carrier rootfs...");

    // Create BIOS packages directory.
    char bios_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bios_dir, sizeof(bios_dir), "%s" CONFIG_PACKAGES_BIOS_DIR, carrier_rootfs_path);
    if (mkdir_p(bios_dir) != 0)
    {
        LOG_ERROR("Failed to create BIOS packages directory");
        return -1;
    }

    // Create EFI packages directory.
    char efi_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(efi_dir, sizeof(efi_dir), "%s" CONFIG_PACKAGES_EFI_DIR, carrier_rootfs_path);
    if (mkdir_p(efi_dir) != 0)
    {
        LOG_ERROR("Failed to create EFI packages directory");
        return -1;
    }

    // Try to restore BIOS packages from cache.
    int bios_cached = use_cache && bundle_cache_exists("bios");
    if (bios_cached)
    {
        if (restore_bundles("bios", bios_dir) != 0)
        {
            LOG_WARNING("Failed to restore BIOS packages from cache, downloading...");
            bios_cached = 0;
        }
    }

    // Download BIOS packages if not cached.
    if (!bios_cached)
    {
        LOG_INFO("Downloading BIOS packages...");
        if (download_packages(carrier_rootfs_path, CONFIG_PACKAGES_BIOS_DIR, CONFIG_BIOS_PACKAGES) != 0)
        {
            LOG_ERROR("Failed to download BIOS packages");
            return -2;
        }
        if (use_cache)
        {
            save_bundles("bios", bios_dir);
        }
    }

    // Try to restore EFI packages from cache.
    int efi_cached = use_cache && bundle_cache_exists("efi");
    if (efi_cached)
    {
        if (restore_bundles("efi", efi_dir) != 0)
        {
            LOG_WARNING("Failed to restore EFI packages from cache, downloading...");
            efi_cached = 0;
        }
    }

    // Download EFI packages if not cached.
    if (!efi_cached)
    {
        LOG_INFO("Downloading EFI packages...");
        if (download_packages(carrier_rootfs_path, CONFIG_PACKAGES_EFI_DIR, CONFIG_EFI_PACKAGES) != 0)
        {
            LOG_ERROR("Failed to download EFI packages");
            return -2;
        }
        if (use_cache)
        {
            save_bundles("efi", efi_dir);
        }
    }

    LOG_INFO("Bootloader packages bundled successfully");

    return 0;
}
