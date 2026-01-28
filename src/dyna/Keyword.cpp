#include <koo/dyna/Keyword.hpp>

namespace koo::dyna {

GenericKeyword::GenericKeyword(const std::string& keywordName)
    : keywordName_(keywordName) {}

bool GenericKeyword::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    rawLines_ = lines;
    format_ = format;
    return true;
}

std::vector<std::string> GenericKeyword::write(
    util::CardParser::Format /*format*/) const {
    return rawLines_;
}

void GenericKeyword::accept(ModelVisitor& /*visitor*/) {
    // Generic keywords don't have specific visitor methods
}

} // namespace koo::dyna
