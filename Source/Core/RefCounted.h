#pragma once

#define HANDLE_HEAVY_DEBUG
#ifdef HANDLE_HEAVY_DEBUG
#include "Threading/AssertRWMutex.h"
#endif

// a base for objects that need to be managed via Handle
class RefCounted
{
public:
	RefCounted();

	// Necessary to allow calling child destructors 
	// and avoiding memory leaks
	virtual ~RefCounted() {}

private:
	// ==========================
	// Handle<T> support
	template<typename T>
	friend class Handle;
	void AddRef() {	myCounter++; }
	void RemoveRef();
	uint32_t GetRefCount() const { return myCounter; }
	// ==========================

private:
	atomic<uint32_t> myCounter;
};

// =======================================================

// A smart-handle that shares ownership of RefCounted-ables.
// Not thread-safe. Will assert on contention.
// To avoid threading issues, don't pass around by refs around threads,
// instead pass by value, so every thread has it's own object
template<typename T>
class Handle
{
	static_assert(is_base_of_v<RefCounted, T>, "T isn't RefCounted-based!");
public:
	Handle()
		: myObject(nullptr)
	{
	}

	Handle(RefCounted* anObject)
		: myObject(anObject)
	{
		if (myObject)
		{
			myObject->AddRef();
		}
	}

	Handle(const Handle& aHandle)
	{
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock theirLock(aHandle.myDebugMutex);
#endif
		myObject = aHandle.myObject;
		if (myObject)
		{
			myObject->AddRef();
		}
	}

	Handle(Handle&& aHandle)
	{
		myObject = aHandle.myObject;
		aHandle.myObject = nullptr;
	}

	~Handle()
	{
		if (myObject)
		{
			myObject->RemoveRef();
		}
	}

	Handle& operator=(const Handle& aHandle)
	{
#ifdef HANDLE_HEAVY_DEBUG
		AssertWriteLock ourLock(myDebugMutex);
		AssertReadLock theirLock(aHandle.myDebugMutex);
#endif
		if (myObject)
		{
			myObject->RemoveRef();
		}

		myObject = aHandle.myObject;
		myObject->AddRef();
		return *this;
	}

	// Accessor of internal object casted to type of handle
	T* Get() 
	{
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		return static_cast<T*>(myObject); 
	}

	// Const accessor of internal object casted to type of handle
	const T* Get() const 
	{ 
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		return static_cast<const T*>(myObject); 
	}

	// Accessor of internal object casted to other type, if possible
	template<typename TOther>
	TOther* Get() 
	{ 
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		static_assert(is_convertible_v<T*, TOther*>, "Can't convert types!");
		return static_cast<TOther*>(myObject); 
	}

	// Const accessor of internal object casted to other type, if possible
	template<typename TOther>
	const TOther* Get() const 
	{ 
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		static_assert(is_convertible_v<T*, TOther*>, "Can't convert types!");
		return static_cast<const TOther*>(myObject); 
	}

	bool IsValid() const 
	{ 
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		return myObject != nullptr; 
	}

	bool IsLastHandle() const
	{
#ifdef HANDLE_HEAVY_DEBUG
		AssertReadLock ourLock(myDebugMutex);
#endif
		return myObject->GetRefCount() == 1;
	}

	T* operator->()
	{
		return Get();
	}

	const T* operator->() const
	{
		return Get();
	}

private:
	RefCounted* myObject;
#ifdef HANDLE_HEAVY_DEBUG
	mutable AssertRWMutex myDebugMutex;
#endif
};