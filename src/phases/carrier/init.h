#pragma once

/**
 * Configures init to launch the installer directly.
 *
 * Sets up the carrier rootfs so it boots directly into the installation wizard
 * without a login prompt or shell.
 *
 * @param rootfs_path The path to the carrier rootfs directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates service file write failure.
 * @return - `-2` - Indicates service enable failure.
 * @return - `-3` - Indicates default target set failure.
 * @return - `-4` - Indicates getty disable failure.
 */
int configure_carrier_init(const char *rootfs_path);
