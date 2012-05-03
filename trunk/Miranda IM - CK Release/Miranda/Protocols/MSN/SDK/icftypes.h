/*
MSN Protocol plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (c) 2006-2012 Boris Krasnovskiy.
			Copyright (c) 2003-2005 George Hazan.
			Copyright (c) 2002-2003 Richard Hughes (original version).

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

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __icftypes_h__
#define __icftypes_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_icftypes_0000_0000 */
/* [local] */ 

#include <winapifamily.h>
#pragma region Desktop Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
typedef 
enum NET_FW_POLICY_TYPE_
    {
        NET_FW_POLICY_GROUP	= 0,
        NET_FW_POLICY_LOCAL	= ( NET_FW_POLICY_GROUP + 1 ) ,
        NET_FW_POLICY_EFFECTIVE	= ( NET_FW_POLICY_LOCAL + 1 ) ,
        NET_FW_POLICY_TYPE_MAX	= ( NET_FW_POLICY_EFFECTIVE + 1 ) 
    } 	NET_FW_POLICY_TYPE;

typedef 
enum NET_FW_PROFILE_TYPE_
    {
        NET_FW_PROFILE_DOMAIN	= 0,
        NET_FW_PROFILE_STANDARD	= ( NET_FW_PROFILE_DOMAIN + 1 ) ,
        NET_FW_PROFILE_CURRENT	= ( NET_FW_PROFILE_STANDARD + 1 ) ,
        NET_FW_PROFILE_TYPE_MAX	= ( NET_FW_PROFILE_CURRENT + 1 ) 
    } 	NET_FW_PROFILE_TYPE;

typedef 
enum NET_FW_PROFILE_TYPE2_
    {
        NET_FW_PROFILE2_DOMAIN	= 0x1,
        NET_FW_PROFILE2_PRIVATE	= 0x2,
        NET_FW_PROFILE2_PUBLIC	= 0x4,
        NET_FW_PROFILE2_ALL	= 0x7fffffff
    } 	NET_FW_PROFILE_TYPE2;

typedef 
enum NET_FW_IP_VERSION_
    {
        NET_FW_IP_VERSION_V4	= 0,
        NET_FW_IP_VERSION_V6	= ( NET_FW_IP_VERSION_V4 + 1 ) ,
        NET_FW_IP_VERSION_ANY	= ( NET_FW_IP_VERSION_V6 + 1 ) ,
        NET_FW_IP_VERSION_MAX	= ( NET_FW_IP_VERSION_ANY + 1 ) 
    } 	NET_FW_IP_VERSION;

typedef 
enum NET_FW_SCOPE_
    {
        NET_FW_SCOPE_ALL	= 0,
        NET_FW_SCOPE_LOCAL_SUBNET	= ( NET_FW_SCOPE_ALL + 1 ) ,
        NET_FW_SCOPE_CUSTOM	= ( NET_FW_SCOPE_LOCAL_SUBNET + 1 ) ,
        NET_FW_SCOPE_MAX	= ( NET_FW_SCOPE_CUSTOM + 1 ) 
    } 	NET_FW_SCOPE;

typedef 
enum NET_FW_IP_PROTOCOL_
    {
        NET_FW_IP_PROTOCOL_TCP	= 6,
        NET_FW_IP_PROTOCOL_UDP	= 17,
        NET_FW_IP_PROTOCOL_ANY	= 256
    } 	NET_FW_IP_PROTOCOL;

typedef 
enum NET_FW_SERVICE_TYPE_
    {
        NET_FW_SERVICE_FILE_AND_PRINT	= 0,
        NET_FW_SERVICE_UPNP	= ( NET_FW_SERVICE_FILE_AND_PRINT + 1 ) ,
        NET_FW_SERVICE_REMOTE_DESKTOP	= ( NET_FW_SERVICE_UPNP + 1 ) ,
        NET_FW_SERVICE_NONE	= ( NET_FW_SERVICE_REMOTE_DESKTOP + 1 ) ,
        NET_FW_SERVICE_TYPE_MAX	= ( NET_FW_SERVICE_NONE + 1 ) 
    } 	NET_FW_SERVICE_TYPE;

typedef 
enum NET_FW_RULE_DIRECTION_
    {
        NET_FW_RULE_DIR_IN	= 1,
        NET_FW_RULE_DIR_OUT	= ( NET_FW_RULE_DIR_IN + 1 ) ,
        NET_FW_RULE_DIR_MAX	= ( NET_FW_RULE_DIR_OUT + 1 ) 
    } 	NET_FW_RULE_DIRECTION;

typedef 
enum NET_FW_ACTION_
    {
        NET_FW_ACTION_BLOCK	= 0,
        NET_FW_ACTION_ALLOW	= ( NET_FW_ACTION_BLOCK + 1 ) ,
        NET_FW_ACTION_MAX	= ( NET_FW_ACTION_ALLOW + 1 ) 
    } 	NET_FW_ACTION;

typedef 
enum NET_FW_MODIFY_STATE_
    {
        NET_FW_MODIFY_STATE_OK	= 0,
        NET_FW_MODIFY_STATE_GP_OVERRIDE	= ( NET_FW_MODIFY_STATE_OK + 1 ) ,
        NET_FW_MODIFY_STATE_INBOUND_BLOCKED	= ( NET_FW_MODIFY_STATE_GP_OVERRIDE + 1 ) 
    } 	NET_FW_MODIFY_STATE;

typedef 
enum NET_FW_RULE_CATEGORY_
    {
        NET_FW_RULE_CATEGORY_BOOT	= 0,
        NET_FW_RULE_CATEGORY_STEALTH	= ( NET_FW_RULE_CATEGORY_BOOT + 1 ) ,
        NET_FW_RULE_CATEGORY_FIREWALL	= ( NET_FW_RULE_CATEGORY_STEALTH + 1 ) ,
        NET_FW_RULE_CATEGORY_CONSEC	= ( NET_FW_RULE_CATEGORY_FIREWALL + 1 ) ,
        NET_FW_RULE_CATEGORY_MAX	= ( NET_FW_RULE_CATEGORY_CONSEC + 1 ) 
    } 	NET_FW_RULE_CATEGORY;

typedef 
enum NET_FW_EDGE_TRAVERSAL_TYPE_
    {
        NET_FW_EDGE_TRAVERSAL_TYPE_DENY	= 0,
        NET_FW_EDGE_TRAVERSAL_TYPE_ALLOW	= ( NET_FW_EDGE_TRAVERSAL_TYPE_DENY + 1 ) ,
        NET_FW_EDGE_TRAVERSAL_TYPE_DEFER_TO_APP	= ( NET_FW_EDGE_TRAVERSAL_TYPE_ALLOW + 1 ) ,
        NET_FW_EDGE_TRAVERSAL_TYPE_DEFER_TO_USER	= ( NET_FW_EDGE_TRAVERSAL_TYPE_DEFER_TO_APP + 1 ) 
    } 	NET_FW_EDGE_TRAVERSAL_TYPE;

typedef 
enum NET_FW_AUTHENTICATE_TYPE_
    {
        NET_FW_AUTHENTICATE_NONE	= 0,
        NET_FW_AUTHENTICATE_NO_ENCAPSULATION	= ( NET_FW_AUTHENTICATE_NONE + 1 ) ,
        NET_FW_AUTHENTICATE_WITH_INTEGRITY	= ( NET_FW_AUTHENTICATE_NO_ENCAPSULATION + 1 ) ,
        NET_FW_AUTHENTICATE_AND_NEGOTIATE_ENCRYPTION	= ( NET_FW_AUTHENTICATE_WITH_INTEGRITY + 1 ) ,
        NET_FW_AUTHENTICATE_AND_ENCRYPT	= ( NET_FW_AUTHENTICATE_AND_NEGOTIATE_ENCRYPTION + 1 ) 
    } 	NET_FW_AUTHENTICATE_TYPE;

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */
#pragma endregion


extern RPC_IF_HANDLE __MIDL_itf_icftypes_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_icftypes_0000_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


