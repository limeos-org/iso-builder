#pragma once

/**
 * Runs the target phase.
 *
 * Copies the base rootfs, installs target-specific packages, applies OS
 * branding, and packages the result as a tarball for embedding in the carrier.
 *
 * @param base_rootfs_dir The path to the base rootfs to copy from.
 * @param rootfs_dir The directory for the target rootfs.
 * @param tarball_path The output path for the packaged tarball.
 * @param version The version string for OS branding.
 * @param use_cache Whether to use package caching.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_target_phase(
    const char *base_rootfs_dir, const char *rootfs_dir,
    const char *tarball_path, const char *version, int use_cache
);
