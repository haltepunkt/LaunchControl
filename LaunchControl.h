#import <Foundation/Foundation.h>
#import "Service.h"

#include <xpc/xpc.h>

#ifndef LAUNCHCONTROL_LOGGING
#define LAUNCHCONTROL_LOGGING 0
#endif

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
    DOMAIN_SYSTEM = 1,
    DOMAIN_USER,
    DOMAIN_LOGIN,
    DOMAIN_SESSION,
    DOMAIN_PID,
    DOMAIN_6,
    DOMAIN_LEGACY,
    DOMAIN_GUI
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

#define ROUTINE_MANAGER         0x12d
#define ROUTINE_KICKSTART       0x2be
#define ROUTINE_ATTACH          0x2bf
#define ROUTINE_CONFIGURE       0x2c1
#define ROUTINE_SIGNAL          0x2c3
#define ROUTINE_0x2c4           0x2c4
#define ROUTINE_RUNSTATS        0x2c5
#define ROUTINE_LOAD            0x320
#define ROUTINE_UNLOAD          0x321
#define ROUTINE_ENABLE          0x328
#define ROUTINE_DISABLE         0x329
#define ROUTINE_KILL            0x32c
#define ROUTINE_START           0x32d
#define ROUTINE_STOP            0x32e
#define ROUTINE_LIST            0x32f
#define ROUTINE_REMOVE          0x330
#define ROUTINE_SETUNSETENV     0x333
#define ROUTINE_GETENV          0x334
#define ROUTINE_PROCINFO        0x336
#define ROUTINE_0x339           0x339
#define ROUTINE_0x33a           0x33a
#define ROUTINE_0x33b           0x33b
#define ROUTINE_VERSION         0x33c
#define ROUTINE_DUMPSTATE       0x342
#define ROUTINE_0x343           0x343
#define ROUTINE_BOOTSHELL       0x344
#define ROUTINE_DUMPJPCATEGORY  0x345

@interface LaunchControl : NSObject {
    
    mach_port_t port;
    
    struct xpc_global_data *global_data;
    
    xpc_object_t dict;
    
}

/**
 Returns the UID of the current launchd session.

 @return An int64_t integer.
 */
- (int64_t)managerUID;

/**
 Returns the PID of the launchd controlling the session.

 @return An int64_t integer.
 */
- (int64_t)managerPID;

/**
 Returns the name of the current launchd session.
 
 @return An NSString object.
 */
- (NSString *)managerName;

/**
 Forces an existing service to kickstart for a given domain and handle.

 @param name The name of the service to kickstart.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully kickstarted, otherwise FALSE.
 */
- (BOOL)kickstart:(NSString * _Nonnull)name domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Bootstraps a service into a domain.
 
 @param path The path of the service to bootstrap.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully bootstrapped, otherwise FALSE.
 */
- (BOOL)bootstrap:(NSString * _Nonnull)path domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Removes a service from a domain.

 @param name The name of the service to remove.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully removed, otherwise FALSE.
 */
- (BOOL)bootout:(NSString * _Nonnull)name domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Bootstraps a service.
 
 @note Legacy command, instead use bootstrap:domain:handle:error:

 @param path The path of the service to bootstrap.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully bootstrapped, otherwise FALSE.
 */
- (BOOL)load:(NSString * _Nonnull)path error:(NSError **)error;

/**
 Unloads a service.
 
 @note Legacy command, instead use bootout:domain:handle:error

 @param path The path of the service to unload.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully unloaded, otherwise FALSE.
 */
- (BOOL)unload:(NSString * _Nonnull)path error:(NSError **)error;

/**
 Enables a service.

 @param name The name of the service to enable.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully enabled, otherwise FALSE.
 */
- (BOOL)enable:(NSString * _Nonnull)name domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Disables a service.

 @param name The name of the service to disable.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully disabled, otherwise FALSE.
 */
- (BOOL)disable:(NSString * _Nonnull)name domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Sends a signal to a service.

 @param name The name of the service to send the signal to.
 @param signal The signal to send to the service.
 @param domain The domain to target, such as DOMAIN_SYSTEM, DOMAIN_USER, DOMAIN_LOGIN, DOMAIN_GUI, DOMAIN_SESSION, or DOMAIN_PID.
 @param handle The handle to target, such as a UID, an ASID, or a PID.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the signal was successfully sent, otherwise FALSE.
 */
- (BOOL)kill:(NSString * _Nonnull)name signal:(int)signal domain:(domain_t)domain handle:(uint64_t)handle error:(NSError **)error;

/**
 Starts a service.

 @param name The name of the service to start.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully started, otherwise FALSE.
 */
- (BOOL)start:(NSString * _Nonnull)name error:(NSError **)error;

/**
 Stops a service.

 @param name The name of the service to stop.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully stopped, otherwise FALSE.
 */
- (BOOL)stop:(NSString * _Nonnull)name error:(NSError **)error;

/**
 Lists information about services.

 @return An NSArray object containing Service objects.
 */
- (NSArray <Service *>*)list;

/**
 Unloads a service.

 @param name The name of the service to unload.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the service was successfully unloaded, otherwise FALSE.
 */
- (BOOL)remove:(NSString * _Nonnull)name error:(NSError **)error;

/**
 Sets an environment variable.

 @param key The key of the environment variable to set.
 @param value The new value of the environment variable to set.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the environment variable was successfully set, otherwise FALSE.
 */
- (BOOL)setEnvironmentVariable:(NSString * _Nonnull)key value:(NSString *)value error:(NSError **)error;

/**
 Unsets an environment variable.

 @param key The key of the environment variable to unset.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return TRUE if the environment variable was successfully unset, otherwise FALSE.
 */
- (BOOL)unsetEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error;

/**
 Returns the value of an environment variable.

 @param key The key of the environment variable to return its value.
 @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
 @return An NSString object.
 */
- (NSString *)getEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error;

/**
 Returns the launchd version.

 @return An NSString object.
 */
- (NSString *)version;

/**
 Returns the launchd variant.

 @return An NSString object.
 */
- (NSString *)variant;

@end
