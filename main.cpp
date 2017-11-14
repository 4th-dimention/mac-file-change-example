/*
Experiments with FSEvents
*/

#define MAIN_WATCH "/Users/allenwebster/fsevents"
#define ALTERNATING_WATCH "/Users/allenwebster/4ed/code"

#include <CoreServices/CoreServices.h>
#include <stdio.h>

//////////////////

void
file_watch_callback(ConstFSEventStreamRef stream, void *callbackInfo, size_t numEvents, void *evPaths, const FSEventStreamEventFlags *evFlags, const FSEventStreamEventId *evIds){
    char const **paths = (char const**)evPaths;
    for (int i = 0; i < numEvents; ++i){
        fprintf(stdout, "%llu\t%u\t%s\n", (unsigned long long)evIds[i], evFlags[i], paths[i]);
    }
}

//////////////////

typedef struct{
    FSEventStreamRef stream;
} File_Watching_Handle;

File_Watching_Handle
schedule_file_watching(char *f){
    File_Watching_Handle handle = {0};
    
    CFStringRef arg = CFStringCreateWithCString(0, f, kCFStringEncodingUTF8);
    
    CFArrayRef paths = CFArrayCreate(0, (const void**)&arg, 1, 0);
    
    void *callbackInfo = 0;
    CFAbsoluteTime latency = 2.0;
    
    handle.stream = FSEventStreamCreate(0, &file_watch_callback,  0, paths, kFSEventStreamEventIdSinceNow, latency, kFSEventStreamCreateFlagFileEvents);
    
    FSEventStreamScheduleWithRunLoop(handle.stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    
    FSEventStreamStart(handle.stream);
    return(handle);
}

void
unschedule_file_watching(File_Watching_Handle handle){
    FSEventStreamStop(handle.stream);
    FSEventStreamInvalidate(handle.stream);
    FSEventStreamRelease(handle.stream);
}

File_Watching_Handle other_handle = {0};

void
watch_other(void){
    if (other_handle.stream == 0){
        other_handle = schedule_file_watching(ALTERNATING_WATCH);
    }
}

void
unwatch_other(void){
    if (other_handle.stream != 0){
        unschedule_file_watching(other_handle);
        other_handle.stream = 0;
    }
}

void
switch_other_watcher(CFRunLoopTimerRef timer, void *info){
    static int is_on = false;
    static int counter = 0;
    ++counter;
    if (counter == 6){
        fprintf(stdout, "finished\n");
        exit(0);
    }
    if (is_on){
        fprintf(stdout, "no longer watching other\n");
        unwatch_other();
        is_on = false;
    }
    else{
        fprintf(stdout, "watching other\n");
        watch_other();
        is_on = true;
    }
}

int main(){
    fprintf(stdout, "eventID\t\tFlag\tPath\n");
    
    CFRunLoopTimerRef timer = CFRunLoopTimerCreate(0, 0.0, 10.0, 0, 0, switch_other_watcher, 0);
    CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
    schedule_file_watching(MAIN_WATCH);
    
    CFRunLoopRun();
    
    return(0);
}

// BOTTOM

