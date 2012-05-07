/*
Flash Avatar support plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors
			Copyright (C)	2001-2007 Big Muscle
							2001-2006 Jacek Sieka


Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#if !defined(CRITICAL_SECTION_H)
#define CRITICAL_SECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CriticalSection  
{
public:
	void enter() throw() {
		EnterCriticalSection(&cs);
	}
	void leave() throw() {
		LeaveCriticalSection(&cs);
	}
	CriticalSection() throw() {
		InitializeCriticalSection(&cs);
	}
	~CriticalSection() throw() {
		DeleteCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;

	CriticalSection(const CriticalSection&);
	CriticalSection& operator=(const CriticalSection&);
};

template<class T>
class LockBase {
public:
	LockBase(T& aCs) throw() : cs(aCs)  { cs.enter(); }
	~LockBase() throw() { cs.leave(); }
private:
	LockBase& operator=(const LockBase&);
	T& cs;
};
typedef LockBase<CriticalSection> Lock;

/*
template<class T = CriticalSection>
class RWLock
{
public:
	RWLock() throw() : cs(), readers(0) { }
	~RWLock() throw() { }

	void enterRead() throw() {
		cs.enter();
		InterlockedIncrement(&readers);
		cs.leave();
	}
	void leaveRead() throw() {
		InterlockedDecrement(&readers);
	}
	void enterWrite() throw() {
		cs.enter();
		while(readers > 0) {
			::Sleep(1);
		}
	}
	void leaveWrite() {
		cs.leave();
	}
private:
	T cs;
	volatile long readers;
};

template<class T = CriticalSection>
class RLock {
public:
	RLock(RWLock<T>& aRwl) throw() : rwl(aRwl)  { rwl.enterRead(); }
	~RLock() throw() { rwl.leaveRead(); }
private:
	RLock& operator=(const RLock&);
	RWLock<T>& rwl;
};

template<class T = CriticalSection>
class WLock {
public:
	WLock(RWLock<T>& aRwl) throw() : rwl(aRwl)  { rwl.enterWrite(); }
	~WLock() throw() { rwl.leaveWrite(); }
private:
	WLock& operator=(const WLock&);
	RWLock<T>& rwl;
};
*/
#endif // !defined(CRITICAL_SECTION_H)

/**
 * @file
 * $Id: CriticalSection.h,v 1.20 2006/03/05 10:17:03 bigmuscle Exp $
 */
