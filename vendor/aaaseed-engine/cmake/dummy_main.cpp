// Phase A/B stub. Will be removed in Phase C when the real source enumeration is wired up.
//
// Beyond the minimal entry point, this file exercises the build infrastructure put in place
// by Phase A and Phase B :
//   - aaa_build_config.h is force-included by the build system (NOT via #include here).
//   - aaa_sdk_version.generated.h is produced by configure_file and on the include path.
//   - the include path resolves project headers like aaa_type.h.
// If any of these break, the static_assert below stops the compile early with a clear error.

#include "aaa_type.h"
#include "license/aaa_sdk_version.generated.h"

static_assert(AAASEED() == 1,                       "AAASEED() must be 1 -- aaa_build_config.h not force-included?");
static_assert(AAA_NEW_DESIGN() == 1,                "AAA_NEW_DESIGN() must be 1 -- aaa_build_config.h not force-included?");
static_assert(AAA_TRACKER() == 1,                   "AAA_TRACKER() must be 1 -- aaa_build_config.h not force-included?");
static_assert(sizeof(AAA_WINDOWS_SDK_VERSION) > 1,  "AAA_WINDOWS_SDK_VERSION empty -- configure_file did not substitute the SDK version?");

int main()
{
    return 0;
}
