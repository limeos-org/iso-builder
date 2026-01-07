#pragma once

/**
 * Configures Plymouth with the LimeOS boot splash.
 *
 * Installs the splash image and configures Plymouth to display it during boot.
 *
 * @param rootfs_path The path to the root filesystem directory.
 * @param logo_path The path to the splash logo image.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates splash logo not found.
 * @return - `-2` - Indicates theme directory creation failure.
 * @return - `-3` - Indicates splash logo copy failure.
 * @return - `-4` - Indicates theme file write failure.
 * @return - `-5` - Indicates script file write failure.
 */
int setup_splash(const char *rootfs_path, const char *logo_path);
