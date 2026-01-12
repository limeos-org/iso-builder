#pragma once

/**
 * Applies payload-specific branding to the rootfs.
 *
 * Calls shared branding functions for OS identity, Plymouth splash,
 * and GRUB configuration for silent boot.
 *
 * @param path The path to the payload rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - OS identity branding failure.
 * @return - `-2` - Splash branding failure.
 * @return - `-3` - GRUB config failure.
 */
int brand_payload_rootfs(const char *path, const char *version);
