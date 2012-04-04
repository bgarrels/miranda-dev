/*
Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2011-2012 Robert Pösel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"

int OmegleProto::RecvMsg(HANDLE hContact, PROTORECVEVENT *pre)
{
	return 0;
}

void OmegleProto::SendMsgWorker(void *p)
{
	if(p == NULL)
		return;
  
	ScopedLock s( facy.send_message_lock_ );

	std::string *data = static_cast<std::string*>(p);

	*data = utils::text::trim(*data);

	if (data->length() && facy.send_message( *data ))
	{
		UpdateChat(facy.nick_.c_str(), data->c_str());
	}

	delete data;
}

void OmegleProto::SendTypingWorker(void *p)
{
	if (p == NULL)
		return;

	std::string *data = static_cast<std::string*>(p);

	bool typ = (*data == "1");
	delete data;
	
	if (typ)
		facy.typing_start();
	else
		facy.typing_stop();
}

void OmegleProto::NewChatWorker(void*p)
{
	NewChat();
}

void OmegleProto::StopChatWorker(void*p)
{
	StopChat();
}

int OmegleProto::SendMsg(HANDLE hContact, int flags, const char *msg)
{
	// TODO: msg comes as Unicode (retyped wchar_t*), why should we convert it as ANSI to UTF-8? o_O
/*	if ( flags & PREF_UNICODE )
		msg = mir_utf8encode(msg);
  
	facy.msgid_ = (facy.msgid_ % 1024)+1;
	ForkThread( &OmegleProto::SendMsgWorker, this,new send_direct(hContact,msg,(HANDLE)facy.msgid_) );
	return facy.msgid_;*/
	return 0;
}
