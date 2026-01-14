#pragma once

/**
 * Checks if a valid cache exists for the current configuration.
 *
 * @param out_cache_path Buffer to store the cache file path if found.
 * @param buffer_length Size of the output buffer.
 *
 * @return - `1` - Indicates cache exists and is valid.
 * @return - `0` - Indicates no valid cache found.
 */
int cache_exists(char *out_cache_path, size_t buffer_length);

/**
 * Restores the base rootfs from cache.
 *
 * @param cache_path Path to the cached tarball.
 * @param rootfs_dir Directory to extract the rootfs into.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int restore_from_cache(const char *cache_path, const char *rootfs_dir);

/**
 * Saves the base rootfs to cache.
 *
 * @param rootfs_dir Directory containing the rootfs to cache.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int save_to_cache(const char *rootfs_dir);
