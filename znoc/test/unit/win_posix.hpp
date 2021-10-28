#ifndef _ZNO_WINDOWS_POSIX_H
#define _ZNO_WINDOWS_POSIX_H

#include <stdio.h>
#include <atomic>
#include <fmt/format.h>

#if !__has_include(<unistd.h>)
	// If non-posix then add filler for fmemopen
	std::atomic_uint32_t fmem_count = 0;

	FILE* fmemopen(void *buf, size_t size, const char *mode) {
		uint32_t id = fmem_count++;
		FILE *tmpFile = fopen(fmt::format("tmp{}", id).c_str(), "w+");
		if (!tmpFile) return nullptr;
		fwrite(buf, size, 1, tmpFile);
		fclose(tmpFile);
		return fopen(fmt::format("tmp{}", id).c_str(), mode);
	}
#endif

#endif