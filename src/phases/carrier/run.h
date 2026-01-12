#pragma once

/**
 * Runs the carrier phase.
 *
 * Copies the base rootfs, installs carrier-specific packages, embeds the
 * payload tarball, installs LimeOS components, configures init, and bundles
 * boot-mode-specific packages.
 *
 * If cache_dir is provided, the shared apt cache will be mounted to speed up
 * package downloads on subsequent builds.
 *
 * @param base_rootfs_dir The path to the base rootfs to copy from.
 * @param rootfs_dir The directory for the carrier rootfs.
 * @param tarball_path The path to the payload tarball to embed.
 * @param components_dir The directory containing downloaded components.
 * @param cache_dir The cache directory (NULL to disable caching).
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_carrier_phase(
    const char *base_rootfs_dir,
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir,
    const char *cache_dir
);
