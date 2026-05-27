# AAA — Threading Primitives

All types live in the `aaa` namespace.

## Mutex Types (`aaa_mutex.h/cpp`)

| Type | Underlying | Notes |
|------|-----------|-------|
| `c_mutex_ct` | Windows `CRITICAL_SECTION` | Fast, non-recursive by default; reentry debug via `AAA_THREAD_TRACK_REENTRY()` |
| `c_mutex` / `aaa::MUTEX` | `std::mutex` | Cross-platform, non-recursive |
| `c_mutex_recursive` / `aaa::MUTEX_RECURSIVE` | `std::recursive_mutex` | Allows same-thread relock |
| `c_spinlock` | `std::atomic_flag` | `acquire/release` semantics; yields on contention via `std::this_thread::yield()` (not `SwitchToThread` — changed Sept 2020) |
| `c_benaphore` | `CRITICAL_SECTION` + Windows semaphore | Atomic pre-check counter; falls back to `WaitForSingleObject` |
| `c_benaphore_recursive` | same | Tracks owner thread ID (`_owner`) and recursion count (`_recursion`) |

All types provide: `lock()`, `try_lock()`, `unlock()`.

## Lock Guards (`aaa_lock_guard.h`)

`aaa::lock_guard<TMutex>` — non-copyable RAII:
- `lock_guard(TMutex&)` — locks immediately
- `lock_guard(TMutex&, adopt_lock_t)` — assumes already locked

`aaa::unique_lock<TMutex>` — RAII with ownership transfer:
- `unique_lock(TMutex&)` — locks immediately
- `unique_lock(TMutex&, adopt_lock_t)` — assumes locked
- `unique_lock(TMutex&, defer_lock_t)` — deferred; caller must call `lock()`
- Move constructor and move `operator=` transfer ownership
- `swap()` + `std::swap()` overload

Type aliases (`aaa_lock.h`):
```cpp
aaa::mutex_t        = aaa::MUTEX
aaa::lock_guard_t   = aaa::lock_guard<aaa::mutex_t>
aaa::unique_lock_t  = aaa::unique_lock<aaa::mutex_t>
```

## Geometry Structs (`aaa_rect.h`)

Plain structs in the `aaa::rect` namespace:
- `lbrt` — left, bottom, right, top (INT32)
- `lbrt_real` — same with REAL precision
- `xy_sxy` — x, y, size_x, size_y (INT32)
- `xy_sxy_real` — REAL variant
- `lbrt_sxy` / `lbrt_sxy_real` — combined format

## Process Execution (`execute.h/cpp`)

`aaa::execute_shell(command, args)` — wraps C `system()`, returns `AAA_ERR`.
`aaa::execute_process(command, args)` — wraps `CreateProcessA()` with `CREATE_NEW_CONSOLE`.
Both log via `DBG_PRINT_STRING()`.
