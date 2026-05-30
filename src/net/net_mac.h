// src/net/net_mac.h
//
// Native macOS HTTP + JSON sub-library for the AAASeed Mac port.
// This header declares the HTTP transport surface ; the JSON->Lua
// binding lives in the companion `net_json_lua.h` / `net_json_lua.mm`.
//
// Scope :
//   - A thin, hermetic wrapper over NSURLSession for async GET / POST,
//     plus a synchronous (test-only) GET helper that blocks on a
//     dispatch_semaphore.
//   - A process-global Authorization state (none / basic / bearer) set
//     via set_auth(), applied to every outbound request.
//   - A pure, side-effect-free make_auth_header() helper so the
//     credential-encoding logic (base64 for Basic, raw for Bearer) can
//     be unit-tested deterministically without touching the network.
//
// Foundation surface actually used (verified against the Foundation
// headers -- see the implementation file for the exact symbols) :
//   - NSURLSession sharedSession
//   - dataTaskWithRequest:completionHandler:
//   - NSMutableURLRequest, setHTTPMethod:, setValue:forHTTPHeaderField:,
//     setHTTPBody:
//   - NSHTTPURLResponse statusCode
//   - NSData base64EncodedStringWithOptions: (for the Basic scheme)
//   - NSJSONSerialization (used only in net_json_lua.mm)
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + Foundation only ; the JSON binding TU
//     additionally links aaaseed_lua. No engine link, no o_str, no
//     aaa_mem cascade, no vendor includes.
//   - Manual reference counting (compiled with -fno-objc-arc).
//   - ASCII / English-only comments.
//
// Threading :
//   - set_auth() and the in-flight request map are guarded by an
//     internal std::mutex ; get() / post() may be called from any
//     thread. The completion handler runs on an NSURLSession delegate
//     queue (a private GCD queue), NOT the calling thread.

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace aaa
{
namespace net
{

//	Authorization scheme applied to outbound requests. Kept as a plain
//	enum (not enum class) so the Lua binding layer -- should it ever
//	expose auth -- can pass it as a plain integer without a cast.
enum AuthMode
{
    AUTH_NONE   = 0,    //	no Authorization header
    AUTH_BASIC  = 1,    //	"Authorization: Basic <base64(credential)>"
    AUTH_BEARER = 2,    //	"Authorization: Bearer <credential>"
};

//	Completion callback for the async get() / post() entry points.
//	`status_code` is the HTTP status (NSHTTPURLResponse.statusCode), or
//	a negative value on a transport-level failure (see kStatusError*).
//	`body` is the raw response body decoded as UTF-8 (empty on error).
typedef std::function<void( long status_code, std::string body )>
    Completion;

//	Negative status sentinels reported through Completion::status_code
//	when the request never produced an HTTP response. These never
//	collide with real HTTP statuses (which are >= 100).
enum : long
{
    kStatusErrorTransport   = -1,   //	NSError from the data task
    kStatusErrorNoResponse  = -2,   //	no NSHTTPURLResponse + no error
    kStatusErrorBadRequest  = -3,   //	malformed URL / could not build request
};

//	Set the process-global Authorization state. The credential is the
//	raw value the scheme expects :
//	  - AUTH_BASIC  : "user:password" (will be base64-encoded per RFC 7617)
//	  - AUTH_BEARER : the bearer token (sent verbatim)
//	  - AUTH_NONE   : credential ignored ; no header is emitted
//	Thread-safe : guarded by an internal mutex. Applies to all
//	subsequent get() / post() calls.
void set_auth( AuthMode mode, std::string credential );

//	Pure helper : compute the Authorization header VALUE (the part after
//	"Authorization: ") for a given mode + credential. No global state,
//	no I/O -- safe to unit-test in isolation.
//	  - AUTH_BASIC  -> "Basic <base64(credential)>"
//	  - AUTH_BEARER -> "Bearer <credential>"
//	  - AUTH_NONE   -> "" (empty ; caller must not set the header)
//	An empty credential with AUTH_BEARER yields "Bearer " ; with
//	AUTH_BASIC yields "Basic " + base64("") == "Basic ". Callers that
//	treat "" as "omit the header" should check mode == AUTH_NONE.
std::string make_auth_header( AuthMode mode, std::string const & credential );

//	Async HTTP GET. Returns a monotonically increasing request id ; the
//	same id keys the in-flight request map until `completion` fires.
//	`completion` is invoked exactly once, on a private session queue.
//	A return value of 0 means the request could not be started (bad
//	URL) and `completion` was invoked synchronously with
//	kStatusErrorBadRequest before returning.
std::uint64_t get( std::string const & url, Completion completion );

//	Async HTTP POST with an explicit request body and Content-Type.
//	`content_type` is written into the Content-Type header verbatim
//	(e.g. "application/json"). Semantics for the return id and the
//	`completion` callback match get().
std::uint64_t post( std::string const & url,
                     std::string const & body,
                     std::string const & content_type,
                     Completion          completion );

//	Test-only synchronous GET. Blocks the calling thread (via a
//	dispatch_semaphore) until the request completes or `timeout_sec`
//	elapses. Writes the HTTP status into *status_out and the body into
//	*body_out. Returns true if a response arrived within the timeout,
//	false on timeout or transport error.
//
//	This performs real network I/O and is intended ONLY for tests that
//	explicitly opt in (CI has no guaranteed network) ; production code
//	should use the async get(). Either out-pointer may be null.
bool get_sync( std::string const & url,
               double              timeout_sec,
               long *              status_out,
               std::string *       body_out );

}   //	namespace net
}   //	namespace aaa
