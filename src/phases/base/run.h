#pragma once

/**
 * Runs the base phase.
 *
 * Creates a minimal, stripped rootfs that serves as the foundation for
 * both the payload (installed system) and carrier (live installer) rootfs.
 * Running debootstrap once and copying saves significant build time.
 *
 * @param rootfs_dir The directory for the base rootfs.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_base_phase(const char *rootfs_dir);
