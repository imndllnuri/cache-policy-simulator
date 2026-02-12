#include "../inc/policy_factory.h"

PolicyFactory &PolicyFactory::instance() {
  static PolicyFactory factory;
  return factory;
}

void PolicyFactory::register_policy(const std::string &name, Creator creator) {
  registry_[name] = creator;
}

std::unique_ptr<ReplacementPolicy>
PolicyFactory::create(const std::string &name, size_t sets, size_t ways) {
  auto it = registry_.find(name);
  if (it != registry_.end()) {
    return it->second(sets, ways);
  }
  return nullptr;
}
