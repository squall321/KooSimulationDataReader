#include <gtest/gtest.h>
#include <koo/dyna/Material.hpp>

using namespace koo::dyna;

TEST(MatElasticTest, ParseAndWrite) {
    MatElastic mat;

    std::vector<std::string> lines = {
        "         1   7.85-9    2.1+5       0.3"
    };

    EXPECT_TRUE(mat.parse(lines));

    EXPECT_EQ(mat.getMaterialId(), 1);
    EXPECT_NEAR(mat.getData().ro, 7.85e-9, 1e-15);
    EXPECT_NEAR(mat.getData().e, 2.1e5, 1e-10);
    EXPECT_DOUBLE_EQ(mat.getData().pr, 0.3);

    auto written = mat.write();
    EXPECT_EQ(written.size(), 1);

    MatElastic parsed;
    EXPECT_TRUE(parsed.parse(written));
    EXPECT_EQ(parsed.getMaterialId(), 1);
}

TEST(MatElasticTest, DataAccess) {
    MatElastic mat;

    mat.setMaterialId(5);
    mat.getData().ro = 2700.0;
    mat.getData().e = 70000.0;
    mat.getData().pr = 0.33;

    EXPECT_EQ(mat.getMaterialId(), 5);
    EXPECT_DOUBLE_EQ(mat.getData().ro, 2700.0);
    EXPECT_DOUBLE_EQ(mat.getData().e, 70000.0);
    EXPECT_DOUBLE_EQ(mat.getData().pr, 0.33);
}

TEST(MatElasticTest, Clone) {
    MatElastic mat;
    mat.setMaterialId(1);
    mat.getData().e = 210000.0;

    auto cloned = mat.clone();
    auto* matClone = dynamic_cast<MatElastic*>(cloned.get());

    ASSERT_NE(matClone, nullptr);
    EXPECT_EQ(matClone->getMaterialId(), 1);
    EXPECT_DOUBLE_EQ(matClone->getData().e, 210000.0);
}

TEST(MatElasticTest, MaterialType) {
    MatElastic mat;
    EXPECT_EQ(mat.getMaterialType(), MaterialType::Elastic);
    EXPECT_EQ(mat.getKeywordName(), "*MAT_ELASTIC");
}

TEST(MatRigidTest, ParseAndWrite) {
    MatRigid mat;

    std::vector<std::string> lines = {
        "         1   7.85-9    2.1+5       0.3",
        "         1         4         7"
    };

    EXPECT_TRUE(mat.parse(lines));

    EXPECT_EQ(mat.getMaterialId(), 1);
    EXPECT_EQ(mat.getData().cmo, 1);
    EXPECT_EQ(mat.getData().con1, 4);
    EXPECT_EQ(mat.getData().con2, 7);

    auto written = mat.write();
    EXPECT_EQ(written.size(), 2);

    MatRigid parsed;
    EXPECT_TRUE(parsed.parse(written));
    EXPECT_EQ(parsed.getData().cmo, 1);
}

TEST(MatRigidTest, MaterialType) {
    MatRigid mat;
    EXPECT_EQ(mat.getMaterialType(), MaterialType::Rigid);
    EXPECT_EQ(mat.getKeywordName(), "*MAT_RIGID");
}

TEST(MatRigidTest, Clone) {
    MatRigid mat;
    mat.setMaterialId(2);
    mat.getData().cmo = 1;

    auto cloned = mat.clone();
    auto* matClone = dynamic_cast<MatRigid*>(cloned.get());

    ASSERT_NE(matClone, nullptr);
    EXPECT_EQ(matClone->getMaterialId(), 2);
    EXPECT_EQ(matClone->getData().cmo, 1);
}
