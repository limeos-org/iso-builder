/**
 * This code is responsible for orchestrating the payload phase.
 */

#include "all.h"

int run_payload_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version
)
{
    if (create_payload_rootfs(base_rootfs_dir, rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create payload rootfs");
        return -1;
    }

    if (brand_payload_rootfs(rootfs_dir, version) != 0)
    {
        LOG_ERROR("Failed to brand payload rootfs");
        return -1;
    }

    if (create_default_user(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create default user");
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

    if (package_payload_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package payload rootfs");
        return -1;
    }

    rm_rf(rootfs_dir);
    LOG_INFO("Phase 3 complete: Payload rootfs packaged");
    return 0;
}
