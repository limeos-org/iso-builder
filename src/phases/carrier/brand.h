#pragma once

/**
 * Applies carrier-specific branding to the rootfs.
 *
 * Calls shared branding functions for OS identity and Plymouth splash.
 *
 * @param path The path to the carrier rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - OS identity branding failure.
 * @return - `-2` - Splash branding failure.
 */
int brand_carrier_rootfs(const char *path, const char *version);
