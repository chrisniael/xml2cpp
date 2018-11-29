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
  config::gateserver().Init("gateserver.xml.example");

  std::cout << config::gateserver().name().as_string() << std::endl;
  std::cout << config::gateserver().thread().as_string() << std::endl;

  std::cout << config::gateserver().item().ip.as_string() << std::endl;
  std::cout << config::gateserver().test().name.as_string() << std::endl;

  std::cout << config::gateserver().a().id.as_string() << std::endl;
  std::cout << config::gateserver().a().name().as_string() << std::endl;

  std::cout << config::gateserver().item2().name().as_string() << std::endl;
  std::cout << config::gateserver().item2().a().id.as_string() << std::endl;
  std::cout << config::gateserver().item2().a().name().as_string() << std::endl;

  std::cout << "rep_size:" << config::gateserver().day().size() << std::endl;
  for (auto& day : config::gateserver().day()) {
    std::cout << day.as_string() << std::endl;
  }

  std::cout << "" << config::gateserver().b().c().day().size() << std::endl;
  for (auto& day : config::gateserver().day()) {
    std::cout << day.as_string() << std::endl;
  }

  std::cout << config::gateserver().b().c().name().as_string() << std::endl;

  std::cout << config::gateserver().b().c().item().size() << std::endl;
  for (auto& item : config::gateserver().b().c().item()) {
    std::cout << item.as_string() << std::endl;
  }

  std::cout << config::gateserver().item3().size() << std::endl;
  for (auto& item3 : config::gateserver().item3()) {
    std::cout << item3.ip.as_string() << ", " << item3.ip.as_string()
              << std::endl;
  }

  std::cout << config::gateserver().d().item3().size() << std::endl;
  for (auto& item3 : config::gateserver().d().item3()) {
    std::cout << item3.ip.as_string() << std::endl;
    std::cout << item3.port.as_string() << std::endl;
    std::cout << item3.a().as_string() << std::endl;
    std::cout << item3.b().as_string() << std::endl;
  }
}
