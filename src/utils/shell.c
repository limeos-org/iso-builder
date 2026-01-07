/**
 * This code is responsible for shell quoting utilities to prevent command injection.
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
