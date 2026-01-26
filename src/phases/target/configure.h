#pragma once

/**
 * Configures the target rootfs with OS identity, splash, GRUB, TTY policy, and XDM.
 *
 * @param path The path to the target rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - OS identity failure.
 * @return - `-2` - Plymouth configuration failure.
 * @return - `-3` - GRUB configuration failure.
 * @return - `-4` - TTY policy enforcement failure.
 * @return - `-5` - XDM configuration failure.
 */
int configure_target_rootfs(const char *path, const char *version);
