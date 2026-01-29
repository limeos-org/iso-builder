/**
 * This code is responsible for embedding the target rootfs tarball into the
 * live rootfs so the installer can access it during installation.
 */

#include "all.h"

int embed_target_rootfs(const char *live_rootfs_path, const char *tarball_path)
{
    LOG_INFO("Embedding target rootfs tarball into live rootfs...");

    // Create the target directory within the live rootfs.
    char dst_dir[COMMON_MAX_PATH_LENGTH];
    snprintf(dst_dir, sizeof(dst_dir), "%s/usr/share/limeos", live_rootfs_path);
    if (common.mkdir_p(dst_dir) != 0)
    {
        LOG_ERROR("Failed to create limeos directory in live rootfs");
        return -1;
    }

    // Copy the tarball to the live rootfs.
    char dst_path[COMMON_MAX_PATH_LENGTH];
    snprintf(dst_path, sizeof(dst_path), "%s" CONFIG_TARGET_ROOTFS_PATH, live_rootfs_path);
    if (common.copy_file(tarball_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy target rootfs tarball");
        return -2;
    }

    LOG_INFO("Target rootfs tarball embedded successfully");

    return 0;
}
