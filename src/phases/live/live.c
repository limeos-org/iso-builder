/**
 * This code is responsible for orchestrating the live phase.
 */

#include "all.h"

int run_live_phase(
    const char *base_rootfs_dir,
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir,
    const char *version
)
{
    // Create live rootfs from base.
    if (create_live_rootfs(base_rootfs_dir, rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create live rootfs");
        return -1;
    }

    // Configure live rootfs.
    if (configure_live_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to configure live rootfs");
        return -2;
    }

    // Embed the target rootfs tarball.
    if (embed_target_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to embed target rootfs");
        return -3;
    }

    // Install LimeOS components (installer, etc.).
    if (install_live_components(rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        return -4;
    }

    // Configure autostart to launch installer on boot.
    if (configure_live_autostart(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure autostart");
        return -5;
    }

    // Clean up apt cache and lists before bundling bootloader packages.
    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -6;
    }

    // Bundle boot-mode-specific packages (GRUB for BIOS/EFI). Must happen after
    // cleanup so the packages remain in /var/cache/apt/archives/.
    if (bundle_live_packages(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to bundle packages");
        return -7;
    }

    LOG_INFO("Phase 4 complete: Live rootfs created");
    
    return 0;
}
