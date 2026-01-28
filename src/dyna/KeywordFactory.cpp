#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

KeywordFactory& KeywordFactory::instance() {
    static KeywordFactory factory;
    return factory;
}

bool KeywordFactory::registerKeyword(const std::string& name, KeywordCreator creator) {
    std::string upperName = util::StringUtils::toUpper(name);
    auto result = creators_.emplace(upperName, std::move(creator));
    return result.second;
}

std::unique_ptr<Keyword> KeywordFactory::create(const std::string& name) const {
    std::string upperName = util::StringUtils::toUpper(name);
    auto it = creators_.find(upperName);
    if (it != creators_.end()) {
        return it->second();
    }
    // Return GenericKeyword for unknown keywords
    return std::make_unique<GenericKeyword>(upperName);
}

bool KeywordFactory::isRegistered(const std::string& name) const {
    std::string upperName = util::StringUtils::toUpper(name);
    return creators_.find(upperName) != creators_.end();
}

std::vector<std::string> KeywordFactory::getRegisteredKeywords() const {
    std::vector<std::string> result;
    result.reserve(creators_.size());
    for (const auto& pair : creators_) {
        result.push_back(pair.first);
    }
    return result;
}

} // namespace koo::dyna
