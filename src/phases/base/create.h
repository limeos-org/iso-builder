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
 * @return - `-1` - Indicates path quoting failure.
 * @return - `-2` - Indicates debootstrap failure.
 * @return - `-3` - Indicates apt sources configuration failure.
 * @return - `-4` - Indicates package list update failure.
 * @return - `-5` - Indicates initramfs directory creation failure.
 * @return - `-6` - Indicates initramfs config write failure.
 */
int create_base_rootfs(const char *path);
