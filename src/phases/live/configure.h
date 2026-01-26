#pragma once

/**
 * Configures the live rootfs with OS identity and splash.
 *
 * @param path The path to the live rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - OS identity failure.
 * @return - `-2` - Plymouth configuration failure.
 */
int configure_live_rootfs(const char *path, const char *version);
