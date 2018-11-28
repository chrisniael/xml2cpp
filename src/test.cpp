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
  config::Init("gateserver.xml.example");

  std::cout << config::name().as_string() << std::endl;
  std::cout << config::thread().as_string() << std::endl;

  std::cout << config::item().ip.as_string() << std::endl;
  std::cout << config::test().name.as_string() << std::endl;

  std::cout << config::a().id.as_string() << std::endl;
  std::cout << config::a().name().as_string() << std::endl;

  std::cout << config::item2().name().as_string() << std::endl;
  std::cout << config::item2().a().id.as_string() << std::endl;
  std::cout << config::item2().a().name().as_string() << std::endl;

  std::cout << "rep_size:" << config::day().size() << std::endl;
  for (auto& day : config::day()) {
    std::cout << day.as_string() << std::endl;
  }

  std::cout << "" << config::b().c().day().size() << std::endl;
  for (auto& day : config::day()) {
    std::cout << day.as_string() << std::endl;
  }

  std::cout << config::b().c().name().as_string() << std::endl;

  std::cout << config::b().c().item().size() << std::endl;
  for (auto& item : config::b().c().item()) {
    std::cout << item.as_string() << std::endl;
  }

  std::cout << config::item3().size() << std::endl;
  for (auto& item3 : config::item3()) {
    std::cout << item3.ip.as_string() << ", " << item3.ip.as_string()
              << std::endl;
  }

  std::cout << config::d().item3().size() << std::endl;
  for (auto& item3 : config::d().item3()) {
    std::cout << item3.ip.as_string() << std::endl;
    std::cout << item3.port.as_string() << std::endl;
    std::cout << item3.a().as_string() << std::endl;
    std::cout << item3.b().as_string() << std::endl;
  }
}
