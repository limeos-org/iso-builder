#pragma once

/**
 * Aggressively strips the base rootfs to minimize size.
 *
 * Removes unnecessary firmware (WiFi, Bluetooth, server NICs), documentation,
 * and non-English locales. Preserves GPU firmware for Plymouth and CPU
 * microcode for system stability. Does NOT clean apt cache since payload
 * and carrier phases need to install packages after copying from base.
 *
 * @param path The path to the base rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates documentation removal failure.
 * @return - `-2` - Indicates locale removal failure.
 */
int strip_base_rootfs(const char *path);
