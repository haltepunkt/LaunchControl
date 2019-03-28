#import <Foundation/Foundation.h>
#import "Service.h"

#include <xpc/xpc.h>

#define LAUNCHCONTROL_LOGGING 0

struct _os_alloc_once_s {
    long once;
    void *ptr;
};

extern struct _os_alloc_once_s _os_alloc_once_table[];

void xpc_dictionary_set_mach_send(xpc_object_t, const char*, mach_port_t);
int xpc_pipe_routine(xpc_object_t, xpc_object_t, xpc_object_t*);
char *xpc_strerror(uint64_t);

struct xpc_global_data {
    uint64_t a;
    uint64_t xpc_flags;
    mach_port_t task_bootstrap_port;
    xpc_object_t xpc_bootstrap_pipe;
};

typedef enum {
    DOMAIN_TYPE_SYSTEM = 1,
    DOMAIN_TYPE_USER,
    DOMAIN_TYPE_LOGIN,
    DOMAIN_TYPE_SESSION,
    DOMAIN_TYPE_5,
    DOMAIN_TYPE_6,
    DOMAIN_TYPE_LEGACY,
    DOMAIN_TYPE_GUI
} domain_t;

#define DICT_KEY_ASID       "asid"
#define DICT_KEY_DOMAINPORT "domain-port"
#define DICT_KEY_ERROR      "error"
#define DICT_KEY_ERRORS     "errors"
#define DICT_KEY_FD         "fd"
#define DICT_KEY_GET        "get"
#define DICT_KEY_HANDLE     "handle"
#define DICT_KEY_LEGACY     "legacy"
#define DICT_KEY_NAME       "name"
#define DICT_KEY_NAMES      "names"
#define DICT_KEY_OUT        "out"
#define DICT_KEY_OUTGSK     "outgsk"
#define DICT_KEY_PATHS      "paths"
#define DICT_KEY_ROUTINE    "routine"
#define DICT_KEY_SELF       "self"
#define DICT_KEY_SESSION    "session"
#define DICT_KEY_SIGNAL     "signal"
#define DICT_KEY_SUBSYSTEM  "subsystem"
#define DICT_KEY_TYPE       "type"
#define DICT_KEY_VALUE      "value"

#define ROUTINE_MANAGER     0x12d
#define ROUTINE_KICKSTART   0x2be
#define ROUTINE_LOAD        0x320
#define ROUTINE_UNLOAD      0x321
#define ROUTINE_ENABLE      0x328
#define ROUTINE_DISABLE     0x329
#define ROUTINE_KILL        0x32c
#define ROUTINE_START       0x32d
#define ROUTINE_STOP        0x32e
#define ROUTINE_LIST        0x32f
#define ROUTINE_REMOVE      0x330
#define ROUTINE_SETUNSETENV 0x333
#define ROUTINE_GETENV      0x334
#define ROUTINE_PROCINFO    0x336
#define ROUTINE_VERSION     0x33c

@interface LaunchControl : NSObject {
    
    mach_port_t port;
    
    struct xpc_global_data *global_data;
    
    xpc_object_t dict;
    
}

- (int64_t)managerUID;
- (int64_t)managerPID;
- (NSString *)managerName;
- (BOOL)kickstart:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)bootstrap:(NSString * _Nonnull)path domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)bootout:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)load:(NSString * _Nonnull)path error:(NSError **)error;
- (BOOL)unload:(NSString * _Nonnull)path error:(NSError **)error;
- (BOOL)enable:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)disable:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)kill:(NSString * _Nonnull)name signal:(int)signal domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error;
- (BOOL)start:(NSString * _Nonnull)name error:(NSError **)error;
- (BOOL)stop:(NSString * _Nonnull)name error:(NSError **)error;
- (NSArray <Service *>*)list;
- (BOOL)remove:(NSString * _Nonnull)name error:(NSError **)error;
- (BOOL)setEnvironmentVariable:(NSString * _Nonnull)key value:(NSString *)value error:(NSError **)error;
- (BOOL)unsetEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error;
- (NSString *)getEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error;
- (NSString *)version;
- (NSString *)variant;

@end
