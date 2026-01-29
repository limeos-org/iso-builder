/**
 * This code is responsible for orchestrating the target phase.
 */

#include "all.h"

int run_target_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version
)
{
    if (create_target_rootfs(base_rootfs_dir, rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create target rootfs");
        return -1;
    }

    if (configure_target_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to configure target rootfs");
        return -2;
    }

    if (cleanup_apt_directories(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to cleanup apt directories");
        return -3;
    }

    if (package_target_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package target rootfs");
        return -4;
    }

    // Remove the target rootfs directory.
    common.rm_rf(rootfs_dir);

    LOG_INFO("Phase 3 complete: Target rootfs packaged");
    
    return 0;
}
