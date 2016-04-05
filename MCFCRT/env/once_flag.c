// 这个文件是 MCF 的一部分。
// 有关具体授权说明，请参阅 MCFLicense.txt。
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#include "once_flag.h"
#include "_nt_timeout.h"
#include "../ext/assert.h"
#include "../ext/expect.h"
#include <winternl.h>
#include <ntstatus.h>

extern __attribute__((__dllimport__, __stdcall__))
NTSTATUS NtWaitForKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);
extern __attribute__((__dllimport__, __stdcall__))
NTSTATUS NtReleaseKeyedEvent(HANDLE hKeyedEvent, void *pKey, BOOLEAN bAlertable, const LARGE_INTEGER *pliTimeout);

#define MASK_LOCKED             ((uintptr_t) 0x0001)
#define MASK_FINISHED           ((uintptr_t) 0x0002)
#define MASK_THREADS_TRAPPED    ((uintptr_t)~0x000F)

static _MCFCRT_OnceResult RealWaitForOnceFlag(volatile uintptr_t *puControl, bool bMayTimeOut, uint64_t u64UntilFastMonoClock){
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_CONSUME);
		if(_MCFCRT_EXPECT(uOld & MASK_FINISHED)){
			return _MCFCRT_kOnceResultFinished;
		}
		if(_MCFCRT_EXPECT(!(uOld & MASK_LOCKED))){
			uNew = uOld | MASK_LOCKED;
			if(_MCFCRT_EXPECT(__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_CONSUME))){
				return _MCFCRT_kOnceResultUninitialized;
			}
		}
	}
	if(bMayTimeOut && _MCFCRT_EXPECT(u64UntilFastMonoClock == 0)){
		return false;
	}

	for(;;){
		bool bFinished, bTaken;
		{
			uintptr_t uOld, uNew;
			uOld = __atomic_load_n(puControl, __ATOMIC_CONSUME);
			do {
				bFinished = !!(uOld & MASK_FINISHED);
				if(bFinished){
					break;
				}
				bTaken = !(uOld & MASK_LOCKED);
				if(!bTaken){
					uNew = uOld + (MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED);
				} else {
					uNew = uOld + MASK_LOCKED; // uOld | MASK_LOCKED;
				}
			} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_CONSUME)));
		}
		if(_MCFCRT_EXPECT(bFinished)){
			return _MCFCRT_kOnceResultFinished;
		}
		if(_MCFCRT_EXPECT(bTaken)){
			return _MCFCRT_kOnceResultUninitialized;
		}
		if(bMayTimeOut){
			LARGE_INTEGER liTimeout;
			__MCF_CRT_InitializeNtTimeout(&liTimeout, u64UntilFastMonoClock);
			NTSTATUS lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, &liTimeout);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() 失败。");
			if(_MCFCRT_EXPECT(lStatus == STATUS_TIMEOUT)){
				bool bDecremented;
				{
					uintptr_t uOld, uNew;
					uOld = __atomic_load_n(puControl, __ATOMIC_CONSUME);
					do {
						bDecremented = (uOld & MASK_THREADS_TRAPPED) > 0;
						if(!bDecremented){
							break;
						}
						uNew = uOld - (MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED);
					} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_CONSUME)));
				}
				if(bDecremented){
					return _MCFCRT_kOnceResultTimedOut;
				}
				lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, nullptr);
				_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() 失败。");
				_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
				return _MCFCRT_kOnceResultTimedOut;
			}
		} else {
			NTSTATUS lStatus = NtWaitForKeyedEvent(nullptr, (void *)puControl, false, nullptr);
			_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtWaitForKeyedEvent() 失败。");
			_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
		}
	}
	return _MCFCRT_kOnceResultUninitialized;
}
static void RealSetAndSignalOnceFlag(volatile uintptr_t *puControl, bool bFinished){
	bool bSignalOne;
	{
		uintptr_t uOld, uNew;
		uOld = __atomic_load_n(puControl, __ATOMIC_CONSUME);
		do {
			_MCFCRT_ASSERT_MSG(uOld & MASK_LOCKED,      L"一次性初始化标志没有被任何线程锁定。");
			_MCFCRT_ASSERT_MSG(!(uOld & MASK_FINISHED), L"一次性初始化标志已被使用。");

			uNew = uOld & ~MASK_LOCKED;
			bSignalOne = (uOld & MASK_THREADS_TRAPPED) > 0;
			if(bSignalOne){
				uNew -= (MASK_THREADS_TRAPPED & -MASK_THREADS_TRAPPED);
			}
			if(bFinished){
				uNew |= MASK_FINISHED;
			}
		} while(_MCFCRT_EXPECT_NOT(!__atomic_compare_exchange_n(puControl, &uOld, uNew, false, __ATOMIC_ACQ_REL, __ATOMIC_CONSUME)));
	}
	if(bSignalOne){
		NTSTATUS lStatus = NtReleaseKeyedEvent(nullptr, (void *)puControl, false, nullptr);
		_MCFCRT_ASSERT_MSG(NT_SUCCESS(lStatus), L"NtReleaseKeyedEvent() 失败。");
		_MCFCRT_ASSERT(lStatus != STATUS_TIMEOUT);
	}
}

_MCFCRT_OnceResult _MCFCRT_WaitForOnceFlag(_MCFCRT_OnceFlag *pOnceFlag, _MCFCRT_STD uint64_t u64UntilFastMonoClock){
	return RealWaitForOnceFlag((volatile uintptr_t *)pOnceFlag, true, u64UntilFastMonoClock);
}
_MCFCRT_OnceResult _MCFCRT_WaitForOnceFlagForever(_MCFCRT_OnceFlag *pOnceFlag){
	return RealWaitForOnceFlag((volatile uintptr_t *)pOnceFlag, false, UINT64_MAX);
}
void _MCFCRT_SignalOnceFlagAsFinished(_MCFCRT_OnceFlag *pOnceFlag){
	RealSetAndSignalOnceFlag((volatile uintptr_t *)pOnceFlag, true);
}
void _MCFCRT_SignalOnceFlagAsAborted(_MCFCRT_OnceFlag *pOnceFlag){
	RealSetAndSignalOnceFlag((volatile uintptr_t *)pOnceFlag, false);
}