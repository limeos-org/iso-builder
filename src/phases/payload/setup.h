#pragma once
#include "../../all.h"

/**
 * Creates the default user account.
 *
 * Creates user "user" with password "password" and sudo access.
 *
 * @param rootfs_path The path to the payload rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int create_default_user(const char *rootfs_path);
