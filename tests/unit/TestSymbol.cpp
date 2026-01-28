#include <gtest/gtest.h>
#include <koo/ecad/Symbol.hpp>
#include <koo/ecad/Feature.hpp>

using namespace koo::ecad;

// ============================================================================
// Symbol Basic Tests
// ============================================================================

TEST(SymbolTest, CreateWithName) {
    Symbol symbol("round100");
    EXPECT_EQ(symbol.getName(), "round100");
}

TEST(SymbolTest, SetName) {
    Symbol symbol;
    symbol.setName("rect50x100");
    EXPECT_EQ(symbol.getName(), "rect50x100");
}

TEST(SymbolTest, IsStandard) {
    // Standard symbols are predefined (round, rect, oval, etc.)
    // Use parseStandardSymbol to detect standard symbols
    EXPECT_TRUE(Symbol::isStandardSymbolName("r100"));
    EXPECT_TRUE(Symbol::isStandardSymbolName("s50"));
    EXPECT_TRUE(Symbol::isStandardSymbolName("rect50x100"));

    Symbol userSymbol("my_custom_pad");
    EXPECT_FALSE(userSymbol.isStandard());
}

// ============================================================================
// Symbol Features Tests
// ============================================================================

TEST(SymbolTest, AddFeature) {
    Symbol symbol("custom");

    auto line = std::make_unique<LineFeature>();
    line->setStart(-5, -5);
    line->setEnd(5, 5);
    symbol.addFeature(std::move(line));

    EXPECT_EQ(symbol.getFeatures().size(), 1);
}

TEST(SymbolTest, AddMultipleFeatures) {
    Symbol symbol("complex_pad");

    symbol.addFeature(std::make_unique<LineFeature>());
    symbol.addFeature(std::make_unique<ArcFeature>());
    symbol.addFeature(std::make_unique<SurfaceFeature>());

    EXPECT_EQ(symbol.getFeatures().size(), 3);
}

TEST(SymbolTest, ClearFeatures) {
    Symbol symbol("test");

    symbol.addFeature(std::make_unique<LineFeature>());
    symbol.addFeature(std::make_unique<LineFeature>());

    symbol.clearFeatures();
    EXPECT_EQ(symbol.getFeatures().size(), 0);
}

// ============================================================================
// Symbol Type Tests
// ============================================================================

TEST(SymbolTest, SetType) {
    Symbol symbol("test");
    symbol.setType(SymbolType::Round);
    EXPECT_EQ(symbol.getType(), SymbolType::Round);

    symbol.setType(SymbolType::Rectangle);
    EXPECT_EQ(symbol.getType(), SymbolType::Rectangle);
}

// ============================================================================
// Symbol Dimension Tests
// ============================================================================

TEST(SymbolTest, SetDimensions) {
    Symbol symbol("rect");
    symbol.setPrimaryDimension(100.0);
    symbol.setSecondaryDimension(50.0);

    EXPECT_DOUBLE_EQ(symbol.getPrimaryDimension(), 100.0);
    EXPECT_DOUBLE_EQ(symbol.getSecondaryDimension(), 50.0);
    EXPECT_DOUBLE_EQ(symbol.getWidth(), 100.0);
    EXPECT_DOUBLE_EQ(symbol.getHeight(), 50.0);
}

// ============================================================================
// Symbol Attributes Tests
// ============================================================================

TEST(SymbolTest, SetAndGetAttribute) {
    Symbol symbol("custom");

    symbol.setAttribute("type", "smd");
    symbol.setAttribute("width", "2.5");

    auto& attrs = symbol.getAttributes();
    EXPECT_EQ(attrs.at("type"), "smd");
    EXPECT_EQ(attrs.at("width"), "2.5");
}

// ============================================================================
// SymbolLibrary Tests
// ============================================================================

TEST(SymbolLibraryTest, AddAndGetSymbol) {
    SymbolLibrary lib;

    auto sym = std::make_unique<Symbol>("round100");
    lib.addSymbol(std::move(sym));

    Symbol* found = lib.getSymbol("round100");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "round100");
}

TEST(SymbolLibraryTest, GetNonexistentSymbol) {
    SymbolLibrary lib;

    EXPECT_EQ(lib.getSymbol("nonexistent"), nullptr);
}

TEST(SymbolLibraryTest, GetSymbolCount) {
    SymbolLibrary lib;

    lib.addSymbol(std::make_unique<Symbol>("sym1"));
    lib.addSymbol(std::make_unique<Symbol>("sym2"));
    lib.addSymbol(std::make_unique<Symbol>("sym3"));

    EXPECT_EQ(lib.getSymbolCount(), 3);
}

TEST(SymbolLibraryTest, GetSymbolNames) {
    SymbolLibrary lib;

    lib.addSymbol(std::make_unique<Symbol>("round100"));
    lib.addSymbol(std::make_unique<Symbol>("rect50x100"));

    auto names = lib.getSymbolNames();
    EXPECT_EQ(names.size(), 2);

    // Check both names exist
    EXPECT_NE(std::find(names.begin(), names.end(), "round100"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "rect50x100"), names.end());
}

TEST(SymbolLibraryTest, HasSymbol) {
    SymbolLibrary lib;

    lib.addSymbol(std::make_unique<Symbol>("my_pad"));

    EXPECT_TRUE(lib.hasSymbol("my_pad"));
    EXPECT_FALSE(lib.hasSymbol("other_pad"));
}

TEST(SymbolLibraryTest, Clear) {
    SymbolLibrary lib;

    lib.addSymbol(std::make_unique<Symbol>("sym1"));
    lib.addSymbol(std::make_unique<Symbol>("sym2"));

    lib.clear();

    EXPECT_EQ(lib.getSymbolCount(), 0);
}

// ============================================================================
// Standard Symbol Helper Classes Tests
// ============================================================================

TEST(RoundSymbolTest, Create) {
    RoundSymbol symbol(100.0);
    EXPECT_DOUBLE_EQ(symbol.getDiameter(), 100.0);
    EXPECT_EQ(symbol.getType(), SymbolType::Round);
}

TEST(SquareSymbolTest, Create) {
    SquareSymbol symbol(50.0);
    EXPECT_DOUBLE_EQ(symbol.getSize(), 50.0);
    EXPECT_EQ(symbol.getType(), SymbolType::Square);
}

TEST(RectangleSymbolTest, Create) {
    RectangleSymbol symbol(100.0, 50.0);
    EXPECT_DOUBLE_EQ(symbol.getWidth(), 100.0);
    EXPECT_DOUBLE_EQ(symbol.getHeight(), 50.0);
    EXPECT_EQ(symbol.getType(), SymbolType::Rectangle);
}

TEST(OblongSymbolTest, Create) {
    OblongSymbol symbol(80.0, 160.0);
    EXPECT_DOUBLE_EQ(symbol.getWidth(), 80.0);
    EXPECT_DOUBLE_EQ(symbol.getHeight(), 160.0);
    EXPECT_EQ(symbol.getType(), SymbolType::Oblong);
}

TEST(RoundDonutSymbolTest, Create) {
    RoundDonutSymbol symbol(100.0, 50.0);
    EXPECT_DOUBLE_EQ(symbol.getOuterDiameter(), 100.0);
    EXPECT_DOUBLE_EQ(symbol.getInnerDiameter(), 50.0);
    EXPECT_EQ(symbol.getType(), SymbolType::RoundDonut);
}

TEST(ThermalSymbolTest, Create) {
    ThermalSymbol symbol(100.0, 60.0, 45.0, 4, 20.0);
    EXPECT_DOUBLE_EQ(symbol.getOuterDiameter(), 100.0);
    EXPECT_DOUBLE_EQ(symbol.getInnerDiameter(), 60.0);
    EXPECT_EQ(symbol.getSpokes(), 4);
}

// ============================================================================
// Symbol Clone Tests
// ============================================================================

TEST(SymbolTest, Clone) {
    Symbol original("test_symbol");
    original.setType(SymbolType::User);
    original.setAttribute("key", "value");

    auto line = std::make_unique<LineFeature>();
    line->setStart(0, 0);
    line->setEnd(10, 10);
    original.addFeature(std::move(line));

    auto cloned = original.clone();

    EXPECT_EQ(cloned->getName(), "test_symbol");
    EXPECT_FALSE(cloned->isStandard());
    EXPECT_EQ(cloned->getFeatures().size(), 1);
}
