#pragma once

/**
 * Downloads and bundles .deb packages into the carrier rootfs.
 *
 * Downloads packages that cannot be pre-installed (due to conflicts) and
 * stores them in the carrier rootfs for the installer to selectively install.
 *
 * @param carrier_rootfs_path The path to the carrier rootfs directory.
 *
 * @return - `0` - Indicates successful bundling.
 * @return - `-1` - Indicates directory creation failure.
 * @return - `-2` - Indicates package download failure.
 */
int bundle_packages(const char *carrier_rootfs_path);
