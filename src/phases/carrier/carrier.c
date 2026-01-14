/**
 * This code is responsible for orchestrating the carrier phase.
 */

#include "all.h"

int run_carrier_phase(
    const char *base_rootfs_dir,
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir,
    const char *version,
    int use_cache
)
{
    if (create_carrier_rootfs(base_rootfs_dir, rootfs_dir, use_cache) != 0)
    {
        LOG_ERROR("Failed to create carrier rootfs");
        return -1;
    }

    if (brand_carrier_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to brand carrier rootfs");
        return -1;
    }

    if (embed_target_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to embed target rootfs");
        return -1;
    }

    if (install_carrier_components(rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        return -1;
    }

    if (configure_carrier_init(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure init");
        return -1;
    }

    if (bundle_packages(rootfs_dir, use_cache) != 0)
    {
        LOG_ERROR("Failed to bundle packages");
        return -1;
    }

    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -1;
    }

    LOG_INFO("Phase 4 complete: Carrier rootfs created");
    return 0;
}
