#pragma once

/**
 * Lightly strips the payload rootfs while preserving user-facing content.
 *
 * Unlike the carrier rootfs, the payload system keeps locales and
 * documentation since it's the user's permanent installation. Only package
 * caches are removed to reduce tarball size.
 *
 * @param path The path to the payload rootfs directory.
 *
 * @return - `0` - Indicates successful stripping.
 * @return - `-1` - Indicates apt cache removal failure.
 * @return - `-2` - Indicates apt lists removal failure.
 */
int strip_payload_rootfs(const char *path);
