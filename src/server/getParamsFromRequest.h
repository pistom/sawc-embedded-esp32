#include <HTTPRequest.hpp>
using namespace httpsserver;
using namespace std;

#ifndef GET_PARAMS_FROM_REQUEST_H
#define GET_PARAMS_FROM_REQUEST_H

tuple<string, string, int, int, int, int> getParamsFromRequest(HTTPRequest * req);

#endif