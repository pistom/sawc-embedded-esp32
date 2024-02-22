#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

string generateRandomString(int length)
{
  const string charset = "abcdefhijkmnqrstuvwxyz23456789";
  std::string result;
  result.reserve(length);

  std::srand(std::time(0));

  for (int i = 0; i < length; ++i)
  {
    result += charset[std::rand() % charset.length()];
  }
  return result;
}