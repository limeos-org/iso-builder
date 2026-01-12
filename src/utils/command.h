#pragma once
#include "../all.h"

/**
 * The maximum length for file path strings.
 *
 * Set to 4096 to match PATH_MAX on Linux systems, preventing buffer overflows
 * when constructing paths with deeply nested directories.
 */
#define COMMAND_PATH_MAX_LENGTH 4096

/**
 * The maximum length for constructed shell command strings.
 *
 * Calculated as 4 * COMMAND_PATH_MAX_LENGTH to accommodate a command with
 * one fully-quoted path (which can expand up to 4x due to shell escaping)
 * plus command name and arguments.
 */
#define COMMAND_MAX_LENGTH (4 * COMMAND_PATH_MAX_LENGTH)

/**
 * The maximum length for a shell-quoted string.
 *
 * Worst case: every character needs escaping (' -> '\''), so the output
 * can be up to 4x the input length plus 2 for surrounding quotes plus 1
 * for null terminator.
 */
#define COMMAND_QUOTED_MAX_LENGTH (COMMAND_PATH_MAX_LENGTH * 4 + 3)

/**
 * Quotes a string for safe use in shell commands.
 *
 * Wraps the input in single quotes and escapes any internal single quotes
 * using the '\'' technique (end quote, escaped quote, start quote).
 *
 * @param input The string to quote.
 * @param out_quoted The buffer to store the quoted string.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates successful quoting.
 * @return - `-1` - Indicates the output buffer is too small.
 * @return - `-2` - Indicates a NULL input or output pointer.
 */
int shell_quote(const char *input, char *out_quoted, size_t buffer_length);

/**
 * Quotes a file path for safe use in shell commands.
 *
 * Convenience wrapper around shell_quote() specifically for paths.
 * Validates that the path is non-empty before quoting.
 *
 * @param path The file path to quote.
 * @param out_quoted The buffer to store the quoted path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates successful quoting.
 * @return - `-1` - Indicates the output buffer is too small.
 * @return - `-2` - Indicates a NULL or empty path.
 */
int shell_quote_path(const char *path, char *out_quoted, size_t buffer_length);

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

/**
 * Finds the first file matching a glob pattern.
 *
 * @param pattern The glob pattern to match.
 * @param out_path The buffer to store the matched path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates a match was found.
 * @return - `-1` - Indicates no match was found.
 */
int find_first_glob(const char *pattern, char *out_path, size_t buffer_length);

/**
 * Creates a secure temporary directory using mkdtemp().
 *
 * This function creates a uniquely-named directory under /tmp with
 * restricted permissions (0700), protecting against symlink attacks
 * and race conditions that affect predictable directory names.
 *
 * @param out_path The buffer to store the created directory path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int create_secure_tmpdir(char *out_path, size_t buffer_length);

/**
 * Cleans apt cache and lists directories in a rootfs.
 *
 * Removes /var/cache/apt and /var/lib/apt/lists, then recreates the
 * empty directories. This reduces rootfs size while keeping apt functional.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates apt cache removal failure.
 * @return - `-2` - Indicates apt lists removal failure.
 */
int cleanup_apt_directories(const char *rootfs_path);

/**
 * Copies kernel and initrd to standard boot paths.
 *
 * Finds vmlinuz-* and initrd.img-* using glob patterns and copies them
 * to /boot/vmlinuz and /boot/initrd.img respectively.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates kernel copy failure.
 * @return - `-2` - Indicates initrd copy failure.
 */
int copy_kernel_and_initrd(const char *rootfs_path);

/**
 * Removes unnecessary firmware from a rootfs.
 *
 * Removes WiFi, Bluetooth, server NIC, and audio DSP firmware while
 * preserving GPU firmware (for Plymouth) and CPU microcode (for stability).
 *
 * @param rootfs_path The path to the rootfs directory.
 */
void cleanup_unnecessary_firmware(const char *rootfs_path);

/**
 * Blacklists wireless and bluetooth kernel modules.
 *
 * Creates /etc/modprobe.d/blacklist-wireless.conf to prevent modules from
 * loading and looking for firmware that was removed.
 *
 * @param rootfs_path The path to the rootfs directory.
 */
void blacklist_wireless_modules(const char *rootfs_path);

/**
 * Masks the systemd-rfkill service.
 *
 * Since wireless/bluetooth hardware is disabled, rfkill has nothing to
 * manage and would fail on boot.
 *
 * @param rootfs_path The path to the rootfs directory.
 */
void mask_rfkill_service(const char *rootfs_path);
