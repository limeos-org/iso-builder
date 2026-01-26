#pragma once

/**
 * Configures GRUB for live ISO boot.
 *
 * Creates the GRUB directory structure and writes grub.cfg with kernel
 * parameters for booting the live system. Called during ISO creation
 * to set up the staging directory before grub-mkrescue.
 *
 * @param staging_path The path to the ISO staging directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates GRUB directory creation failure.
 * @return - `-2` - Indicates GRUB config file write failure.
 */
int setup_grub(const char *staging_path);
