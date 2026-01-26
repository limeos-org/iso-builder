#pragma once

/**
 * Creates a minimal base rootfs using debootstrap.
 *
 * This creates the foundation that both target and live rootfs will
 * be copied from. Runs debootstrap, configures apt sources, updates
 * package lists, and pre-configures initramfs for hardware support.
 *
 * @param path The path to create the base rootfs.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates debootstrap failure.
 * @return - `-2` - Indicates apt sources or update failure.
 * @return - `-3` - Indicates initramfs configuration failure.
 */
int create_base_rootfs(const char *path);
