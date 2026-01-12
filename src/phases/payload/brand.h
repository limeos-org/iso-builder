#pragma once

/**
 * Applies OS branding to the payload rootfs.
 *
 * Replaces Debian branding with LimeOS branding in system identification
 * files. This includes /etc/os-release for programmatic identification,
 * /etc/issue for login prompts, and GRUB distributor config for boot menus.
 *
 * @param path The path to the payload rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Indicates successful branding.
 * @return - `-1` - Indicates /etc/os-release write failure.
 * @return - `-2` - Indicates /etc/issue write failure.
 * @return - `-3` - Indicates GRUB distributor config write failure.
 */
int brand_payload_rootfs(const char *path, const char *version);
