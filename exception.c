#include <unwind.h>
#include <stdio.h>
#include <stdlib.h>

struct _my_Unwind_Exception {
	struct _Unwind_Exception internal;
	uint64_t errorCode;
	void *ipToSet;
	void *lastCleanedFn;
};

/**
 * EXCEPTION FRAME
 * -----------------------------    <- START
 * |   except_table_header_t   |
 * -----------------------------    <- L0
 * |     call_site_header_t    |
 * -----------------------------    <- L1
 * |        call_site_t        |
 * |        call_site_t        |
 * -----------------------------    <- L2
 * |      action_record_t      |
 * |      action_record_t      |
 * -----------------------------    <- L3
 * |      action_typeinfo      |
 * |      action_typeinfo      |
 * -----------------------------    <- L4
 */

typedef struct {
	uint8_t start_encoding;
	uint8_t type_encoding;
	uint8_t len;					// L4 - L0
} except_table_header_t;

typedef struct {
	uint8_t encoding;				// Technically should be leb128
	uint8_t len;					// L2 - L1
} call_site_header_t;

typedef struct {
	uint8_t try_start;				// IP before try
	uint8_t try_length;				// IP after try
	uint8_t landing_pad;			// landing pad location (referenced to function start)
	uint8_t action;					// Either 0 for cleanup, or 1-indexed action in action table
} call_site_t;

typedef struct {
	uint8_t type_index;
	uint8_t next_action_record;
} action_record_t;

typedef uint32_t action_typeinfo;

int8_t leb128_signed_decode_1char(uint8_t v) {
	union {int8_t i; uint8_t u;} v2;
	v2.u = v & 0b011111111;
	char is_negative = v2.u & (1<<6);
	if (is_negative) v2.u |= (1<<7);
	return v2.i;
}

_Unwind_Reason_Code __zno_personality_fn(int version, _Unwind_Action actions, uint64_t exceptionClass, struct _my_Unwind_Exception *exceptionObject, struct _Unwind_Context *context) {
	printf("===================\n");
	if (actions & _UA_HANDLER_FRAME) {
		printf("Personality function, go to landingpad\n");
		printf("Setting IP to %p\n", exceptionObject->ipToSet);
		_Unwind_SetIP(context, exceptionObject->ipToSet);
		_Unwind_SetGR(context, __builtin_eh_return_data_regno(0), exceptionObject->errorCode);
		return _URC_INSTALL_CONTEXT;
	}
	
	except_table_header_t *lsda = _Unwind_GetLanguageSpecificData(context);
	call_site_header_t *call_site_header = lsda+1;

	if (lsda->type_encoding == 255) call_site_header = (uint8_t*)call_site_header - sizeof(uint8_t);

	int call_site_count = call_site_header->len / sizeof(call_site_t);

	printf("%i call sites\n", call_site_count);
	printf("%x\n", call_site_header->len);

	uintptr_t throw_ip = _Unwind_GetIP(context) - 1;
	uintptr_t fn_start = _Unwind_GetRegionStart(context);
	printf("throw ip is %p\n", throw_ip);
	printf("fn start is %p\n", fn_start);

	
	//printf("Personality function, search phase\n");
	printf("Type to handle exception code: %lli\n", exceptionObject->errorCode);

	call_site_t *call_sites = ((uint8_t*)call_site_header) + sizeof(call_site_header_t);
	for (int i = 0; i < call_site_count; i++) {
		uintptr_t call_site_start = fn_start + call_sites->try_start;
		uintptr_t call_site_end = call_site_start + call_sites->try_length;

		//printf("Call site %i: %p %p %x %x\n", i, call_site_start, call_site_end, call_sites->landing_pad, call_sites->action);

		if (throw_ip > call_site_start && throw_ip <= call_site_end) {
			printf("Inside call site %i\n", i);
			if (call_sites->action == 0) {
				printf("Cleanup action\n");
				if (actions & _UA_SEARCH_PHASE) {
					return _URC_CONTINUE_UNWIND;
				} else if (actions & _UA_CLEANUP_PHASE) {
					if (actions & _UA_HANDLER_FRAME) {
						fprintf(stderr, "\033[1m\033[31mZnO runtime - in handler frame but with no landingpad somehow?\033[0m\n");
						//abort();
						return _URC_FATAL_PHASE2_ERROR;
					}

					//void (*cleanup_fn)(void) = fn_start + call_sites->landing_pad;
					printf("Setting up cleanup fn\n");
					//cleanup_fn();
					void *cleanupFn = fn_start;
					printf("Function at %p\n", cleanupFn);
					if (cleanupFn == exceptionObject->lastCleanedFn) {
						printf("Already cleaned this up - continue unwinding\n");
						return _URC_CONTINUE_UNWIND;
					} else {
						exceptionObject->lastCleanedFn = cleanupFn;
						_Unwind_SetIP(context, cleanupFn + call_sites->landing_pad);
						_Unwind_SetGR(context, __builtin_eh_return_data_regno(0), exceptionObject);
						printf("Calling cleanup fn\n");
						return _URC_INSTALL_CONTEXT;
					}
				}
			}
			break;
		}

		call_sites++;
	}

	action_record_t *action_record = ((uint8_t*)call_site_header) + sizeof(call_site_header_t) + call_site_header->len + (call_sites->action-1);

	while (1) {
		uint8_t tyIdx = action_record->type_index;
		uint8_t *typeInfo = ((uint8_t*)call_site_header) + lsda->len;
		uint64_t **gotpcrel_ty = (((uint32_t*)typeInfo) - tyIdx);
		printf("%i\n", tyIdx);

		if (tyIdx == 0) {
			printf("cleanup action\n");
		} else {
			uint64_t **gotpcrel_ty2 = (uint8_t*)gotpcrel_ty + ((uint32_t)*gotpcrel_ty);
			uint64_t action_type = **gotpcrel_ty2;
			//printf("%lli\n", action_type);

			if (action_type == exceptionObject->errorCode) {
				printf("Found handler for exception code %i\n", exceptionObject->errorCode);
				printf("Should jump to %p\n", fn_start + call_sites->landing_pad);
				printf("%i\n", call_sites->landing_pad);
				exceptionObject->ipToSet = fn_start + call_sites->landing_pad;
				return _URC_HANDLER_FOUND;
			}
		}

		
		int8_t next = leb128_signed_decode_1char(action_record->next_action_record);
		printf("Next action is: %i\n", next);

		action_record = (uint8_t*) action_record + 1 + next;
		//printf("%p\n", action_record);

		if (next == 0) break;
	}

	return _URC_CONTINUE_UNWIND;
}

struct _my_Unwind_Exception exceptionBuffer;

void __zno_raise(uint64_t exceptionCode) {
	exceptionBuffer.internal.exception_class = 64;
	exceptionBuffer.errorCode = exceptionCode;
	_Unwind_Reason_Code ret = _Unwind_RaiseException(&exceptionBuffer);
	if (ret == _URC_END_OF_STACK) fprintf(stderr, "\033[1m\033[31mZnO runtime - uncaught exception of code %i\033[0m\n", exceptionBuffer.errorCode);
	else fprintf(stderr, "\033[1m\033[31mZnO runtime - throw failed with URC %i\033[0m\n", ret);
	abort();
}