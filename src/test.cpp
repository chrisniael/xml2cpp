/**
 * @file test.cpp
 * @brief test
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-27
 */

#include <iostream>

#include "gateserver.xml.h"

int main() {
  config::Init();

  std::cout << config::gateserver().id.as_string() << std::endl;
  std::cout << config::gateserver().ip.as_string() << std::endl;
  std::cout << config::gateserver().port.as_string() << std::endl;
  std::cout << config::gateserver().thread.as_string() << std::endl;
}
