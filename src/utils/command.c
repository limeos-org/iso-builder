/**
 * This code is responsible for shell quoting, command execution, and
 * filesystem operations.
 */

#include "all.h"

int shell_quote(const char *input, char *out_quoted, size_t buffer_length)
{
    size_t out_pos = 0;
    const char *p;

    // Validate inputs.
    if (!input || !out_quoted)
    {
        return -2;
    }

    // Start with opening single quote.
    if (out_pos >= buffer_length - 1)
    {
        return -1;
    }
    out_quoted[out_pos++] = '\'';

    // Process each character.
    for (p = input; *p; p++)
    {
        if (*p == '\'')
        {
            // Escape single quote: end quote, escaped quote, start quote.
            // Need 4 characters: '\''
            if (out_pos + 4 >= buffer_length)
            {
                return -1;
            }
            out_quoted[out_pos++] = '\'';
            out_quoted[out_pos++] = '\\';
            out_quoted[out_pos++] = '\'';
            out_quoted[out_pos++] = '\'';
        }
        else
        {
            // Copy character as-is (safe inside single quotes).
            if (out_pos >= buffer_length - 2)
            {
                return -1;
            }
            out_quoted[out_pos++] = *p;
        }
    }

    // Close with ending single quote.
    if (out_pos >= buffer_length - 1)
    {
        return -1;
    }
    out_quoted[out_pos++] = '\'';
    out_quoted[out_pos] = '\0';

    return 0;
}

int shell_quote_path(const char *path, char *out_quoted, size_t buffer_length)
{
    // Validate path is non-empty.
    if (!path || path[0] == '\0')
    {
        return -2;
    }

    return shell_quote(path, out_quoted, buffer_length);
}

/** The maximum length for a single line of command output. */
#define COMMAND_OUTPUT_LINE_MAX 4096

/** ANSI escape code for gray (bright black) text. */
#define ANSI_GRAY "\033[90m"

/** ANSI escape code to reset text formatting. */
#define ANSI_RESET "\033[0m"

int run_command(const char *command)
{
    // Redirect stderr to stdout so we capture all output.
    char full_command[COMMAND_MAX_LENGTH];
    snprintf(full_command, sizeof(full_command), "%s 2>&1", command);

    // Open a pipe to capture command output.
    FILE *pipe = popen(full_command, "r");
    if (pipe == NULL)
    {
        return -1;
    }

    // Print each line with gray color and gutter bar.
    char line[COMMAND_OUTPUT_LINE_MAX];
    while (fgets(line, sizeof(line), pipe) != NULL)
    {
        printf(ANSI_GRAY "  | %s" ANSI_RESET, line);
        fflush(stdout);
    }

    // Close the pipe and extract the exit status.
    int status = pclose(pipe);
    if (status == -1)
    {
        return -1;
    }

    // Return the command's exit code.
    return WEXITSTATUS(status);
}

int run_chroot(const char *rootfs_path, const char *command)
{
    // Quote the rootfs path to prevent command injection.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Quote the command to prevent injection.
    char quoted_cmd[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote(command, quoted_cmd, sizeof(quoted_cmd)) != 0)
    {
        return -2;
    }

    // Execute the command inside the chroot environment via sh -c.
    char full_command[COMMAND_MAX_LENGTH];
    snprintf(
        full_command, sizeof(full_command),
        "chroot %s /bin/sh -c %s",
        quoted_path, quoted_cmd
    );
    if (run_command(full_command) != 0)
    {
        return -1;
    }

    return 0;
}

int mkdir_p(const char *path)
{
    // Quote the path to prevent command injection.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path: %s", path);
        return -1;
    }

    // Construct the mkdir command with parent directory creation.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "mkdir -p %s", quoted_path);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: mkdir -p %s", path);
        return -1;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    // Quote the source path for shell safety.
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(src, quoted_src, sizeof(quoted_src)) != 0)
    {
        LOG_ERROR("Failed to quote source path: %s", src);
        return -1;
    }

    // Quote the destination path for shell safety.
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(dst, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        LOG_ERROR("Failed to quote destination path: %s", dst);
        return -1;
    }

    // Construct the copy command.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "cp %s %s", quoted_src, quoted_dst);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: cp %s %s", src, dst);
        return -1;
    }

    return 0;
}

int rm_rf(const char *path)
{
    // Quote the path to prevent command injection.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the recursive remove command.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "rm -rf %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int rm_file(const char *path)
{
    // Quote the path to prevent command injection.
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        return -2;
    }

    // Execute the remove command.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "rm -f %s", quoted_path);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int chmod_file(const char *mode, const char *path)
{
    // Validate inputs.
    if (!mode || !path)
    {
        return -2;
    }

    // Handle "+x" mode: add executable permission.
    if (strcmp(mode, "+x") == 0)
    {
        // Get current file permissions.
        struct stat st;
        if (stat(path, &st) != 0)
        {
            LOG_ERROR("Failed to stat file for chmod: %s", path);
            return -1;
        }

        // Add executable bit for user, group, and others.
        mode_t new_mode = st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;
        if (chmod(path, new_mode) != 0)
        {
            LOG_ERROR("Failed to chmod file: %s", path);
            return -1;
        }

        return 0;
    }

    // Handle numeric modes (e.g., "755", "644").
    if (mode[0] >= '0' && mode[0] <= '7')
    {
        char *endptr;
        long numeric_mode = strtol(mode, &endptr, 8);

        // Validate the mode string was fully parsed and in valid range.
        if (*endptr != '\0' || numeric_mode < 0 || numeric_mode > 07777)
        {
            LOG_ERROR("Invalid chmod mode: %s", mode);
            return -2;
        }

        if (chmod(path, (mode_t)numeric_mode) != 0)
        {
            LOG_ERROR("Failed to chmod file: %s", path);
            return -1;
        }

        return 0;
    }

    // Reject unknown mode formats to prevent injection.
    LOG_ERROR("Unsupported chmod mode format: %s", mode);
    return -2;
}

int symlink_file(const char *target, const char *link_path)
{
    // Quote the target path for shell safety.
    char quoted_target[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote(target, quoted_target, sizeof(quoted_target)) != 0)
    {
        return -2;
    }

    // Quote the link path for shell safety.
    char quoted_link[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(link_path, quoted_link, sizeof(quoted_link)) != 0)
    {
        return -2;
    }

    // Execute the symlink command.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "ln -sf %s %s", quoted_target, quoted_link);
    if (run_command(command) != 0)
    {
        return -1;
    }

    return 0;
}

int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

int write_file(const char *path, const char *content)
{
    // Open the file for writing.
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }

    // Write the content to the file.
    if (fputs(content, file) == EOF)
    {
        LOG_ERROR("Failed to write to file %s", path);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int find_first_glob(const char *pattern, char *out_path, size_t buffer_length)
{
    // Perform glob expansion without sorting results.
    glob_t glob_result;
    int ret = glob(pattern, GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0)
    {
        globfree(&glob_result);
        return -1;
    }

    // Copy the first matched path to the output buffer.
    strncpy(out_path, glob_result.gl_pathv[0], buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    // Free glob resources.
    globfree(&glob_result);
    return 0;
}

int create_secure_tmpdir(char *out_path, size_t buffer_length)
{
    // Template for mkdtemp - X's will be replaced with unique characters.
    char template[] = CONFIG_TMPDIR_PREFIX "XXXXXX";

    // Validate buffer size.
    if (buffer_length < sizeof(template))
    {
        LOG_ERROR("Buffer too small for temporary directory path");
        return -1;
    }

    // Create the secure temporary directory.
    char *result = mkdtemp(template);
    if (result == NULL)
    {
        LOG_ERROR("Failed to create secure temporary directory: %s", strerror(errno));
        return -1;
    }

    // Copy the path to the output buffer.
    strncpy(out_path, result, buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    LOG_INFO("Created secure build directory: %s", out_path);
    return 0;
}

int cleanup_apt_directories(const char *rootfs_path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

    // Remove apt cache.
    snprintf(dir_path, sizeof(dir_path), "%s/var/cache/apt", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt cache");
        return -1;
    }

    // Recreate apt cache directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt cache directory");
    }

    // Remove apt lists.
    snprintf(dir_path, sizeof(dir_path), "%s/var/lib/apt/lists", rootfs_path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt lists");
        return -2;
    }

    // Recreate apt lists directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt lists directory");
    }

    return 0;
}

int copy_kernel_and_initrd(const char *rootfs_path)
{
    char pattern[COMMAND_PATH_MAX_LENGTH];
    char src[COMMAND_PATH_MAX_LENGTH];
    char dst[COMMAND_PATH_MAX_LENGTH];

    // Copy kernel to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/vmlinuz-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        LOG_ERROR("Failed to find kernel");
        return -1;
    }
    snprintf(dst, sizeof(dst), "%s/boot/vmlinuz", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy initrd to standard path.
    snprintf(pattern, sizeof(pattern), "%s/boot/initrd.img-*", rootfs_path);
    if (find_first_glob(pattern, src, sizeof(src)) != 0)
    {
        LOG_ERROR("Failed to find initrd");
        return -2;
    }
    snprintf(dst, sizeof(dst), "%s/boot/initrd.img", rootfs_path);
    if (copy_file(src, dst) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -2;
    }

    return 0;
}

int cleanup_versioned_boot_files(const char *rootfs_path)
{
    // Construct the boot directory path.
    char boot_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(boot_path, sizeof(boot_path), "%s/boot", rootfs_path);

    // Quote the boot path for shell safety.
    char quoted_boot[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(boot_path, quoted_boot, sizeof(quoted_boot)) != 0)
    {
        LOG_ERROR("Failed to quote boot path");
        return -1;
    }

    // Remove versioned kernel, initrd, config, and System.map files.
    // These are created by the kernel package but not needed after
    // copying to generic names (vmlinuz, initrd.img).
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "find %s -maxdepth 1 \\( "
        "-name 'vmlinuz-*' -o "
        "-name 'initrd.img-*' -o "
        "-name 'config-*' -o "
        "-name 'System.map-*' "
        "\\) -type f -delete 2>/dev/null",
        quoted_boot
    );

    // Ignore errors since files may not exist.
    run_command(command);

    return 0;
}
