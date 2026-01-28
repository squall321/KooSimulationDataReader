#include <gtest/gtest.h>
#include <koo/ecad/EdaData.hpp>

using namespace koo::ecad;

// ============================================================================
// Package Tests
// ============================================================================

TEST(PackageTest, CreateAndGetName) {
    Package pkg("BGA256");
    EXPECT_EQ(pkg.getName(), "BGA256");
}

TEST(PackageTest, SetPitch) {
    Package pkg("QFP100");
    pkg.setPitch(0.5);
    EXPECT_DOUBLE_EQ(pkg.getPitch(), 0.5);
}

TEST(PackageTest, SetBoundingBox) {
    Package pkg("SOT23");
    BoundingBox2D box;
    box.min = {-1.0, -1.5};
    box.max = {1.0, 1.5};
    pkg.setBoundingBox(box);

    auto bbox = pkg.getBoundingBox();
    EXPECT_DOUBLE_EQ(bbox.min.x, -1.0);
    EXPECT_DOUBLE_EQ(bbox.min.y, -1.5);
    EXPECT_DOUBLE_EQ(bbox.max.x, 1.0);
    EXPECT_DOUBLE_EQ(bbox.max.y, 1.5);
}

TEST(PackageTest, AddAndGetPins) {
    Package pkg("DIP8");

    Pin pin1;
    pin1.name = "1";
    pin1.x = -1.27;
    pin1.y = -3.81;
    pin1.type = PinType::ThroughHole;
    pkg.addPin(pin1);

    Pin pin2;
    pin2.name = "2";
    pin2.x = -1.27;
    pin2.y = -1.27;
    pin2.type = PinType::ThroughHole;
    pkg.addPin(pin2);

    EXPECT_EQ(pkg.getPinCount(), 2);

    const Pin* found = pkg.getPin("1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "1");
    EXPECT_DOUBLE_EQ(found->x, -1.27);

    EXPECT_EQ(pkg.getPin("nonexistent"), nullptr);
}

TEST(PackageTest, AddOutline) {
    Package pkg("QFN16");

    // Create outline using Contour with segments
    Contour outline(-1.0, -1.0, PolygonType::Island);
    outline.addLineSegment(1.0, -1.0);
    outline.addLineSegment(1.0, 1.0);
    outline.addLineSegment(-1.0, 1.0);
    outline.addLineSegment(-1.0, -1.0);  // close
    pkg.addOutline(outline);

    EXPECT_EQ(pkg.getOutlines().size(), 1);
    EXPECT_EQ(pkg.getOutlines()[0].getSegments().size(), 4);
}

TEST(PackageTest, Attributes) {
    Package pkg("BGA");
    pkg.setAttribute("height", "1.2");
    pkg.setAttribute("pitch", "0.8");

    auto& attrs = pkg.getAttributes();
    EXPECT_EQ(attrs.at("height"), "1.2");
    EXPECT_EQ(attrs.at("pitch"), "0.8");
}

// ============================================================================
// Component Tests
// ============================================================================

TEST(ComponentTest, CreateAndGetRefDes) {
    Component comp("U1");
    EXPECT_EQ(comp.getRefDes(), "U1");
}

TEST(ComponentTest, SetPosition) {
    Component comp("R1");
    comp.setPosition(10.5, 20.3);

    auto pos = comp.getPosition();
    EXPECT_DOUBLE_EQ(pos.x, 10.5);
    EXPECT_DOUBLE_EQ(pos.y, 20.3);
}

TEST(ComponentTest, SetRotationAndMirror) {
    Component comp("C1");
    comp.setRotation(90.0);
    comp.setMirrored(true);

    EXPECT_DOUBLE_EQ(comp.getRotation(), 90.0);
    EXPECT_TRUE(comp.isMirrored());
}

TEST(ComponentTest, SetSide) {
    Component comp("U2");
    comp.setSide(MountSide::Bottom);
    EXPECT_EQ(comp.getSide(), MountSide::Bottom);
}

TEST(ComponentTest, SetPartNumber) {
    Component comp("U1");
    comp.setPartNumber("LM358");
    comp.setPackageName("SOIC8");

    EXPECT_EQ(comp.getPartNumber(), "LM358");
    EXPECT_EQ(comp.getPackageName(), "SOIC8");
}

TEST(ComponentTest, BomProperties) {
    Component comp("R1");
    comp.setValue("10K");
    comp.setDescription("1% resistor");
    comp.setManufacturer("Yageo");
    comp.setManufacturerPartNumber("RC0603FR-0710KL");

    EXPECT_EQ(comp.getValue(), "10K");
    EXPECT_EQ(comp.getDescription(), "1% resistor");
    EXPECT_EQ(comp.getManufacturer(), "Yageo");
    EXPECT_EQ(comp.getManufacturerPartNumber(), "RC0603FR-0710KL");
}

TEST(ComponentTest, AddAndGetPins) {
    Component comp("U1");

    Pin pin1;
    pin1.name = "VCC";
    pin1.netName = "VDD";
    pin1.x = 0.0;
    pin1.y = 1.27;
    comp.addPin(pin1);

    Pin pin2;
    pin2.name = "GND";
    pin2.netName = "VSS";
    pin2.x = 0.0;
    pin2.y = -1.27;
    comp.addPin(pin2);

    EXPECT_EQ(comp.getPinCount(), 2);

    const Pin* found = comp.getPin("VCC");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->netName, "VDD");
}

TEST(ComponentTest, Attributes) {
    Component comp("J1");
    comp.setAttribute("category", "connector");
    comp.setAttribute("pins", "40");

    EXPECT_EQ(comp.getAttribute("category"), "connector");
    EXPECT_EQ(comp.getAttribute("pins"), "40");
    EXPECT_EQ(comp.getAttribute("nonexistent"), "");
}

// ============================================================================
// Net Tests
// ============================================================================

TEST(NetTest, CreateAndGetName) {
    Net net("VCC");
    EXPECT_EQ(net.getName(), "VCC");
}

TEST(NetTest, SetNetNumber) {
    Net net("GND");
    net.setNetNumber(0);
    EXPECT_EQ(net.getNetNumber(), 0);
}

TEST(NetTest, AddPins) {
    Net net("SIG1");
    net.addPin("U1", "1");
    net.addPin("R1", "2");

    PinRef ref;
    ref.refDes = "C1";
    ref.pinName = "1";
    net.addPin(ref);

    EXPECT_EQ(net.getPinCount(), 3);

    const auto& pins = net.getPins();
    EXPECT_EQ(pins[0].refDes, "U1");
    EXPECT_EQ(pins[0].pinName, "1");
    EXPECT_EQ(pins[2].refDes, "C1");
}

TEST(NetTest, NetClass) {
    Net net1("VCC");
    net1.setNetClass(NetClass::Power);
    EXPECT_EQ(net1.getNetClass(), NetClass::Power);
    EXPECT_TRUE(net1.isPower());
    EXPECT_FALSE(net1.isGround());

    Net net2("GND");
    net2.setNetClass(NetClass::Ground);
    EXPECT_TRUE(net2.isGround());

    Net net3("CLK");
    net3.setNetClass(NetClass::Signal);
    EXPECT_TRUE(net3.isSignal());
}

TEST(NetTest, Attributes) {
    Net net("DIFF_P");
    net.setAttribute("impedance", "50");
    net.setAttribute("class", "differential");

    auto& attrs = net.getAttributes();
    EXPECT_EQ(attrs.at("impedance"), "50");
}

TEST(NetTest, AddSubnet) {
    Net net("SIG");

    Net::Subnet subnet1;
    subnet1.type = Net::Subnet::Type::Trace;
    subnet1.featureIds = {1, 2, 3};
    net.addSubnet(subnet1);

    Net::Subnet subnet2;
    subnet2.type = Net::Subnet::Type::Via;
    subnet2.featureIds = {4};
    net.addSubnet(subnet2);

    EXPECT_EQ(net.getSubnets().size(), 2);
    EXPECT_EQ(net.getSubnets()[0].type, Net::Subnet::Type::Trace);
    EXPECT_EQ(net.getSubnets()[1].type, Net::Subnet::Type::Via);
}

// ============================================================================
// EdaData Container Tests
// ============================================================================

TEST(EdaDataTest, AddAndGetComponent) {
    EdaData eda;

    auto comp = std::make_unique<Component>("U1");
    comp->setPartNumber("MCU");
    eda.addComponent(std::move(comp));

    EXPECT_EQ(eda.getComponentCount(), 1);

    Component* found = eda.getComponent("U1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getPartNumber(), "MCU");

    EXPECT_EQ(eda.getComponent("U2"), nullptr);
}

TEST(EdaDataTest, AddAndGetNet) {
    EdaData eda;

    auto net = std::make_unique<Net>("VCC");
    net->setNetClass(NetClass::Power);
    eda.addNet(std::move(net));

    EXPECT_EQ(eda.getNetCount(), 1);

    Net* found = eda.getNet("VCC");
    ASSERT_NE(found, nullptr);
    EXPECT_TRUE(found->isPower());
}

TEST(EdaDataTest, AddAndGetPackage) {
    EdaData eda;

    auto pkg1 = std::make_unique<Package>("SOIC8");
    eda.addPackage(std::move(pkg1));

    auto pkg2 = std::make_unique<Package>("QFP100");
    eda.addPackage(std::move(pkg2));

    EXPECT_EQ(eda.getPackageCount(), 2);

    Package* found = eda.getPackage("SOIC8");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "SOIC8");

    // Get by index
    Package* byIdx = eda.getPackage(1);
    ASSERT_NE(byIdx, nullptr);
    EXPECT_EQ(byIdx->getName(), "QFP100");
}

TEST(EdaDataTest, GetComponentRefDes) {
    EdaData eda;

    eda.addComponent(std::make_unique<Component>("U1"));
    eda.addComponent(std::make_unique<Component>("R1"));
    eda.addComponent(std::make_unique<Component>("C1"));

    auto refDes = eda.getComponentRefDes();
    EXPECT_EQ(refDes.size(), 3);

    // Check all are present (order may vary)
    EXPECT_NE(std::find(refDes.begin(), refDes.end(), "U1"), refDes.end());
    EXPECT_NE(std::find(refDes.begin(), refDes.end(), "R1"), refDes.end());
    EXPECT_NE(std::find(refDes.begin(), refDes.end(), "C1"), refDes.end());
}

TEST(EdaDataTest, GetNetNames) {
    EdaData eda;

    eda.addNet(std::make_unique<Net>("VCC"));
    eda.addNet(std::make_unique<Net>("GND"));
    eda.addNet(std::make_unique<Net>("CLK"));

    auto netNames = eda.getNetNames();
    EXPECT_EQ(netNames.size(), 3);
}

TEST(EdaDataTest, GetPackageNames) {
    EdaData eda;

    eda.addPackage(std::make_unique<Package>("SOIC8"));
    eda.addPackage(std::make_unique<Package>("QFP44"));

    auto pkgNames = eda.getPackageNames();
    EXPECT_EQ(pkgNames.size(), 2);
}

TEST(EdaDataTest, GetComponentsOnSide) {
    EdaData eda;

    auto comp1 = std::make_unique<Component>("U1");
    comp1->setSide(MountSide::Top);
    eda.addComponent(std::move(comp1));

    auto comp2 = std::make_unique<Component>("U2");
    comp2->setSide(MountSide::Bottom);
    eda.addComponent(std::move(comp2));

    auto comp3 = std::make_unique<Component>("R1");
    comp3->setSide(MountSide::Top);
    eda.addComponent(std::move(comp3));

    auto topComps = eda.getComponentsOnSide(MountSide::Top);
    EXPECT_EQ(topComps.size(), 2);

    auto botComps = eda.getComponentsOnSide(MountSide::Bottom);
    EXPECT_EQ(botComps.size(), 1);
}

TEST(EdaDataTest, LayerNames) {
    EdaData eda;

    eda.addLayerName("top");
    eda.addLayerName("inner1");
    eda.addLayerName("inner2");
    eda.addLayerName("bottom");

    EXPECT_EQ(eda.getLayerNames().size(), 4);
    EXPECT_EQ(eda.getLayerNames()[0], "top");
    EXPECT_EQ(eda.getLayerNames()[3], "bottom");
}

TEST(EdaDataTest, BomItems) {
    EdaData eda;

    BomItem item1;
    item1.refDes = "R1";
    item1.partNumber = "RC0603";
    eda.addBomItem(item1);

    BomItem item2;
    item2.refDes = "C1";
    item2.partNumber = "CC0402";
    eda.addBomItem(item2);

    EXPECT_EQ(eda.getBomItemCount(), 2);

    const BomItem* found = eda.getBomItem("R1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->partNumber, "RC0603");
}

TEST(EdaDataTest, FeatureGroups) {
    EdaData eda;

    EdaData::FeatureGroup group;
    group.type = "via_in_pad";

    FeatureId fid1;
    fid1.type = 'C';
    fid1.layerNum = 1;
    fid1.featureNum = 100;
    group.features.push_back(fid1);

    FeatureId fid2;
    fid2.type = 'C';
    fid2.layerNum = 1;
    fid2.featureNum = 101;
    group.features.push_back(fid2);

    group.attributes["count"] = "2";
    eda.addFeatureGroup(group);

    EXPECT_EQ(eda.getFeatureGroups().size(), 1);
    EXPECT_EQ(eda.getFeatureGroups()[0].type, "via_in_pad");
    EXPECT_EQ(eda.getFeatureGroups()[0].features.size(), 2);
}

TEST(EdaDataTest, FeatureIdRecords) {
    EdaData eda;

    EdaData::FeatureIdRecord record;
    record.featureId.type = 'C';
    record.featureId.layerNum = 1;
    record.featureId.featureNum = 42;
    record.netNum = 5;
    record.subnetNum = 2;
    eda.addFeatureIdRecord(record);

    FeatureId queryFid;
    queryFid.type = 'C';
    queryFid.layerNum = 1;
    queryFid.featureNum = 42;

    auto [netNum, subnetNum] = eda.getFeatureNetSubnet(queryFid);
    EXPECT_EQ(netNum, 5);
    EXPECT_EQ(subnetNum, 2);
}

// ============================================================================
// PinRef Tests
// ============================================================================

TEST(PinRefTest, Equality) {
    PinRef ref1{"U1", "1"};
    PinRef ref2{"U1", "1"};
    PinRef ref3{"U1", "2"};
    PinRef ref4{"U2", "1"};

    EXPECT_TRUE(ref1 == ref2);
    EXPECT_FALSE(ref1 == ref3);
    EXPECT_FALSE(ref1 == ref4);
}
