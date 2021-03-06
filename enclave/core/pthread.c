// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#include <openenclave/corelibc/errno.h>
#include <openenclave/corelibc/pthread.h>
#include <openenclave/corelibc/string.h>
#include <openenclave/enclave.h>
#include <openenclave/internal/defs.h>
#include <openenclave/internal/thread.h>
#include <openenclave/internal/trace.h>

OE_STATIC_ASSERT(sizeof(oe_pthread_once_t) == sizeof(oe_once_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_spinlock_t) == sizeof(oe_spinlock_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_mutex_t) >= sizeof(oe_mutex_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_cond_t) >= sizeof(oe_cond_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_rwlock_t) >= sizeof(oe_rwlock_t));

/* Map an oe_result_t to a POSIX error number */
OE_INLINE int _to_errno(oe_result_t result)
{
    switch (result)
    {
        case OE_OK:
            return 0;
        case OE_INVALID_PARAMETER:
            return OE_EINVAL;
        case OE_BUSY:
            return OE_EBUSY;
        case OE_NOT_OWNER:
            return OE_EPERM;
        case OE_OUT_OF_MEMORY:
            return OE_ENOMEM;
        case OE_TIMEDOUT:
            return OE_ETIMEDOUT;
        default:
            return OE_EINVAL; /* unreachable */
    }
}

/*
**==============================================================================
**
** pthread_attr_t
**
**==============================================================================
*/

// Internal attr implementation
typedef struct _oe_attr_impl
{
    bool detached;
} oe_attr_impl_t;

OE_STATIC_ASSERT(sizeof(oe_attr_impl_t) <= sizeof(oe_pthread_attr_t));

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

int oe_pthread_attr_init(oe_pthread_attr_t* attr)
{
    oe_assert(attr);
    memset(attr, 0, sizeof *attr);
    return 0;
}

int oe_pthread_attr_destroy(oe_pthread_attr_t* attr)
{
    oe_assert(attr);
    (void)attr;
    return 0;
}

int oe_pthread_attr_setdetachstate(oe_pthread_attr_t* attr, int detachstate)
{
    oe_assert(attr);
    oe_attr_impl_t* const iattr = (oe_attr_impl_t*)attr;

    switch (detachstate)
    {
        case PTHREAD_CREATE_JOINABLE:
            iattr->detached = false;
            break;
        case PTHREAD_CREATE_DETACHED:
            iattr->detached = true;
            break;
        default:
            return OE_EINVAL;
    }

    return 0;
}

/*
**==============================================================================
**
** pthread_t
**
**==============================================================================
*/

OE_STATIC_ASSERT(sizeof(oe_pthread_mutex_t) >= sizeof(oe_mutex_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_cond_t) >= sizeof(oe_cond_t));
OE_STATIC_ASSERT(sizeof(oe_pthread_t) == sizeof(oe_thread_t));

oe_pthread_t oe_pthread_self()
{
    return (oe_pthread_t)oe_thread_self();
}

int oe_pthread_equal(oe_pthread_t thread1, oe_pthread_t thread2)
{
    return (int)oe_thread_equal((oe_thread_t)thread1, (oe_thread_t)thread2);
}

int oe_pthread_create(
    oe_pthread_t* thread,
    const oe_pthread_attr_t* attr,
    void* (*start_routine)(void*),
    void* arg)
{
    const oe_result_t res = oe_thread_create(thread, start_routine, arg);

    if (res == OE_OK && attr && ((oe_attr_impl_t*)attr)->detached &&
        oe_thread_detach(*thread) != OE_OK)
        oe_abort();

    return _to_errno(res);
}

int oe_pthread_join(oe_pthread_t thread, void** retval)
{
    return _to_errno(oe_thread_join(thread, retval));
}

int oe_pthread_detach(oe_pthread_t thread)
{
    return _to_errno(oe_thread_detach(thread));
}

void oe_pthread_exit(void* retval)
{
    oe_thread_exit(retval);
}

/*
**==============================================================================
**
** pthread_once_t
**
**==============================================================================
*/

int oe_pthread_once(oe_pthread_once_t* once, void (*func)(void))
{
    return _to_errno(oe_once((oe_once_t*)once, func));
}

/*
**==============================================================================
**
** oe_pthread_spinlock_t
**
**==============================================================================
*/

int oe_pthread_spin_init(oe_pthread_spinlock_t* spinlock, int pshared)
{
    OE_UNUSED(pshared);
    return _to_errno(oe_spin_init((oe_spinlock_t*)spinlock));
}

int oe_pthread_spin_lock(oe_pthread_spinlock_t* spinlock)
{
    return _to_errno(oe_spin_lock((oe_spinlock_t*)spinlock));
}

int oe_pthread_spin_unlock(oe_pthread_spinlock_t* spinlock)
{
    return _to_errno(oe_spin_unlock((oe_spinlock_t*)spinlock));
}

int oe_pthread_spin_destroy(oe_pthread_spinlock_t* spinlock)
{
    return _to_errno(oe_spin_destroy((oe_spinlock_t*)spinlock));
}

/*
**==============================================================================
**
** oe_pthread_mutex_t
**
**==============================================================================
*/

int oe_pthread_mutexattr_init(oe_pthread_mutexattr_t* attr)
{
    OE_UNUSED(attr);
    return 0;
}

int oe_pthread_mutexattr_settype(oe_pthread_mutexattr_t* attr, int type)
{
    OE_UNUSED(attr);
    OE_UNUSED(type);
    return 0;
}

int oe_pthread_mutexattr_destroy(oe_pthread_mutexattr_t* attr)
{
    OE_UNUSED(attr);
    return 0;
}

int oe_pthread_mutex_init(
    oe_pthread_mutex_t* m,
    const oe_pthread_mutexattr_t* attr)
{
    OE_UNUSED(attr);
    return _to_errno(oe_mutex_init((oe_mutex_t*)m));
}

int oe_pthread_mutex_lock(oe_pthread_mutex_t* m)
{
    return _to_errno(oe_mutex_lock((oe_mutex_t*)m));
}

int oe_pthread_mutex_trylock(oe_pthread_mutex_t* m)
{
    return _to_errno(oe_mutex_trylock((oe_mutex_t*)m));
}

int oe_pthread_mutex_unlock(oe_pthread_mutex_t* m)
{
    return _to_errno(oe_mutex_unlock((oe_mutex_t*)m));
}

int oe_pthread_mutex_destroy(oe_pthread_mutex_t* m)
{
    return _to_errno(oe_mutex_destroy((oe_mutex_t*)m));
}

/*
**==============================================================================
**
** oe_pthread_rwlock_t
**
**==============================================================================
*/

int oe_pthread_rwlock_init(
    oe_pthread_rwlock_t* rwlock,
    const oe_pthread_rwlockattr_t* attr)
{
    OE_UNUSED(attr);
    return _to_errno(oe_rwlock_init((oe_rwlock_t*)rwlock));
}

int oe_pthread_rwlock_rdlock(oe_pthread_rwlock_t* rwlock)
{
    return _to_errno(oe_rwlock_rdlock((oe_rwlock_t*)rwlock));
}

int oe_pthread_rwlock_wrlock(oe_pthread_rwlock_t* rwlock)
{
    return _to_errno(oe_rwlock_wrlock((oe_rwlock_t*)rwlock));
}

int oe_pthread_rwlock_unlock(oe_pthread_rwlock_t* rwlock)
{
    return _to_errno(oe_rwlock_unlock((oe_rwlock_t*)rwlock));
}

int oe_pthread_rwlock_destroy(oe_pthread_rwlock_t* rwlock)
{
    return _to_errno(oe_rwlock_destroy((oe_rwlock_t*)rwlock));
}

/*
**==============================================================================
**
** oe_pthread_cond_t
**
**==============================================================================
*/

int oe_pthread_cond_init(
    oe_pthread_cond_t* cond,
    const oe_pthread_condattr_t* attr)
{
    OE_UNUSED(attr);
    return _to_errno(oe_cond_init((oe_cond_t*)cond, (oe_condattr_t*)attr));
}

int oe_pthread_cond_wait(oe_pthread_cond_t* cond, oe_pthread_mutex_t* mutex)
{
    return _to_errno(oe_cond_wait((oe_cond_t*)cond, (oe_mutex_t*)mutex));
}

int oe_pthread_cond_timedwait(
    oe_pthread_cond_t* cond,
    oe_pthread_mutex_t* mutex,
    const struct oe_timespec* ts)
{
    return _to_errno(
        oe_cond_timedwait((oe_cond_t*)cond, (oe_mutex_t*)mutex, ts));
}

int oe_pthread_cond_signal(oe_pthread_cond_t* cond)
{
    return _to_errno(oe_cond_signal((oe_cond_t*)cond));
}

int oe_pthread_cond_broadcast(oe_pthread_cond_t* cond)
{
    return _to_errno(oe_cond_broadcast((oe_cond_t*)cond));
}

int oe_pthread_cond_destroy(oe_pthread_cond_t* cond)
{
    return _to_errno(oe_cond_destroy((oe_cond_t*)cond));
}

int oe_pthread_condattr_init(oe_pthread_condattr_t* attr)
{
    return _to_errno(oe_condattr_init((oe_condattr_t*)attr));
}

int oe_pthread_condattr_setclock(oe_pthread_condattr_t* attr, int clockid)
{
    return _to_errno(oe_condattr_setclock((oe_condattr_t*)attr, clockid));
}

/*
**==============================================================================
**
** oe_pthread_key_t (thread specific data)
**
**==============================================================================
*/

int oe_pthread_key_create(
    oe_pthread_key_t* key,
    void (*destructor)(void* value))
{
    return _to_errno(oe_thread_key_create((oe_thread_key_t*)key, destructor));
}

OE_WEAK_ALIAS(oe_pthread_key_create, __pthread_key_create);

int oe_pthread_key_delete(oe_pthread_key_t key)
{
    return _to_errno(oe_thread_key_delete((oe_thread_key_t)key));
}

int oe_pthread_setspecific(oe_pthread_key_t key, const void* value)
{
    return _to_errno(oe_thread_setspecific((oe_thread_key_t)key, value));
}

void* oe_pthread_getspecific(oe_pthread_key_t key)
{
    return oe_thread_getspecific((oe_thread_key_t)key);
}
