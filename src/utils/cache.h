#pragma once

/**
 * Initializes the cache directory structure.
 *
 * Creates the cache directory and subdirectories if they don't exist.
 *
 * @param cache_dir The path to the cache directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int init_cache(const char *cache_dir);

/**
 * Checks if a cached base rootfs exists.
 *
 * @param cache_dir The path to the cache directory.
 *
 * @return - `1` - Indicates cached rootfs exists.
 * @return - `0` - Indicates no cached rootfs.
 */
int has_cached_base_rootfs(const char *cache_dir);

/**
 * Saves the base rootfs to the cache as a tarball.
 *
 * @param rootfs_path The path to the rootfs directory to cache.
 * @param cache_dir The path to the cache directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int save_base_rootfs_to_cache(const char *rootfs_path, const char *cache_dir);

/**
 * Restores the base rootfs from the cache.
 *
 * @param cache_dir The path to the cache directory.
 * @param rootfs_path The path where the rootfs should be extracted.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int restore_base_rootfs_from_cache(const char *cache_dir, const char *rootfs_path);

/**
 * Gets the path to the shared apt cache directory.
 *
 * @param cache_dir The path to the cache directory.
 * @param out_path The buffer to store the apt cache path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int get_apt_cache_path(const char *cache_dir, char *out_path, size_t buffer_length);

/**
 * Mounts the shared apt cache into a rootfs.
 *
 * Uses bind mount to share the apt cache between builds.
 *
 * @param cache_dir The path to the cache directory.
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int mount_apt_cache(const char *cache_dir, const char *rootfs_path);

/**
 * Unmounts the shared apt cache from a rootfs.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int unmount_apt_cache(const char *rootfs_path);

/**
 * Checks if a cached payload rootfs exists.
 *
 * @param cache_dir The path to the cache directory.
 *
 * @return - `1` - Indicates cached rootfs exists.
 * @return - `0` - Indicates no cached rootfs.
 */
int has_cached_payload_rootfs(const char *cache_dir);

/**
 * Saves the payload rootfs to the cache as a tarball.
 *
 * @param rootfs_path The path to the rootfs directory to cache.
 * @param cache_dir The path to the cache directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int save_payload_rootfs_to_cache(const char *rootfs_path, const char *cache_dir);

/**
 * Restores the payload rootfs from the cache.
 *
 * @param cache_dir The path to the cache directory.
 * @param rootfs_path The path where the rootfs should be extracted.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int restore_payload_rootfs_from_cache(const char *cache_dir, const char *rootfs_path);


/**
 * Checks if a cached carrier rootfs exists.
 *
 * @param cache_dir The path to the cache directory.
 *
 * @return - `1` - Indicates cached rootfs exists.
 * @return - `0` - Indicates no cached rootfs.
 */
int has_cached_carrier_rootfs(const char *cache_dir);

/**
 * Saves the carrier rootfs to the cache as a tarball.
 *
 * @param rootfs_path The path to the rootfs directory to cache.
 * @param cache_dir The path to the cache directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int save_carrier_rootfs_to_cache(const char *rootfs_path, const char *cache_dir);

/**
 * Restores the carrier rootfs from the cache.
 *
 * @param cache_dir The path to the cache directory.
 * @param rootfs_path The path where the rootfs should be extracted.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int restore_carrier_rootfs_from_cache(const char *cache_dir, const char *rootfs_path);
