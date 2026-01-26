#pragma once

/**
 * Aggressively strips the base rootfs to minimize size.
 *
 * Removes documentation, non-English locales, and clears MOTD files. Does
 * NOT clean apt cache since target and live phases need to install packages
 * after copying from base.
 *
 * @param path The path to the base rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates documentation removal failure.
 * @return - `-2` - Indicates locale removal failure.
 * @return - `-3` - Indicates MOTD clear failure.
 */
int strip_base_rootfs(const char *path);
