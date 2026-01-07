#pragma once

/**
 * Strips unnecessary packages, docs, and locales from the rootfs.
 *
 * @param path The path to the rootfs directory.
 *
 * @return - `0` - Indicates successful stripping.
 * @return - `-1` - Indicates documentation removal failure.
 * @return - `-2` - Indicates locale removal failure.
 * @return - `-3` - Indicates apt cache removal failure.
 * @return - `-4` - Indicates apt lists removal failure.
 */
int strip_rootfs(const char *path);
