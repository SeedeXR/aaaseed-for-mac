// src/net/net_mac.mm
//
// Implementation of the native macOS HTTP transport declared in
// net_mac.h. Pure ObjC++ over NSURLSession + Foundation, manual
// reference counting (-fno-objc-arc).
//
// Verified Foundation symbols (MacOSX SDK, see header for the grep
// trail) :
//   NSURLSession sharedSession                                  (class prop)
//   -[NSURLSession dataTaskWithRequest:completionHandler:]
//   NSMutableURLRequest requestWithURL:
//   -[NSMutableURLRequest setHTTPMethod:]
//   -[NSMutableURLRequest setValue:forHTTPHeaderField:]
//   -[NSMutableURLRequest setHTTPBody:]
//   -[NSHTTPURLResponse statusCode]
//   -[NSData base64EncodedStringWithOptions:]   (options 0 = no breaks)
//   dispatch_semaphore_create / _wait / _signal               (libdispatch)

#import "net_mac.h"

#import <Foundation/Foundation.h>
#import <dispatch/dispatch.h>

#include <map>
#include <memory>
#include <mutex>

namespace aaa
{
namespace net
{

namespace
{
//	Process-global auth state. Guarded by g_auth_mutex. Defaults to
//	AUTH_NONE so requests carry no Authorization header until set_auth()
//	is called.
std::mutex   g_auth_mutex;
AuthMode     g_auth_mode = AUTH_NONE;
std::string  g_auth_credential;

//	In-flight request registry. Keyed by the request id we hand back to
//	the caller. The mapped value is the retained NSURLSessionDataTask so
//	the task object outlives the dataTask...() call frame (without ARC,
//	nothing else keeps it alive long enough for the completion block to
//	fire). The entry is erased -- and the task released -- from inside
//	the completion block. Guarded by g_inflight_mutex.
std::mutex                                       g_inflight_mutex;
std::map<std::uint64_t, NSURLSessionDataTask *>  g_inflight;
std::uint64_t                                    g_next_id = 1;

//	Allocate a fresh, never-zero request id under the in-flight lock.
std::uint64_t next_request_id()
{
    std::lock_guard<std::mutex> lock( g_inflight_mutex );
    std::uint64_t id = g_next_id++;
    if( id == 0 )                       //	wrap guard : 0 is reserved
        id = g_next_id++;
    return id;
}

//	Snapshot the current global auth state under the auth lock.
void current_auth( AuthMode * mode_out, std::string * cred_out )
{
    std::lock_guard<std::mutex> lock( g_auth_mutex );
    *mode_out = g_auth_mode;
    *cred_out = g_auth_credential;
}

//	Decode an NSData body into a UTF-8 std::string. Returns "" for nil
//	or empty data. NSData may not be NUL-terminated, so we construct the
//	std::string from (bytes, length) explicitly.
std::string data_to_string( NSData * data )
{
    if( data == nil || [data length] == 0 )
        return std::string();
    return std::string( static_cast<char const *>( [data bytes] ),
                        static_cast<std::size_t>( [data length] ) );
}

//	Build a retained NSMutableURLRequest for `url` with the given HTTP
//	method, applying the current global Authorization header if any.
//	Returns nil (and sets nothing) if the URL string is malformed.
//	Caller owns the returned request (+1) and must release it.
NSMutableURLRequest * build_request( std::string const & url,
                                     NSString *          method )
{
    NSString * url_str =
        [[NSString alloc] initWithBytes:url.data()
                                 length:url.size()
                               encoding:NSUTF8StringEncoding];
    if( url_str == nil )
        return nil;

    NSURL * ns_url = [NSURL URLWithString:url_str];
    [url_str release];
    if( ns_url == nil )
        return nil;

    //	requestWithURL: returns an autoreleased request ; retain it so
    //	we control its lifetime explicitly under manual RC.
    NSMutableURLRequest * req =
        [[NSMutableURLRequest requestWithURL:ns_url] retain];
    if( req == nil )
        return nil;

    [req setHTTPMethod:method];

    AuthMode    mode;
    std::string cred;
    current_auth( &mode, &cred );
    if( mode != AUTH_NONE )
    {
        std::string header = make_auth_header( mode, cred );
        if( !header.empty() )
        {
            NSString * value =
                [[NSString alloc] initWithBytes:header.data()
                                         length:header.size()
                                       encoding:NSUTF8StringEncoding];
            if( value != nil )
            {
                [req setValue:value forHTTPHeaderField:@"Authorization"];
                [value release];
            }
        }
    }

    return req;
}

//	Shared session accessor. NSURLSession.sharedSession is a process
//	singleton owned by Foundation ; we do not retain/release it.
NSURLSession * shared_session()
{
    return [NSURLSession sharedSession];
}

//	Launch a data task for `req` (which the caller owns +1). On launch
//	this function takes over ownership of `req` (releases it after the
//	task is created). `completion` is copied into the block and invoked
//	exactly once. Returns the request id, or 0 if the task could not be
//	created (in which case `completion` is invoked synchronously with
//	kStatusErrorBadRequest and `req` is released).
std::uint64_t launch( NSMutableURLRequest * req, Completion completion )
{
    if( req == nil )
    {
        if( completion )
            completion( kStatusErrorBadRequest, std::string() );
        return 0;
    }

    std::uint64_t const id = next_request_id();

    //	Capture `completion` and `id` in the block. The block is copied
    //	onto the heap by dataTaskWithRequest:completionHandler:.
    Completion cb = completion;

    NSURLSessionDataTask * task =
        [shared_session() dataTaskWithRequest:req
            completionHandler:^( NSData * data,
                                 NSURLResponse * response,
                                 NSError * error )
        {
            long        status = kStatusErrorNoResponse;
            std::string body;

            if( error != nil )
            {
                status = kStatusErrorTransport;
            }
            else if( [response isKindOfClass:[NSHTTPURLResponse class]] )
            {
                status = static_cast<long>(
                    [(NSHTTPURLResponse *) response statusCode] );
                body = data_to_string( data );
            }

            //	Drop the in-flight entry first (releases the retained
            //	task) so the registry never grows unbounded, then fire
            //	the user callback.
            {
                std::lock_guard<std::mutex> lock( g_inflight_mutex );
                auto it = g_inflight.find( id );
                if( it != g_inflight.end() )
                {
                    [it->second release];
                    g_inflight.erase( it );
                }
            }

            if( cb )
                cb( status, body );
        }];

    //	`req` was retained by build_request ; the data task keeps its
    //	own copy of the request, so we can release ours now.
    [req release];

    if( task == nil )
    {
        if( completion )
            completion( kStatusErrorBadRequest, std::string() );
        return 0;
    }

    //	Retain the task and stash it so it survives until the completion
    //	block erases the entry.
    {
        std::lock_guard<std::mutex> lock( g_inflight_mutex );
        g_inflight[id] = [task retain];
    }

    [task resume];
    return id;
}
}   //	anonymous namespace

void set_auth( AuthMode mode, std::string credential )
{
    std::lock_guard<std::mutex> lock( g_auth_mutex );
    g_auth_mode       = mode;
    g_auth_credential = std::move( credential );
}

std::string make_auth_header( AuthMode mode, std::string const & credential )
{
    switch( mode )
    {
        case AUTH_BASIC:
        {
            //	base64-encode the raw credential bytes. NSData options 0
            //	== no line breaks (RFC 7617 wants a single token).
            @autoreleasepool
            {
                NSData * raw =
                    [NSData dataWithBytes:credential.data()
                                   length:credential.size()];
                NSString * b64 =
                    [raw base64EncodedStringWithOptions:0];
                std::string out = "Basic ";
                if( b64 != nil )
                    out += [b64 UTF8String];
                return out;
            }
        }
        case AUTH_BEARER:
            return std::string( "Bearer " ) + credential;
        case AUTH_NONE:
        default:
            return std::string();
    }
}

std::uint64_t get( std::string const & url, Completion completion )
{
    @autoreleasepool
    {
        NSMutableURLRequest * req = build_request( url, @"GET" );
        return launch( req, completion );
    }
}

std::uint64_t post( std::string const & url,
                    std::string const & body,
                    std::string const & content_type,
                    Completion          completion )
{
    @autoreleasepool
    {
        NSMutableURLRequest * req = build_request( url, @"POST" );
        if( req != nil )
        {
            NSData * body_data =
                [NSData dataWithBytes:body.data() length:body.size()];
            [req setHTTPBody:body_data];

            if( !content_type.empty() )
            {
                NSString * ct =
                    [[NSString alloc] initWithBytes:content_type.data()
                                             length:content_type.size()
                                           encoding:NSUTF8StringEncoding];
                if( ct != nil )
                {
                    [req setValue:ct forHTTPHeaderField:@"Content-Type"];
                    [ct release];
                }
            }
        }
        return launch( req, completion );
    }
}

bool get_sync( std::string const & url,
               double              timeout_sec,
               long *              status_out,
               std::string *       body_out )
{
    @autoreleasepool
    {
        //	Shared, heap-allocated result block. The completion handler
        //	may fire AFTER this function returns on timeout, so it must
        //	not touch our stack. A std::shared_ptr captured by value
        //	into the lambda keeps the result + its own mutex alive for
        //	as long as either side needs it ; whoever drops last frees
        //	it. The semaphore is likewise reference-counted by GCD.
        struct SyncResult
        {
            std::mutex  mtx;
            long        status       = kStatusErrorNoResponse;
            std::string body;
            bool        got_response = false;
        };
        auto state = std::make_shared<SyncResult>();

        dispatch_semaphore_t sem = dispatch_semaphore_create( 0 );

        std::uint64_t id = get( url,
            [state, sem]( long status, std::string body )
        {
            {
                std::lock_guard<std::mutex> lock( state->mtx );
                state->status       = status;
                state->body         = std::move( body );
                state->got_response = ( status >= 0 );
            }
            dispatch_semaphore_signal( sem );
        } );

        if( id == 0 )
        {
            //	get() already invoked the completion synchronously, so
            //	the semaphore was signalled ; drain it to balance.
            dispatch_semaphore_wait( sem, DISPATCH_TIME_FOREVER );
            dispatch_release( sem );
            std::lock_guard<std::mutex> lock( state->mtx );
            if( status_out != nullptr )
                *status_out = state->status;
            if( body_out != nullptr )
                *body_out = state->body;
            return false;
        }

        dispatch_time_t deadline = dispatch_time(
            DISPATCH_TIME_NOW,
            static_cast<int64_t>( timeout_sec * NSEC_PER_SEC ) );

        long const wait_rc = dispatch_semaphore_wait( sem, deadline );

        dispatch_release( sem );

        if( wait_rc != 0 )
        {
            //	Timed out. Cancel + drop the in-flight task so it stops
            //	consuming resources ; the late completion (if any) only
            //	mutates the shared `state`, never our stack, so this is
            //	safe regardless of when it fires.
            std::lock_guard<std::mutex> lock( g_inflight_mutex );
            auto it = g_inflight.find( id );
            if( it != g_inflight.end() )
                [it->second cancel];
            return false;
        }

        std::lock_guard<std::mutex> lock( state->mtx );
        if( status_out != nullptr )
            *status_out = state->status;
        if( body_out != nullptr )
            *body_out = state->body;
        return state->got_response;
    }
}

}   //	namespace net
}   //	namespace aaa
