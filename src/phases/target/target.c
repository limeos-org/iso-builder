/**
 * This code is responsible for orchestrating the target phase.
 */

#include "all.h"

int run_target_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version
)
{
    // Create target rootfs from base.
    if (create_target_rootfs(base_rootfs_dir, rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create target rootfs");
        return -1;
    }

    // Configure target rootfs.
    if (configure_target_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to configure target rootfs");
        return -1;
    }

    // Clean up apt directories.
    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -1;
    }

    // Package target rootfs as tarball.
    if (package_target_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package target rootfs");
        return -1;
    }

    // Remove the target rootfs directory.
    rm_rf(rootfs_dir);
    LOG_INFO("Phase 3 complete: Target rootfs packaged");
    return 0;
}
