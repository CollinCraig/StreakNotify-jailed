#ifndef __ISPY_H__
#define __ISPY_H__
#import <Foundation/Foundation.h>
#include "objc_type.h"	// taken from the class-dump-z source
#include <objc/runtime.h>
#include <objc/message.h>

// Helper macros
#define SL_SETUP_HOOK(func, returnType, args...)	static returnType (*orig_##func)(args);returnType sl_##func(args)
#define SL_HOOK_FUNCTION(func) (MSHookFunction((void *)func, (void *)sl_##func, (void **)&orig_##func))
#define SL_UNHOOK_FUNCTION(func) MSHookFunction((void *)func, (void *)orig_##func, (void **)&orig_##func)

// Do not fuck with this, it will only break shit.
#define USED __attribute__((used))
#define FOUND 0
#define NOT_FOUND 0xff
#define MAX_HOOKS	256	// ok, you might need to mess with this
#define CTLIOCGINFO _IOWR('N', 3, struct ctl_info)
#define EXPORT extern "C" __attribute__((visibility("default")))

// Cribbed from http://www.opensource.apple.com/source/xnu/xnu-792.13.8/bsd/sys/sys_domain.h
/* Kernel Events Protocol */
#define SYSPROTO_EVENT 1	/* kernel events protocol */

/* Kernel Control Protocol */
#define SYSPROTO_CONTROL 2	/* kernel control protocol */
#define AF_SYS_CONTROL 2	/* corresponding sub address type */

/*
 This where the all the preferences end up, from the 'Settings' app
 The dictionary keys, and ui are defined in layout/Library/PreferenceLoader/Preferences/iSpy.plist
 Format for keys is roughly "namespace_attrib"
 */
#define PREFERENCEFILE      "/private/var/mobile/Library/Preferences/com.bishopfox.iSpy.Settings.plist"
#define APP_PREFERENCEFILE  "/private/var/mobile/Library/Preferences/com.bishopfox.iSpy.Targets.plist"

// Handy macro to write to NSLog and ispy_log in a single call. Pass a C format string like ispy_nslog("Foo = %s", foo);
#define ispy_nslog(...) { NSLog(@__VA_ARGS__); ispy_log(__VA_ARGS__); }

/* System family socket address */
struct sockaddr_sys {
	u_char ss_len; /* sizeof(struct sockaddr_sys) */
	u_char ss_family; /* AF_SYSTEM */
	u_int16_t ss_sysaddr; /* protocol address in AF_SYSTEM */
	u_int32_t ss_reserved[7]; /* reserved to the protocol use */
};

struct sockaddr_ctl {
	u_char sc_len; /* depends on size of bundle ID string */
	u_char sc_family; /* AF_SYSTEM */
	u_int16_t ss_sysaddr; /* AF_SYS_KERNCONTROL */
	u_int32_t sc_id; /* Controller unique identifier */
	u_int32_t sc_unit; /* Developer private unit number */
	u_int32_t sc_reserved[5];
};

struct ctl_info {
	u_int32_t ctl_id; /* Kernel Controller ID */
	char ctl_name[96]; /* Kernel Controller Name (a C string) */
};

/* iSpy Logging Stuffs */
static const unsigned int LOG_STRACE   = 0;
static const unsigned int LOG_MSGSEND  = 1;
static const unsigned int LOG_GENERAL  = 2;
static const unsigned int LOG_HTTP     = 3;
static const unsigned int LOG_TCPIP    = 4;
static const unsigned int LOG_GLOBAL   = 5;
static const unsigned int LOG_REPORT   = 6;
static const unsigned int MAX_LOG      = LOG_REPORT;    // this must be equal to the last number in the list of LOG_* numbers, above.

EXPORT void ispy_init_logwriter(NSString *documents);
EXPORT void ispy_log(const char *msg, ...);

/* Other */
EXPORT void bf_hook_msgSend();
EXPORT void bf_hook_msgSend_stret();
EXPORT void bf_enable_msgSend_stret();
EXPORT void bf_disable_msgSend_stret();
EXPORT void bf_enable_msgSend();
EXPORT void bf_disable_msgSend();
EXPORT int bf_get_msgSend_state();
EXPORT void bf_init_substrate_replacement();
EXPORT int return_false();
EXPORT int return_true();
EXPORT id (*orig_objc_msgSend)(id theReceiver, SEL theSelector, ...);
EXPORT bool bf_get_instance_tracking_state();
EXPORT void bf_disable_instance_tracker();
EXPORT void bf_enable_instance_tracker();
EXPORT bool bf_msgSend_should_we_log_this_call(id Cls, SEL selector);
EXPORT bool startWebServices();
EXPORT NSDictionary *getNetworkInfo(void);
EXPORT void update_msgSend_checklists(id *whiteListPtr, id *blackListPtr);
EXPORT void update_msgSend_checklists_stret(id *whiteListPtr, id *blackListPtr);
EXPORT void bf_logwrite_msgSend(int facility, const char *msg, ...);
EXPORT void hook_SecTrustEvaluate(BOOL enableBypass);

// These functions are a hacked-up way of using pure C code to send data down Web Sockets.
// This is useful in the obj_msgSend logging code where we cannot use Objective-C.
// Not implemented in this stripped version of iSpy.
extern "C" void bf_websocket_write(const char *msg);
void bf_init_msgSend_logging();
void bf_enable_msgSend_logging();
void bf_disable_msgSend_logging();
void hijack_on();
BOOL shouldBlockPath(const char *fpath);
BOOL activelyBlock(void);

// for the objc_msgSend logging code.
struct lr_node {
	intptr_t lr;
	id self;
	int should_filter;
	int regs[6];
};

#endif