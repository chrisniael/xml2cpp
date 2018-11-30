/**
 * @file test.cpp
 * @brief test
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-27
 */

#include <iostream>

#include "test.xml.h"

int main() {
  if (!config::test().Init("config/test.xml")) {
    std::cout << "Load xml fail." << std::endl;
    return 1;
  }

  std::cout << config::test().example().name.as_string() << std::endl;

  std::cout << config::test().example().log().name().as_string() << std::endl;
  std::cout << config::test().example().log().level().as_string() << std::endl;
  std::cout << config::test().example().log().thread().as_string() << std::endl;

  for (auto& server : config::test().example().servers().server()) {
    std::cout << server.ip.as_string() << std::endl;
    std::cout << server.port.as_string() << std::endl;
  }
}
