#pragma once

#include "buffer.h"

void aprsis_build_login(const char *call, int passcode, const char *filter, buffer_t *out_buf);