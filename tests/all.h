#pragma once
#include "../src/all.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/*
 * Declarations for semistatic (semistatic = static in production, non-static
 * in testing) functions from the `src` files. These aren't declared in any `.h`
 * files, so we declare them here, so they can be used in the unit tests.
 */

// From src/phases/preparation/download.c
int compute_file_sha256(const char *path, char *out_hash, size_t hash_len);
