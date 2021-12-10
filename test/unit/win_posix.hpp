#ifndef _ZNO_WINDOWS_POSIX_H
#define _ZNO_WINDOWS_POSIX_H

#include <stdio.h>
#include <atomic>
#include <fmt/format.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	// If windows then add filler for fmemopen
	extern std::atomic_uint32_t fmem_count;

	FILE* fmemopen(void *buf, size_t size, const char *mode);
#endif

#endif
