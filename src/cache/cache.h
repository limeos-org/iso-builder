#pragma once

/**
 * Gets the base cache directory path.
 *
 * Uses XDG_CACHE_HOME if set, otherwise falls back to ~/.cache/limeos-iso-builder.
 *
 * @param out_path Buffer to store the cache directory path.
 * @param buffer_length Size of the output buffer.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure (HOME not set).
 */
int get_cache_dir(char *out_path, size_t buffer_length);
