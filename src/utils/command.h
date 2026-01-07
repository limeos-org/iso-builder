#pragma once

#include <stddef.h>

/**
 * Executes a shell command and returns its exit code.
 *
 * @param command The shell command to execute.
 *
 * @return The exit code of the command.
 */
int run_command(const char *command);

/**
 * Executes a command inside a chroot environment.
 *
 * @param rootfs_path The path to the chroot environment.
 * @param command The command to execute inside the chroot.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates command failure.
 * @return - `-2` - Indicates path quoting failure.
 */
int run_chroot(const char *rootfs_path, const char *command);

/**
 * Creates a directory and all parent directories.
 *
 * @param path The directory path to create.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int mkdir_p(const char *path);

/**
 * Copies a file from source to destination.
 *
 * @param src The source file path.
 * @param dst The destination file path.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int copy_file(const char *src, const char *dst);

/**
 * Recursively removes a file or directory.
 *
 * @param path The path to remove.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int rm_rf(const char *path);

/**
 * Removes a single file.
 *
 * @param path The file path to remove.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int rm_file(const char *path);

/**
 * Changes file permissions.
 *
 * @param mode The permission mode (e.g., "+x", "755").
 * @param path The file path to modify.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int chmod_file(const char *mode, const char *path);

/**
 * Creates a symbolic link.
 *
 * @param target The target of the symlink.
 * @param link_path The path where the symlink will be created.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int symlink_file(const char *target, const char *link_path);

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
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int write_file(const char *path, const char *content);
