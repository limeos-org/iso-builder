#pragma once

/**
 * Applies target-specific branding to the rootfs.
 *
 * Calls shared branding functions for OS identity, Plymouth splash,
 * GRUB configuration for silent boot, APT sources configuration,
 * and essential service enablement.
 *
 * @param path The path to the target rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - OS identity branding failure.
 * @return - `-2` - Splash branding failure.
 * @return - `-3` - GRUB config failure.
 * @return - `-4` - APT sources config failure.
 * @return - `-5` - Service enablement failure.
 */
int brand_target_rootfs(const char *path, const char *version);
