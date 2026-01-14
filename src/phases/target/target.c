/**
 * This code is responsible for orchestrating the target phase.
 */

#include "all.h"

int run_target_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version, int use_cache
)
{
    if (create_target_rootfs(base_rootfs_dir, rootfs_dir, use_cache) != 0)
    {
        LOG_ERROR("Failed to create target rootfs");
        return -1;
    }

    if (brand_target_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to brand target rootfs");
        return -1;
    }

    // Remove firmware that may have been reinstalled by linux-image package.
    cleanup_unnecessary_firmware(rootfs_dir);

    // Clean up apt cache after all packages are installed.
    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -1;
    }

    if (package_target_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package target rootfs");
        return -1;
    }

    rm_rf(rootfs_dir);
    LOG_INFO("Phase 3 complete: Target rootfs packaged");
    return 0;
}
