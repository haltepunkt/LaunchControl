#import "LaunchControl.h"

@implementation LaunchControl

- (instancetype)init {
    if (self = [super init]) {
        port = MACH_PORT_NULL;
        
        task_get_bootstrap_port(mach_task_self(), &port);
        
        global_data = (struct xpc_global_data *)_os_alloc_once_table[1].ptr;
    }
    
    return self;
}

- (int64_t)manageruid {
    return [self managerUidPid:3];
}

- (int64_t)managerpid {
    return [self managerUidPid:4];
}

- (int64_t)managerUidPid:(int)outgsk {
    dict = xpc_dictionary_create((const char *[]){"handle", "outgsk", "self", "type", "get"},
                                 (xpc_object_t []){xpc_uint64_create(0), xpc_uint64_create(outgsk), xpc_bool_create(TRUE), xpc_uint64_create(DOMAIN_TYPE_LEGACY), xpc_bool_create(TRUE)},
                                 5);
    
    xpc_dictionary_set_mach_send(dict, "domain-port", port);
    
    xpc_object_t reply;
    
    [self pipeRoutine:ROUTINE_MANAGER subsystem:6 reply:&reply error:nil];
    
    if (reply) {
        int64_t uid_pid = xpc_dictionary_get_int64(reply, "out");
        
        if (uid_pid) {
            return uid_pid;
        }
    }
    
    return 0;
}

- (NSString *)managername {
    dict = xpc_dictionary_create((const char *[]){"handle", "outgsk", "self", "type", "get"},
                                 (xpc_object_t []){xpc_uint64_create(0), xpc_uint64_create(6), xpc_bool_create(TRUE), xpc_uint64_create(DOMAIN_TYPE_LEGACY), xpc_bool_create(TRUE)},
                                 5);
    
    xpc_dictionary_set_mach_send(dict, "domain-port", port);
    
    xpc_object_t reply;
    
    [self pipeRoutine:ROUTINE_MANAGER subsystem:6 reply:&reply error:nil];
    
    if (reply) {
        const char *reply_out = xpc_dictionary_get_string(reply, "out");
        
        if (reply_out) {
            return [NSString stringWithUTF8String:reply_out];
        }
    }
    
    return nil;
}

- (BOOL)kickstart:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    if (type == DOMAIN_TYPE_SYSTEM) {
        handle = 0;
    }
    
    else if (type == DOMAIN_TYPE_GUI) {
        dict = xpc_dictionary_create((const char *[]){"handle", "type"},
                                     (xpc_object_t []){xpc_uint64_create(handle), xpc_uint64_create(DOMAIN_TYPE_USER)},
                                     2);
        
        xpc_object_t reply;
        
        if ([self pipeRoutine:0x33b subsystem:3 reply:&reply error:&*error]) {
            if (reply) {
                uint64_t asid = xpc_dictionary_get_uint64(reply, "asid");
                
                handle = asid;
                
                type = DOMAIN_TYPE_LOGIN;
            }
        }
    }
    
    dict = xpc_dictionary_create((const char *[]){"handle", "name", "type"},
                                 (xpc_object_t []){xpc_uint64_create(handle), xpc_string_create([name UTF8String]), xpc_uint64_create(type)},
                                 3);
    
    xpc_object_t reply;
    
    return [self pipeRoutine:ROUTINE_KICKSTART subsystem:2 reply:&reply error:&*error];
}

- (BOOL)bootstrap:(NSString * _Nonnull)path domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    return [self bootstrap:path bootout:FALSE domain:type handle:handle error:&*error];
}

- (BOOL)bootout:(NSString * _Nonnull)path domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    return [self bootstrap:path bootout:TRUE domain:type handle:handle error:&*error];
}

- (BOOL)bootstrap:(NSString * _Nonnull)path bootout:(BOOL)bootout domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    xpc_object_t paths = xpc_array_create((xpc_object_t []){xpc_string_create([path UTF8String])}, 1);
    
    if (type == DOMAIN_TYPE_SYSTEM) {
        handle = 0;
    }
    
    else if (type == DOMAIN_TYPE_GUI) {
        dict = xpc_dictionary_create((const char *[]){"handle", "type"},
                                     (xpc_object_t []){xpc_uint64_create(handle), xpc_uint64_create(DOMAIN_TYPE_USER)},
                                     2);
        
        xpc_object_t reply;
        
        if ([self pipeRoutine:0x33b subsystem:3 reply:&reply error:&*error]) {
            if (reply) {
                uint64_t asid = xpc_dictionary_get_uint64(reply, "asid");
                
                handle = asid;
                
                type = DOMAIN_TYPE_LOGIN;
            }
        }
    }
    
    dict = xpc_dictionary_create((const char *[]){"handle", "paths", "type"},
                                 (xpc_object_t []){xpc_uint64_create(handle), paths, xpc_uint64_create(type)},
                                 3);
    
    xpc_object_t reply;
    
    return [self pipeRoutine:(bootout ? ROUTINE_UNLOAD : ROUTINE_LOAD) subsystem:3 reply:&reply error:&*error];
}

- (BOOL)load:(NSString * _Nonnull)path error:(NSError **)error {
    return [self load:path unload:FALSE error:&*error];
}

- (BOOL)unload:(NSString * _Nonnull)path error:(NSError **)error {
    return [self load:path unload:TRUE error:&*error];
}

- (BOOL)load:(NSString * _Nonnull)path unload:(BOOL)unload error:(NSError **)error {
    xpc_object_t paths = xpc_array_create((xpc_object_t []){xpc_string_create([path UTF8String])}, 1);
    
    dict = xpc_dictionary_create((const char *[]){"paths", "session", (unload ? "disable" : "enable")},
                                 (xpc_object_t []){paths, xpc_string_create("Aqua"), xpc_bool_create(TRUE)},
                                 3);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    return [self pipeRoutine:(unload ? ROUTINE_UNLOAD : ROUTINE_LOAD) subsystem:3 reply:&reply error:&*error];
}

- (BOOL)enable:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    return [self enable:name disable:FALSE domain:type handle:handle error:&*error];
}

- (BOOL)disable:(NSString * _Nonnull)name domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    return [self enable:name disable:TRUE domain:type handle:handle error:&*error];
}

- (BOOL)enable:(NSString * _Nonnull)name disable:(BOOL)disable domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    xpc_object_t names = xpc_array_create((xpc_object_t []){xpc_string_create([name UTF8String])}, 1);
    
    dict = xpc_dictionary_create((const char *[]){"handle", "name", "type", "names"},
                                 (xpc_object_t []){xpc_uint64_create(handle), xpc_string_create([name UTF8String]), xpc_uint64_create(type), names},
                                 4);
    
    xpc_object_t reply;
    
    return [self pipeRoutine:(disable ? ROUTINE_DISABLE : ROUTINE_ENABLE) subsystem:3 reply:&reply error:&*error];
}

- (BOOL)kill:(NSString * _Nonnull)name signal:(int)signal domain:(domain_t)type handle:(uint64_t)handle error:(NSError **)error {
    dict = xpc_dictionary_create((const char *[]){"name", "signal", "handle", "type"},
                                 (xpc_object_t []){xpc_string_create([name UTF8String]), xpc_int64_create(signal), xpc_uint64_create(handle), xpc_uint64_create(type)},
                                 4);
    
    xpc_object_t reply;
    
    return [self pipeRoutine:ROUTINE_KILL subsystem:3 reply:&reply error:&*error];
}

- (BOOL)start:(NSString * _Nonnull)name error:(NSError **)error {
    return [self start:name stop:FALSE error:&*error];
}

- (BOOL)stop:(NSString * _Nonnull)name error:(NSError **)error {
    return [self start:name stop:TRUE error:&*error];
}

- (BOOL)start:(NSString * _Nonnull)name stop:(BOOL)stop error:(NSError **)error {
    dict = xpc_dictionary_create((const char *[]){"name"},
                                 (xpc_object_t []){xpc_string_create([name UTF8String])},
                                 1);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    return [self pipeRoutine:(stop ? ROUTINE_STOP : ROUTINE_START) subsystem:3 reply:&reply error:&*error];
}

- (NSArray <Service *>*)list {
    dict = xpc_dictionary_create(NULL, NULL, 0);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    [self pipeRoutine:ROUTINE_LIST subsystem:3 reply:&reply error:nil];
    
    if (reply) {
        xpc_object_t services = xpc_dictionary_get_value(reply, "services");
        
        if (services) {
            NSMutableArray <Service *>*servicesMutableArray = [NSMutableArray arrayWithCapacity:xpc_dictionary_get_count(services)];
            
            xpc_dictionary_apply(services, ^bool(const char *key, xpc_object_t value) {
                xpc_object_t service = xpc_dictionary_get_value(services, key);
                
                if (service) {
                    Service *s = [Service new];
                    
                    [s setLabel:[NSString stringWithUTF8String:key]];
                    [s setPid:xpc_dictionary_get_int64(service, "pid")];
                    [s setStatus:xpc_dictionary_get_int64(service, "status")];
                    
                    [servicesMutableArray addObject:s];
                }
                
                return TRUE;
            });
            
            return [servicesMutableArray copy];
        }
    }
    
    return nil;
}

- (BOOL)remove:(NSString * _Nonnull)name error:(NSError **)error {
    dict = xpc_dictionary_create((const char *[]){"name"},
                                 (xpc_object_t []){xpc_string_create([name UTF8String])},
                                 1);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    return [self pipeRoutine:ROUTINE_REMOVE subsystem:3 reply:&reply error:&*error];
}

- (BOOL)setEnvironmentVariable:(NSString * _Nonnull)key value:(NSString *)value error:(NSError **)error {
    xpc_object_t envvar = xpc_dictionary_create((const char *[]){[key UTF8String]},
                                                (xpc_object_t []){(value ? xpc_string_create([value UTF8String]) : xpc_null_create())},
                                                1);
    
    dict = xpc_dictionary_create((const char *[]){"envvars"},
                                 (xpc_object_t []){envvar},
                                 1);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    return [self pipeRoutine:ROUTINE_SETUNSETENV subsystem:3 reply:&reply error:&*error];
}

- (BOOL)unsetEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error {
    return [self setEnvironmentVariable:key value:nil error:&*error];
}

- (NSString *)getEnvironmentVariable:(NSString * _Nonnull)key error:(NSError **)error {
    dict = xpc_dictionary_create((const char *[]){"envvar"},
                                 (xpc_object_t []){xpc_string_create([key UTF8String])},
                                 1);
    
    [self setLegacyValues];
    
    xpc_object_t reply;
    
    if ([self pipeRoutine:ROUTINE_GETENV subsystem:3 reply:&reply error:&*error]) {
        const char *value = xpc_dictionary_get_string(reply, "value");
        
        if (value) {
            return [NSString stringWithUTF8String:value];
        }
    }
    
    return nil;
}

- (NSString *)version {
    return [self versionVariant:"version"];
}

- (NSString *)variant {
    return [self versionVariant:"variant"];
}

- (NSString *)versionVariant:(const char *)version_variant {
    dict = xpc_dictionary_create((const char *[]){"type", "handle", version_variant},
                                 (xpc_object_t []){xpc_uint64_create(DOMAIN_TYPE_USER), xpc_uint64_create(0), xpc_bool_create(TRUE)},
                                 3);
    
    int fd[2];
    pipe(fd);
    
    xpc_dictionary_set_fd(dict, "fd", fd[1]);
    
    NSFileHandle *fileHandle = [[NSFileHandle alloc] initWithFileDescriptor:fd[0]];
    
    xpc_object_t reply;
    
    [self pipeRoutine:ROUTINE_VERSION subsystem:3 reply:&reply error:nil];
    
    NSString *output = [[NSString alloc] initWithData:[fileHandle availableData] encoding:NSUTF8StringEncoding];
    
    if (output) {
        return output;
    }
    
    return nil;
}

- (BOOL)pipeRoutine:(int)routine subsystem:(int)subsystem reply:(xpc_object_t *)reply error:(NSError **)error {
    if (error) {
        *error = nil;
    }
    
    xpc_dictionary_set_uint64(dict, "subsystem", subsystem);
    xpc_dictionary_set_uint64(dict, "routine", routine);
#if LAUNCHCONTROL_LOGGING
    NSLog(@"Message: %s", xpc_copy_description(dict));
#endif
    int result = xpc_pipe_routine(global_data->xpc_bootstrap_pipe, dict, &*reply);
    
    if (result == 0) {
#if LAUNCHCONTROL_LOGGING
        NSLog(@"Reply: %s", xpc_copy_description(*reply));
#endif
        int64_t error_code = xpc_dictionary_get_int64(*reply, "error");
        
        if (error_code) {
            if (error) {
                *error = [NSError errorWithDomain:@"com.haltepunkt.LaunchControl" code:error_code userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:xpc_strerror(error_code)]}];
            }
            
            return FALSE;
        }
        
        xpc_object_t errors_dict = xpc_dictionary_get_dictionary(*reply, "errors");
        
        if (errors_dict) {
            if (xpc_dictionary_get_count(errors_dict)) {
                if (error) {
                    __block uint64_t code;
                    
                    xpc_dictionary_apply(errors_dict, ^bool(const char * _Nonnull key, xpc_object_t _Nonnull value) {
                        code = xpc_dictionary_get_int64(errors_dict, key);
                        
                        return FALSE;
                    });
                    
                    if (code) {
                        *error = [NSError errorWithDomain:@"com.haltepunkt.LaunchControl" code:code userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithUTF8String:xpc_strerror(code)]}];
                    }
                }
                
                return FALSE;
            }
        }
        
        return TRUE;
    }
    
    return FALSE;
}

- (void)setLegacyValues {
    xpc_dictionary_set_uint64(dict, "type", DOMAIN_TYPE_LEGACY);
    xpc_dictionary_set_uint64(dict, "handle", 0);
    xpc_dictionary_set_mach_send(dict, "domain-port", port);
    xpc_dictionary_set_bool(dict, "legacy", TRUE);
}

@end
