#pragma once

#include <stddef.h>

/**
 * Maximum length for a shell-quoted string.
 *
 * Worst case: every character needs escaping (' -> '\''), so the output
 * can be up to 4x the input length plus 2 for surrounding quotes plus 1
 * for null terminator.
 */
#define SHELL_QUOTED_MAX_LENGTH (CONFIG_MAX_PATH_LENGTH * 4 + 3)

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
