#include <koo/dyna/Database.hpp>
#include <koo/dyna/ModelVisitor.hpp>
#include <koo/dyna/KeywordFactory.hpp>
#include <koo/util/StringUtils.hpp>

namespace koo::dyna {

// ============================================================================
// DatabaseBinaryD3plot
// ============================================================================

bool DatabaseBinaryD3plot::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.beam = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.npltc = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.psetid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBinaryD3plot::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.beam);
    writer.writeInt(data_.npltc);
    writer.writeInt(data_.psetid);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBinaryD3plot::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBinaryD3plot, "*DATABASE_BINARY_D3PLOT")

// ============================================================================
// DatabaseBinaryD3thdt
// ============================================================================

bool DatabaseBinaryD3thdt::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBinaryD3thdt::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBinaryD3thdt::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBinaryD3thdt, "*DATABASE_BINARY_D3THDT")

// ============================================================================
// DatabaseExtentBinary
// ============================================================================

bool DatabaseExtentBinary::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    int cardNum = 0;
    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        if (cardNum == 0) {
            // Card 1: NEIPH, NEIPS, MAXINT, STRFLG, SIGFLG, EPSFLG, RLTFLG, ENGFLG
            data_.neiph = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.neips = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.maxint = static_cast<int>(parser.getInt64At(pos).value_or(3));
            pos += intW;
            data_.strflg = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.sigflg = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.epsflg = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.rltflg = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.engflg = static_cast<int>(parser.getInt64At(pos).value_or(1));
        } else if (cardNum == 1) {
            // Card 2: CMPFLG, IEVERP, BEAMIP, DCOMP, SHGE, STSSZ, N3THDT, IALEMAT
            data_.cmpflg = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.ieverp = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.beamip = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.dcomp = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.shge = static_cast<int>(parser.getInt64At(pos).value_or(1));
            pos += intW;
            data_.stssz = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.n3thdt = static_cast<int>(parser.getInt64At(pos).value_or(2));
            pos += intW;
            data_.ialemat = static_cast<int>(parser.getInt64At(pos).value_or(1));
        } else if (cardNum == 2) {
            // Card 3: NINTSLD, PKP_SEN
            data_.nintsld = static_cast<int>(parser.getInt64At(pos).value_or(0));
            pos += intW;
            data_.pkp_sen = static_cast<int>(parser.getInt64At(pos).value_or(0));
        }

        cardNum++;
        if (cardNum >= 3) break;
    }

    return true;
}

std::vector<std::string> DatabaseExtentBinary::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.neiph);
    writer.writeInt(data_.neips);
    writer.writeInt(data_.maxint);
    writer.writeInt(data_.strflg);
    writer.writeInt(data_.sigflg);
    writer.writeInt(data_.epsflg);
    writer.writeInt(data_.rltflg);
    writer.writeInt(data_.engflg);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeInt(data_.cmpflg);
    writer.writeInt(data_.ieverp);
    writer.writeInt(data_.beamip);
    writer.writeInt(data_.dcomp);
    writer.writeInt(data_.shge);
    writer.writeInt(data_.stssz);
    writer.writeInt(data_.n3thdt);
    writer.writeInt(data_.ialemat);
    result.push_back(writer.getLine());

    // Card 3 (if non-default)
    if (data_.nintsld != 0 || data_.pkp_sen != 0) {
        writer.clear();
        writer.writeInt(data_.nintsld);
        writer.writeInt(data_.pkp_sen);
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseExtentBinary::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseExtentBinary, "*DATABASE_EXTENT_BINARY")

// ============================================================================
// DatabaseGlstat
// ============================================================================

bool DatabaseGlstat::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseGlstat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseGlstat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseGlstat, "*DATABASE_GLSTAT")

// ============================================================================
// DatabaseMatsum
// ============================================================================

bool DatabaseMatsum::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseMatsum::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseMatsum::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseMatsum, "*DATABASE_MATSUM")

// ============================================================================
// DatabaseNodout
// ============================================================================

bool DatabaseNodout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseNodout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.option);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseNodout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseNodout, "*DATABASE_NODOUT")

// ============================================================================
// DatabaseElout
// ============================================================================

bool DatabaseElout::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.option = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseElout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.option);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseElout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseElout, "*DATABASE_ELOUT")

// ============================================================================
// DatabaseRcforc
// ============================================================================

bool DatabaseRcforc::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseRcforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseRcforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseRcforc, "*DATABASE_RCFORC")

// ============================================================================
// DatabaseSleout
// ============================================================================

bool DatabaseSleout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSleout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSleout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSleout, "*DATABASE_SLEOUT")

// ============================================================================
// DatabaseHistoryNode
// ============================================================================

bool DatabaseHistoryNode::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    nodeIds_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        // 8 nodes per line
        for (size_t i = 0; i < 8; ++i) {
            size_t pos = i * intW;
            auto id = parser.getInt64At(pos);
            if (id.has_value() && id.value() != 0) {
                nodeIds_.push_back(id.value());
            }
        }
    }

    return true;
}

std::vector<std::string> DatabaseHistoryNode::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    size_t count = 0;
    for (auto id : nodeIds_) {
        writer.writeInt(id);
        count++;
        if (count % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (count % 8 != 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseHistoryNode::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseHistoryNode, "*DATABASE_HISTORY_NODE")

// ============================================================================
// DatabaseHistoryShell
// ============================================================================

bool DatabaseHistoryShell::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elementIds_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        for (size_t i = 0; i < 8; ++i) {
            size_t pos = i * intW;
            auto id = parser.getInt64At(pos);
            if (id.has_value() && id.value() != 0) {
                elementIds_.push_back(id.value());
            }
        }
    }

    return true;
}

std::vector<std::string> DatabaseHistoryShell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    size_t count = 0;
    for (auto id : elementIds_) {
        writer.writeInt(id);
        count++;
        if (count % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (count % 8 != 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseHistoryShell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseHistoryShell, "*DATABASE_HISTORY_SHELL")

// ============================================================================
// DatabaseHistorySolid
// ============================================================================

bool DatabaseHistorySolid::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elementIds_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        for (size_t i = 0; i < 8; ++i) {
            size_t pos = i * intW;
            auto id = parser.getInt64At(pos);
            if (id.has_value() && id.value() != 0) {
                elementIds_.push_back(id.value());
            }
        }
    }

    return true;
}

std::vector<std::string> DatabaseHistorySolid::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    size_t count = 0;
    for (auto id : elementIds_) {
        writer.writeInt(id);
        count++;
        if (count % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (count % 8 != 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseHistorySolid::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseHistorySolid, "*DATABASE_HISTORY_SOLID")

// ============================================================================
// DatabaseSpcforc
// ============================================================================

bool DatabaseSpcforc::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSpcforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSpcforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSpcforc, "*DATABASE_SPCFORC")

// ============================================================================
// DatabaseRwforc
// ============================================================================

bool DatabaseRwforc::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseRwforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseRwforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseRwforc, "*DATABASE_RWFORC")

// ============================================================================
// DatabaseAbstat
// ============================================================================

bool DatabaseAbstat::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseAbstat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseAbstat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseAbstat, "*DATABASE_ABSTAT")

// ============================================================================
// DatabaseSecforc
// ============================================================================

bool DatabaseSecforc::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSecforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSecforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSecforc, "*DATABASE_SECFORC")

// ============================================================================
// DatabaseJntforc
// ============================================================================

bool DatabaseJntforc::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseJntforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseJntforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseJntforc, "*DATABASE_JNTFORC")

// ============================================================================
// DatabaseBndout
// ============================================================================

bool DatabaseBndout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBndout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBndout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBndout, "*DATABASE_BNDOUT")

// ============================================================================
// DatabaseDeforc
// ============================================================================

bool DatabaseDeforc::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDeforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDeforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDeforc, "*DATABASE_DEFORC")

// ============================================================================
// DatabaseSwforc
// ============================================================================

bool DatabaseSwforc::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSwforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSwforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSwforc, "*DATABASE_SWFORC")

// ============================================================================
// DatabaseNcforc
// ============================================================================

bool DatabaseNcforc::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseNcforc::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseNcforc::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseNcforc, "*DATABASE_NCFORC")

// ============================================================================
// DatabaseHistoryBeam
// ============================================================================

bool DatabaseHistoryBeam::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    elementIds_.clear();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        for (size_t i = 0; i < 8; ++i) {
            size_t pos = i * intW;
            auto id = parser.getInt64At(pos);
            if (id.has_value() && id.value() != 0) {
                elementIds_.push_back(id.value());
            }
        }
    }

    return true;
}

std::vector<std::string> DatabaseHistoryBeam::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    size_t count = 0;
    for (auto id : elementIds_) {
        writer.writeInt(id);
        count++;
        if (count % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (count % 8 != 0) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseHistoryBeam::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseHistoryBeam, "*DATABASE_HISTORY_BEAM")

// ============================================================================
// DatabaseBinaryD3dump
// ============================================================================

bool DatabaseBinaryD3dump::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nrst = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBinaryD3dump::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.nrst);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBinaryD3dump::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBinaryD3dump, "*DATABASE_BINARY_D3DUMP")

// ============================================================================
// DatabaseBinaryRunrsf
// ============================================================================

bool DatabaseBinaryRunrsf::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.nrst = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBinaryRunrsf::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.lcdt);
    writer.writeInt(data_.nrst);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBinaryRunrsf::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBinaryRunrsf, "*DATABASE_BINARY_RUNRSF")

// ============================================================================
// DatabaseSsstat
// ============================================================================

bool DatabaseSsstat::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSsstat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSsstat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSsstat, "*DATABASE_SSSTAT")

// ============================================================================
// DatabaseRbdout
// ============================================================================

bool DatabaseRbdout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseRbdout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseRbdout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseRbdout, "*DATABASE_RBDOUT")

// ============================================================================
// DatabaseCurvout
// ============================================================================

bool DatabaseCurvout::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseCurvout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseCurvout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCurvout, "*DATABASE_CURVOUT")

// ============================================================================
// DatabaseTprint
// ============================================================================

bool DatabaseTprint::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseTprint::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseTprint::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseTprint, "*DATABASE_TPRINT")

// ============================================================================
// DatabaseNodfor
// ============================================================================

bool DatabaseNodfor::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseNodfor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseNodfor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseNodfor, "*DATABASE_NODFOR")

// ============================================================================
// DatabaseDcfail
// ============================================================================

bool DatabaseDcfail::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDcfail::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDcfail::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDcfail, "*DATABASE_DCFAIL")

// ============================================================================
// DatabaseBearing
// ============================================================================

bool DatabaseBearing::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t realW = parser.getRealWidth();
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;
        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcdt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBearing::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcdt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBearing::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBearing, "*DATABASE_BEARING")

// ============================================================================
// DatabaseCrossSectionSet
// ============================================================================

bool DatabaseCrossSectionSet::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: NSID, HSID, BSID, SSID, TSID, DSID, ID, ITYPE
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.nsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.hsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.bsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.ssid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.tsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.dsid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.itype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DatabaseCrossSectionSet::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.nsid);
    writer.writeInt(data_.hsid);
    writer.writeInt(data_.bsid);
    writer.writeInt(data_.ssid);
    writer.writeInt(data_.tsid);
    writer.writeInt(data_.dsid);
    writer.writeInt(data_.id);
    writer.writeInt(data_.itype);
    result.push_back(writer.getLine());

    return result;
}

void DatabaseCrossSectionSet::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCrossSectionSet, "*DATABASE_CROSS_SECTION_SET")

// ============================================================================
// DatabaseCrossSectionPlane
// ============================================================================

bool DatabaseCrossSectionPlane::parse(const std::vector<std::string>& lines,
                                       util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    size_t lineIdx = 0;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 1: PSID, XCT, YCT, ZCT, XCH, YCH, ZCH
    parser.setLine(lines[lineIdx]);
    size_t pos = 0;

    data_.psid = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.xct = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yct = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zct = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xch = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.ych = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zch = parser.getDoubleAt(pos, realW).value_or(0.0);
    ++lineIdx;

    // Skip comments
    while (lineIdx < lines.size() && util::CardParser::isCommentLine(lines[lineIdx])) {
        ++lineIdx;
    }
    if (lineIdx >= lines.size()) return true;

    // Card 2: RADIUS, XHEV, YHEV, ZHEV, LENL, LENM, ID, ITYPE
    parser.setLine(lines[lineIdx]);
    pos = 0;

    data_.radius = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.xhev = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.yhev = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.zhev = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenl = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.lenm = parser.getDoubleAt(pos, realW).value_or(0.0);
    pos += realW;
    data_.id = static_cast<int>(parser.getInt64At(pos).value_or(0));
    pos += intW;
    data_.itype = static_cast<int>(parser.getInt64At(pos).value_or(0));

    return true;
}

std::vector<std::string> DatabaseCrossSectionPlane::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    // Card 1
    writer.writeInt(data_.psid);
    writer.writeDouble(data_.xct);
    writer.writeDouble(data_.yct);
    writer.writeDouble(data_.zct);
    writer.writeDouble(data_.xch);
    writer.writeDouble(data_.ych);
    writer.writeDouble(data_.zch);
    result.push_back(writer.getLine());

    // Card 2
    writer.clear();
    writer.writeDouble(data_.radius);
    writer.writeDouble(data_.xhev);
    writer.writeDouble(data_.yhev);
    writer.writeDouble(data_.zhev);
    writer.writeDouble(data_.lenl);
    writer.writeDouble(data_.lenm);
    writer.writeInt(data_.id);
    writer.writeInt(data_.itype);
    result.push_back(writer.getLine());

    return result;
}

void DatabaseCrossSectionPlane::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCrossSectionPlane, "*DATABASE_CROSS_SECTION_PLANE")

// ============================================================================
// DatabaseFormat
// ============================================================================

bool DatabaseFormat::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.iform = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ibinary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseFormat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeInt(data_.iform);
    writer.writeInt(data_.ibinary);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseFormat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseFormat, "*DATABASE_FORMAT")

// ============================================================================
// DatabaseHistoryTshell
// ============================================================================

bool DatabaseHistoryTshell::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        while (pos + intW <= line.size()) {
            auto val = parser.getInt64At(pos);
            if (val && *val != 0) {
                data_.elemIds.push_back(*val);
            }
            pos += intW;
        }
    }

    return true;
}

std::vector<std::string> DatabaseHistoryTshell::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    for (size_t i = 0; i < data_.elemIds.size(); ++i) {
        writer.writeInt(data_.elemIds[i]);
        if ((i + 1) % 8 == 0) {
            result.push_back(writer.getLine());
            writer.clear();
        }
    }

    if (!writer.getLine().empty()) {
        result.push_back(writer.getLine());
    }

    return result;
}

void DatabaseHistoryTshell::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseHistoryTshell, "*DATABASE_HISTORY_TSHELL")

// ============================================================================
// DatabaseSbtout
// ============================================================================

bool DatabaseSbtout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSbtout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSbtout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSbtout, "*DATABASE_SBTOUT")

// ============================================================================
// DatabaseAtdout
// ============================================================================

bool DatabaseAtdout::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseAtdout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseAtdout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseAtdout, "*DATABASE_ATDOUT")

// ============================================================================
// DatabaseDisbout
// ============================================================================

bool DatabaseDisbout::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDisbout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDisbout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDisbout, "*DATABASE_DISBOUT")

// ============================================================================
// DatabaseDefgeo
// ============================================================================

bool DatabaseDefgeo::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDefgeo::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDefgeo::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDefgeo, "*DATABASE_DEFGEO")

// ============================================================================
// DatabasePrtube
// ============================================================================

bool DatabasePrtube::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabasePrtube::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabasePrtube::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabasePrtube, "*DATABASE_PRTUBE")

// ============================================================================
// DatabaseCpmfor
// ============================================================================

bool DatabaseCpmfor::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseCpmfor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseCpmfor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCpmfor, "*DATABASE_CPMFOR")

// ============================================================================
// DatabasePllyout
// ============================================================================

bool DatabasePllyout::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabasePllyout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabasePllyout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabasePllyout, "*DATABASE_PLLYOUT")

// ============================================================================
// DatabaseDemrcf
// ============================================================================

bool DatabaseDemrcf::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDemrcf::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDemrcf::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDemrcf, "*DATABASE_DEMRCF")

// ============================================================================
// DatabaseMovie
// ============================================================================

bool DatabaseMovie::parse(const std::vector<std::string>& lines,
                           util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseMovie::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseMovie::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseMovie, "*DATABASE_MOVIE")

// ============================================================================
// DatabaseFsi
// ============================================================================

bool DatabaseFsi::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseFsi::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseFsi::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseFsi, "*DATABASE_FSI")

// ============================================================================
// DatabaseMassout
// ============================================================================

bool DatabaseMassout::parse(const std::vector<std::string>& lines,
                             util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseMassout::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseMassout::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseMassout, "*DATABASE_MASSOUT")

// ============================================================================
// DatabaseTracer
// ============================================================================

bool DatabaseTracer::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.nid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.track = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ammgid = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseTracer::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.nid);
    writer.writeInt(data_.track);
    writer.writeInt(data_.ammgid);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseTracer::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseTracer, "*DATABASE_TRACER")

// ============================================================================
// DatabasePwpOutput
// ============================================================================

bool DatabasePwpOutput::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabasePwpOutput::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabasePwpOutput::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabasePwpOutput, "*DATABASE_PWP_OUTPUT")

// ============================================================================
// DatabaseFsiSensor
// ============================================================================

bool DatabaseFsiSensor::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseFsiSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseFsiSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseFsiSensor, "*DATABASE_FSI_SENSOR")

// ============================================================================
// DatabaseJntforcLocal
// ============================================================================

bool DatabaseJntforcLocal::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseJntforcLocal::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseJntforcLocal::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseJntforcLocal, "*DATABASE_JNTFORC_LOCAL")

// ============================================================================
// DatabaseBndoutVent
// ============================================================================

bool DatabaseBndoutVent::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseBndoutVent::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseBndoutVent::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseBndoutVent, "*DATABASE_BNDOUT_VENT")

// ============================================================================
// DatabaseCpmSensor
// ============================================================================

bool DatabaseCpmSensor::parse(const std::vector<std::string>& lines,
                               util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseCpmSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseCpmSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCpmSensor, "*DATABASE_CPM_SENSOR")

// ============================================================================
// DatabaseAleMat
// ============================================================================

bool DatabaseAleMat::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseAleMat::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseAleMat::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseAleMat, "*DATABASE_ALE_MAT")

// ============================================================================
// DatabaseNcforcFilter
// ============================================================================

bool DatabaseNcforcFilter::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseNcforcFilter::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseNcforcFilter::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseNcforcFilter, "*DATABASE_NCFORC_FILTER")

// ============================================================================
// DatabaseCurvoutExtend
// ============================================================================

bool DatabaseCurvoutExtend::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseCurvoutExtend::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseCurvoutExtend::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseCurvoutExtend, "*DATABASE_CURVOUT_EXTEND")

// ============================================================================
// DatabaseSbtoutRetractor
// ============================================================================

bool DatabaseSbtoutRetractor::parse(const std::vector<std::string>& lines,
                                     util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSbtoutRetractor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSbtoutRetractor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSbtoutRetractor, "*DATABASE_SBTOUT_RETRACTOR")

// ============================================================================
// DatabaseSbtoutSensor
// ============================================================================

bool DatabaseSbtoutSensor::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSbtoutSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSbtoutSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSbtoutSensor, "*DATABASE_SBTOUT_SENSOR")

// ============================================================================
// DatabasePllyoutRetractor
// ============================================================================

bool DatabasePllyoutRetractor::parse(const std::vector<std::string>& lines,
                                      util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabasePllyoutRetractor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabasePllyoutRetractor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabasePllyoutRetractor, "*DATABASE_PLLYOUT_RETRACTOR")

// ============================================================================
// DatabaseSphFlowSensor
// ============================================================================

bool DatabaseSphFlowSensor::parse(const std::vector<std::string>& lines,
                                   util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSphFlowSensor::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSphFlowSensor::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSphFlowSensor, "*DATABASE_SPH_FLOW_SENSOR")

// ============================================================================
// DatabaseDemassflow
// ============================================================================

bool DatabaseDemassflow::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseDemassflow::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseDemassflow::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseDemassflow, "*DATABASE_DEMASSFLOW")

// ============================================================================
// DatabaseTotgeo
// ============================================================================

bool DatabaseTotgeo::parse(const std::vector<std::string>& lines,
                            util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseTotgeo::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseTotgeo::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseTotgeo, "*DATABASE_TOTGEO")

// ============================================================================
// DatabasePsd
// ============================================================================

bool DatabasePsd::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabasePsd::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabasePsd::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabasePsd, "*DATABASE_PSD")

// ============================================================================
// DatabaseAbstatMass
// ============================================================================

bool DatabaseAbstatMass::parse(const std::vector<std::string>& lines,
                                util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseAbstatMass::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseAbstatMass::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseAbstatMass, "*DATABASE_ABSTAT_MASS")

// ============================================================================
// DatabaseSwforcFilter
// ============================================================================

bool DatabaseSwforcFilter::parse(const std::vector<std::string>& lines,
                                  util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseSwforcFilter::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseSwforcFilter::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseSwforcFilter, "*DATABASE_SWFORC_FILTER")

// ============================================================================
// DatabaseRve
// ============================================================================

bool DatabaseRve::parse(const std::vector<std::string>& lines,
                         util::CardParser::Format format) {
    if (lines.empty()) return true;

    util::CardParser parser(format);
    const size_t intW = parser.getIntWidth();
    const size_t realW = parser.getRealWidth();

    for (const auto& line : lines) {
        if (util::CardParser::isCommentLine(line)) continue;

        parser.setLine(line);
        size_t pos = 0;

        data_.dt = parser.getDoubleAt(pos, realW).value_or(0.0);
        pos += realW;
        data_.binary = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.lcur = static_cast<int>(parser.getInt64At(pos).value_or(0));
        pos += intW;
        data_.ioopt = static_cast<int>(parser.getInt64At(pos).value_or(0));
        break;
    }

    return true;
}

std::vector<std::string> DatabaseRve::write(util::CardParser::Format format) const {
    std::vector<std::string> result;
    util::CardWriter writer(format);

    writer.writeDouble(data_.dt);
    writer.writeInt(data_.binary);
    writer.writeInt(data_.lcur);
    writer.writeInt(data_.ioopt);

    result.push_back(writer.getLine());
    return result;
}

void DatabaseRve::accept(ModelVisitor& visitor) {
    visitor.visit(*this);
}

REGISTER_KEYWORD(DatabaseRve, "*DATABASE_RVE")

} // namespace koo::dyna
