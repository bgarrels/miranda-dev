/* 
Copyright �2006 Ricardo Pescuma Domenecci

Modified  �2008-2010 DeathAxe, Yasnovidyashii, Merlin, K. Romanov, Kreol

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

===============================================================================

File name      : $HeadURL: http://userinfoex.googlecode.com/svn/trunk/mir_contactqueue.h $
Revision       : $Revision: 187 $
Last change on : $Date: 2010-09-08 16:05:54 +0400 (Ср, 08 сен 2010) $
Last change by : $Author: ing.u.horn $

===============================================================================
*/


#ifndef __CONTACTASYNCQUEUE_H__
#define __CONTACTASYNCQUEUE_H__

#ifndef MIRANDA_VER
#define MIRANDA_VER 0x0900
#endif

#include <windows.h>
#include <newpluginapi.h>
#include <m_system_cpp.h>

/**
 *
 *
 **/
struct CQueueItem
{
	DWORD	check_time;
	HANDLE	hContact;
	PVOID	param;
};

/**
 *
 *
 **/
class CContactQueue
{
public:

	enum EQueueStatus
	{
		RUNNING		= 0,
		STOPPING	= 1,
		STOPPED		= 2
	};

	CContactQueue				(INT initialSize = 10);
	~CContactQueue				();

	inline INT			Size	()			const	{ return _queue.getCount();}
	inline INT			Remove	(INT idx)			{ mir_free(_queue[idx]); return _queue.remove(idx);}
	inline CQueueItem*	Get		(INT idx) const		{ return _queue[idx];}


	VOID RemoveAll();
	
	/**
	 * This function removes all queue items for the hContact.
	 *
	 * @param		hContact		- the contact whose queue items to delete
	 *
	 * @return	nothing
	 **/
	VOID RemoveAll(HANDLE hContact);

	/**
	 * This function removes all queue items for the hContact considering the correct parameter.
	 *
	 * @param		hContact		- the contact whose queue items to delete
	 * @param		param			- a caller defined parameter passed to the callback function
	 *
	 * @return	nothing
	 **/
	VOID RemoveAllConsiderParam(HANDLE hContact, PVOID param);

	/**
	 * This method adds the desired new item.
	 *
	 * @param		waitTime		- the time to wait until the callback is desired to run
	 * @param		hContact		- the contact to perform the action for
	 * @param		param			- a caller defined parameter passed to the callback function
	 *
	 * @retval		TRUE			- The item is added to the queue successfully.
	 * @retval		FALSE			- The item is not added to the queue.
	 **/
	BOOL Add(INT waitTime, HANDLE hContact, PVOID param = NULL);

	/**
	 * This method adds the desired new item only, if the queue does not yet contain
	 * an item for the contact.
	 *
	 * @param		waitTime		- the time to wait until the callback is desired to run
	 * @param		hContact		- the contact to perform the action for
	 * @param		param			- a caller defined parameter passed to the callback function
	 *
	 * @retval		TRUE			- The item is added to the queue successfully.
	 * @retval		FALSE			- The item is not added to the queue.
	 **/
	BOOL AddIfDontHave(INT waitTime, HANDLE hContact, PVOID param = NULL);

	/**
	 * This method removes all existing queue items for the contact and adds a new queue item
	 * for the given contact. This method might be used to move an existing entry, 
	 * whose check_time has changed.
	 *
	 * @param		waitTime		- the time to wait until the callback is desired to run
	 * @param		hContact		- the contact to perform the action for
	 * @param		param			- a caller defined parameter passed to the callback function
	 *
	 * @return		nothing
	 **/
	BOOL AddUnique(INT waitTime, HANDLE hContact, PVOID param = NULL);

	/**
	 * This method removes all existing queue items for the contact with the same parameter as @e param
	 * and adds a new queue item for the given contact. This method might be used to move an existing
	 * entry, whose check_time has changed.
	 *
	 * @param		waitTime		- the time to wait until the callback is desired to run
	 * @param		hContact		- the contact to perform the action for
	 * @param		param			- a caller defined parameter passed to the callback function
	 *
	 * @return	nothing
	 **/
	BOOL AddUniqueConsiderParam	(INT waitTime, HANDLE hContact, PVOID param = NULL);

	/**
	 * This method resumes the worker thread and immitiatly goes on with the next entry.
	 *
	 * @param		none
	 *
	 * @return		nothing
	 **/
	VOID ContinueWithNext();

protected:
	
	virtual VOID OnEmpty		() {};
	virtual VOID Callback		(HANDLE hContact, PVOID param) = 0;

	/**
	 * This is the real thread callback function. As long as _status
	 * is set to RUNNING it looks for items in the queue to perform 
	 * the _pfnCallback function on them. If the queue is empty or the
	 * next upcoming item is located in the future, the thread is suspended
	 * in the meanwhile.
	 *
	 * @param		none
	 *
	 * @return	nothing
	 **/
	VOID Thread();
	
	/**
	 * This is a static method to redirect the thread's callback function
	 * to the desired class object.
	 *
	 * @param		obj	- pointer to the object (instance) of CContactQueue
	 *
	 * @return	nothing
	 **/
	static VOID ThreadProc(CContactQueue* obj)
	{
		obj->Thread();
	}

	/**
	 * This method suspends the worker thread for the given ammount of time.
	 *
	 * @param		time	- milliseconds to suspend the thread for
	 *
	 * @return	nothing
	 **/
	VOID Suspend(INT time) const;

private:

	LIST<CQueueItem>	_queue;

	CRITICAL_SECTION	_cs;
	HANDLE				_hEvent;
	EQueueStatus		_status;

	VOID Lock();
	VOID Release();

	/**
	 * This member function really adds an item into the time sorted queue list.
	 *
	 * @param		waitTime	- the time to wait until the callback is desired to run
	 * @param		hContact	- the contact to perform the action for
	 * @param		param		- a caller defined parameter passed to the callback function
	 *
	 * @retval		TRUE		- The item is added to the queue successfully.
	 * @retval		FALSE		- The item is not added to the queue.
	 **/
	BOOL InternalAdd(INT waitTime, HANDLE hContact, PVOID param);
};

#endif // __CONTACTASYNCQUEUE_H__