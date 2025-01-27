#pragma once

#define var_list __builtin_va_list
#define var_start __builtin_va_start
#define var_arg __builtin_va_arg
#define var_end __builtin_va_end

void printf(const char* fmt, ...);