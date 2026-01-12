#pragma once

/**
 * Configures Plymouth boot splash for a rootfs.
 *
 * Creates the LimeOS Plymouth theme, sets it as default, and
 * regenerates the initramfs to include it.
 *
 * @param rootfs_path The path to the rootfs directory.
 * @param logo_path The path to the splash logo PNG file.
 *
 * @return - `0` - Success.
 * @return - `-1` - Logo file not found.
 * @return - `-2` - Failed to create theme directory.
 * @return - `-3` - Failed to copy logo.
 * @return - `-4` - Failed to write theme config.
 * @return - `-5` - Failed to write theme script.
 */
int brand_splash(const char *rootfs_path, const char *logo_path);
