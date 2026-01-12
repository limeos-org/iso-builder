#pragma once

/**
 * Embeds the payload rootfs tarball into the carrier rootfs.
 *
 * Copies the payload rootfs tarball to the configured location within the
 * carrier rootfs so the installer can access it during installation.
 *
 * @param carrier_rootfs_path The path to the carrier rootfs directory.
 * @param tarball_path The path to the payload rootfs tarball.
 *
 * @return - `0` - Indicates successful embedding.
 * @return - `-1` - Indicates directory creation failure.
 * @return - `-2` - Indicates copy failure.
 */
int embed_payload_rootfs(const char *carrier_rootfs_path, const char *tarball_path);
