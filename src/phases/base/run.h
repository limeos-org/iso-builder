#pragma once

/**
 * Runs the base phase.
 *
 * Creates a minimal, stripped rootfs that serves as the foundation for
 * both the payload (installed system) and carrier (live installer) rootfs.
 * Running debootstrap once and copying saves significant build time.
 *
 * If cache_dir is provided and a cached rootfs exists, it will be restored
 * from cache instead of running debootstrap. If no cache exists, the rootfs
 * will be created and saved to cache for future builds.
 *
 * @param rootfs_dir The directory for the base rootfs.
 * @param cache_dir The cache directory (NULL to disable caching).
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_base_phase(const char *rootfs_dir, const char *cache_dir);
