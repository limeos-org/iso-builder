#pragma once

/**
 * Runs the payload phase.
 *
 * Copies the base rootfs, installs payload-specific packages, applies OS
 * branding, and packages the result as a tarball for embedding in the carrier.
 *
 * @param base_rootfs_dir The path to the base rootfs to copy from.
 * @param rootfs_dir The directory for the payload rootfs.
 * @param tarball_path The output path for the packaged tarball.
 * @param version The version string for OS branding.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_payload_phase(const char *base_rootfs_dir, const char *rootfs_dir,
                      const char *tarball_path, const char *version);
