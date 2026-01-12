/**
 * This code is responsible for embedding the payload rootfs tarball into the
 * carrier rootfs so the installer can access it during installation.
 */

#include "all.h"

int embed_payload_rootfs(const char *carrier_rootfs_path, const char *tarball_path)
{
    char dst_dir[COMMAND_PATH_MAX_LENGTH];
    char dst_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Embedding payload rootfs tarball into carrier rootfs...");

    // Create the payload directory within the carrier rootfs.
    snprintf(dst_dir, sizeof(dst_dir), "%s/usr/share/limeos", carrier_rootfs_path);
    if (mkdir_p(dst_dir) != 0)
    {
        LOG_ERROR("Failed to create limeos directory in carrier rootfs");
        return -1;
    }

    // Copy the tarball to the carrier rootfs.
    snprintf(dst_path, sizeof(dst_path), "%s" CONFIG_PAYLOAD_ROOTFS_PATH, carrier_rootfs_path);
    if (copy_file(tarball_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy payload rootfs tarball");
        return -2;
    }

    LOG_INFO("Payload rootfs tarball embedded successfully");

    return 0;
}
