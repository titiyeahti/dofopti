#ifndef LINPROBJSON_H
#define LINPROBJSON_H

#include "glpapi.h"
#include <stdio.h>
#include <json-c/json.h>

json_object* sol_to_json(linprob_s* lp, pbdata_s* pbd, sqlite3* db);

#endif
