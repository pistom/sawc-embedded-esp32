#include <HTTPRequest.hpp>
#include "HTTPURLEncodedBodyParser.hpp"
#include "../AppConfig.h"
using namespace httpsserver;
using namespace std;

tuple<string, string, int, int, int, int> getParamsFromRequest(HTTPRequest * req) {
  HTTPURLEncodedBodyParser parser(req);
  string output = "";
  string nextOutput = "";

  string token = "";
  string action = "";
  int outputNumber = -1;
  int nextOutputNumber = -1;
  int duration = -1;
  int pumpDelayOff = 0;

  while(parser.nextField()) {
    string name = parser.getFieldName();
    char buf[512];
    size_t readLength = parser.read((byte *)buf, 512);
    if (name == "token") {
      token = string(buf, readLength);
    } else if (name == "output") {
      output = string(buf, readLength);
      if (output == "pump") {
        outputNumber = 0;
      } else if (stoi(output) != 0) {
        outputNumber = stoi(output);
        if (outputNumber > AppConfig::valvesNumber || outputNumber < 0) {
          outputNumber = -1;
        }
      }
    } else if (name == "nextOutput") {
      nextOutput = string(buf, readLength);
      if (stoi(nextOutput) > 0) {
        nextOutputNumber = stoi(nextOutput);
        if (nextOutputNumber > AppConfig::valvesNumber || nextOutputNumber < 0) {
          nextOutputNumber = -1;
        }
      }
    } else if (name == "action") {
      action = string(buf, readLength);
      if (action != "on" && action != "off") {
        action = "";
      }
    } else if (name == "duration") {
      duration = stoi(string(buf, readLength));
    } else if (name == "pumpDelayOff") {
      pumpDelayOff = stoi(string(buf, readLength)) + 1;
    }
  }
  return make_tuple(
    token,
    action,
    outputNumber,
    nextOutputNumber,
    duration,
    pumpDelayOff
  );
}