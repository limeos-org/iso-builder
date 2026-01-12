#pragma once

/**
 * Packages the payload rootfs into a compressed tarball.
 *
 * Creates a gzipped tarball of the payload rootfs that will be embedded
 * in the carrier environment for the installer to extract to disk.
 *
 * @param rootfs_path The path to the payload rootfs directory.
 * @param output_path The path where the tarball will be created.
 *
 * @return - `0` - Indicates successful packaging.
 * @return - `-1` - Indicates tarball creation failure.
 */
int package_payload_rootfs(const char *rootfs_path, const char *output_path);
