#include <lua.h>
#include <string.h>
#include <stdio.h>

#define LUA_QL(x)	"'" x "'"
#define LUA_QS		LUA_QL("%s")

static void tag_error(lua_State *L, int narg, int tag) {
	luaL_typerror(L, narg, lua_typename(L, tag));
}

int luaL_argerror(lua_State * L, int narg, const char * extramsg) {
	return luaL_error(L, "bad argument #%d (%s) in C++ plugin", narg, extramsg);
}

int luaL_checkoption(lua_State * L, int narg, const char * def, const char * const lst[]) {
	const char *name = (def) ? luaL_optstring(L, narg, def) :
		luaL_checkstring(L, narg);
	int i;
	for (i = 0; lst[i]; i++)
		if (strcmp(lst[i], name) == 0)
			return i;

	char buff[1024];
	snprintf(buff, sizeof(buff), "invalid option " LUA_QS, name);
	return luaL_argerror(L, narg, buff);
}

int luaL_typerror(lua_State * L, int narg, const char * tname) {
	char msg[1024];
	snprintf(msg, sizeof(msg), "%s expected, got %s", tname, luaL_typename(L, narg));
	return luaL_argerror(L, narg, msg);
}

void luaL_checktype(lua_State * L, int narg, int t) {
	if (lua_type(L, narg) != t)
		tag_error(L, narg, t);
}

void luaL_checkany(lua_State * L, int narg) {
	if (lua_type(L, narg) == LUA_TNONE)
		luaL_argerror(L, narg, "value expected");
}

const char * luaL_checklstring(lua_State * L, int narg, size_t * len) {
	const char *s = lua_tolstring(L, narg, len);
	if (!s) tag_error(L, narg, LUA_TSTRING);
	return s;
}

const char * luaL_optlstring(lua_State * L, int narg, const char * def, size_t * len) {
	if (lua_isnoneornil(L, narg)) {
		if (len)
			*len = (def ? strlen(def) : 0);
		return def;
	}
	else return luaL_checklstring(L, narg, len);
}

lua_Number luaL_checknumber(lua_State * L, int narg) {
	lua_Number d = lua_tonumber(L, narg);
	if (d == 0 && !lua_isnumber(L, narg))  /* avoid extra test when d is not 0 */
		tag_error(L, narg, LUA_TNUMBER);
	return d;
}

lua_Number luaL_optnumber(lua_State * L, int narg, lua_Number def) {
	return luaL_opt(L, luaL_checknumber, narg, def);
}

lua_Integer luaL_checkinteger(lua_State * L, int narg) {
	lua_Integer d = lua_tointeger(L, narg);
	if (d == 0 && !lua_isnumber(L, narg))  /* avoid extra test when d is not 0 */
		tag_error(L, narg, LUA_TNUMBER);
	return d;
}

lua_Integer luaL_optinteger(lua_State * L, int narg, lua_Integer def) {
	return luaL_opt(L, luaL_checkinteger, narg, def);
}
