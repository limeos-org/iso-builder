#pragma once

/**
 * Creates a directory and all parent directories if they don't exist.
 *
 * @param path The directory path to create.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates creation failure.
 */
int mkdir_p(const char *path);

/**
 * Copies a file from source to destination.
 *
 * @param src The source file path.
 * @param dst The destination file path.
 *
 * @return - `0` - Indicates successful copy.
 * @return - `-1` - Indicates copy failure.
 */
int copy_file(const char *src, const char *dst);

/**
 * Checks if a file or directory exists.
 *
 * @param path The path to check.
 *
 * @return - `1` - Indicates the path exists.
 * @return - `0` - Indicates the path does not exist.
 */
int file_exists(const char *path);

/**
 * Writes content to a file, creating it if it doesn't exist.
 *
 * @param path The file path to write to.
 * @param content The content to write.
 *
 * @return - `0` - Indicates successful write.
 * @return - `-1` - Indicates write failure.
 */
int write_file(const char *path, const char *content);
