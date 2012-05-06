/*
** $Id: lapi.h 1738 2012-03-12 21:30:27Z Nvinside@gmail.com $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include "lobject.h"


void luaA_pushobject (lua_State *L, const TObject *o);

#endif
