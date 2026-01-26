#pragma once

/**
 * Runs the live phase.
 *
 * Copies the base rootfs, installs live-specific packages, embeds the
 * target tarball, installs LimeOS components, configures init, and bundles
 * boot-mode-specific packages.
 *
 * @param base_rootfs_dir The path to the base rootfs to copy from.
 * @param rootfs_dir The directory for the live rootfs.
 * @param tarball_path The path to the target tarball to embed.
 * @param components_dir The directory containing downloaded components.
 * @param version The version string for OS branding.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_live_phase(
    const char *base_rootfs_dir,
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir,
    const char *version
);
