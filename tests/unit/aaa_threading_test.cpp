// Tests the threading primitives in Src/aaa/ that work on Mac :
//   c_mutex (std::mutex)
//   c_mutex_recursive (std::recursive_mutex)
//   c_spinlock (std::atomic_flag)
//   aaa::lock_guard<TMutex>
//   aaa::unique_lock<TMutex>
//
// c_mutex_ct, c_benaphore, c_benaphore_recursive are Win32-only and gated
// out of the Mac build (see vendor/aaaseed-engine/Src/aaa/aaa_mutex.h).
//
// Multi-threaded tests use std::thread + std::atomic for invariant checks
// rather than relying on the engine threading layer (which we want to test,
// not depend on).

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include "aaa_type.h"
#include "aaa/aaa_mutex.h"
#include "aaa/aaa_lock_guard.h"

TEST( AaaMutex, BasicLockUnlock )
{
    aaa::c_mutex m;
    m.lock();
    m.unlock();
    EXPECT_TRUE( m.try_lock() );
    m.unlock();
}

TEST( AaaMutex, TryLockFailsWhenLockedByOtherThread )
{
    aaa::c_mutex m;
    m.lock();

    std::atomic< bool > other_acquired{ false };
    std::thread t( [ & ]() { other_acquired = m.try_lock(); } );
    t.join();

    EXPECT_FALSE( other_acquired.load() );
    m.unlock();
}

TEST( AaaMutexRecursive, SameThreadCanRelock )
{
    aaa::c_mutex_recursive m;
    m.lock();
    m.lock();   // std::recursive_mutex permits this from the same thread
    m.unlock();
    m.unlock();
    EXPECT_TRUE( m.try_lock() );
    m.unlock();
}

TEST( AaaSpinlock, BasicLockUnlock )
{
    aaa::c_spinlock s;
    s.lock();
    s.unlock();
    EXPECT_TRUE( s.try_lock() );
    s.unlock();
}

TEST( AaaSpinlock, TryLockFailsWhenHeld )
{
    aaa::c_spinlock s;
    s.lock();
    EXPECT_FALSE( s.try_lock() );
    s.unlock();
    EXPECT_TRUE( s.try_lock() );
    s.unlock();
}

TEST( AaaLockGuard, AcquiresAndReleases )
{
    aaa::c_mutex m;
    {
        aaa::lock_guard< aaa::c_mutex > g( m );
        EXPECT_FALSE( m.try_lock() );   // already locked by the guard
    }
    EXPECT_TRUE( m.try_lock() );        // released on scope exit
    m.unlock();
}

TEST( AaaLockGuard, AdoptLockSkipsInitialLock )
{
    aaa::c_mutex m;
    m.lock();
    {
        aaa::lock_guard< aaa::c_mutex > g( m, aaa::adopt_lock );
        EXPECT_FALSE( m.try_lock() );   // still locked
    }
    EXPECT_TRUE( m.try_lock() );        // guard's destructor released it
    m.unlock();
}

TEST( AaaMutex, MutualExclusionUnderContention )
{
    constexpr int kThreads = 8;
    constexpr int kIterationsPerThread = 10000;

    aaa::c_mutex m;
    INT64 counter = 0;

    std::vector< std::thread > workers;
    for( int i = 0; i < kThreads; ++i )
    {
        workers.emplace_back( [ & ]() {
            for( int j = 0; j < kIterationsPerThread; ++j )
            {
                aaa::lock_guard< aaa::c_mutex > g( m );
                ++counter;
            }
        } );
    }
    for( auto& w : workers )
        w.join();

    EXPECT_EQ( counter, static_cast< INT64 >( kThreads ) * kIterationsPerThread );
}

TEST( AaaSpinlock, MutualExclusionUnderContention )
{
    constexpr int kThreads = 4;
    constexpr int kIterationsPerThread = 5000;

    aaa::c_spinlock s;
    INT64 counter = 0;

    std::vector< std::thread > workers;
    for( int i = 0; i < kThreads; ++i )
    {
        workers.emplace_back( [ & ]() {
            for( int j = 0; j < kIterationsPerThread; ++j )
            {
                aaa::lock_guard< aaa::c_spinlock > g( s );
                ++counter;
            }
        } );
    }
    for( auto& w : workers )
        w.join();

    EXPECT_EQ( counter, static_cast< INT64 >( kThreads ) * kIterationsPerThread );
}
