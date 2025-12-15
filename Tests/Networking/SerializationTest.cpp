#include <gtest/gtest.h>
#include "Networking/Serialization/Serialization.h"

TEST(WriteArchiveTest, DefaultConstructionProducesMinimalBytes)
{
    WriteArchive writer;
    auto bytes = writer.getBytes();

    EXPECT_TRUE(bytes.size() < 16);
}

TEST(SerializationRoundTripTest, SignedIntegers)
{
    int8_t i8 = -128;
    int16_t i16 = -32768;
    int32_t i32 = -2147483648;
    int64_t i64 = -9223372036854775807LL;

    WriteArchive writer;
    writer.process(i8);
    writer.process(i16);
    writer.process(i32);
    writer.process(i64);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    int8_t ri8 = 0;
    int16_t ri16 = 0;
    int32_t ri32 = 0;
    int64_t ri64 = 0;

    reader.process(ri8);
    reader.process(ri16);
    reader.process(ri32);
    reader.process(ri64);

    EXPECT_EQ(ri8, i8);
    EXPECT_EQ(ri16, i16);
    EXPECT_EQ(ri32, i32);
    EXPECT_EQ(ri64, i64);
}

TEST(SerializationRoundTripTest, UnsignedIntegers)
{
    uint8_t u8 = 255;
    uint16_t u16 = 65535;
    uint32_t u32 = 4294967295U;
    uint64_t u64 = 18446744073709551615ULL;

    WriteArchive writer;
    writer.process(u8);
    writer.process(u16);
    writer.process(u32);
    writer.process(u64);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    uint8_t ru8 = 0;
    uint16_t ru16 = 0;
    uint32_t ru32 = 0;
    uint64_t ru64 = 0;

    reader.process(ru8);
    reader.process(ru16);
    reader.process(ru32);
    reader.process(ru64);

    EXPECT_EQ(ru8, u8);
    EXPECT_EQ(ru16, u16);
    EXPECT_EQ(ru32, u32);
    EXPECT_EQ(ru64, u64);
}

TEST(SerializationRoundTripTest, FloatingPoint)
{
    float f = 3.14159f;
    double d = 2.718281828459045;

    WriteArchive writer;
    writer.process(f);
    writer.process(d);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    float rf = 0.0f;
    double rd = 0.0;

    reader.process(rf);
    reader.process(rd);

    EXPECT_FLOAT_EQ(rf, f);
    EXPECT_DOUBLE_EQ(rd, d);
}

TEST(SerializationRoundTripTest, FloatingPointEdgeCases)
{
    float zero = 0.0f;
    float negative = -123.456f;
    double tiny = 1e-300;
    double large = 1e300;

    WriteArchive writer;
    writer.process(zero);
    writer.process(negative);
    writer.process(tiny);
    writer.process(large);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    float rZero = 1.0f;
    float rNegative = 0.0f;
    double rTiny = 0.0;
    double rLarge = 0.0;

    reader.process(rZero);
    reader.process(rNegative);
    reader.process(rTiny);
    reader.process(rLarge);

    EXPECT_FLOAT_EQ(rZero, zero);
    EXPECT_FLOAT_EQ(rNegative, negative);
    EXPECT_DOUBLE_EQ(rTiny, tiny);
    EXPECT_DOUBLE_EQ(rLarge, large);
}

TEST(SerializationRoundTripTest, BoolValues)
{
    bool t = true;
    bool f = false;

    WriteArchive writer;
    writer.process(t);
    writer.process(f);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    bool rt = false;
    bool rf = true;

    reader.process(rt);
    reader.process(rf);

    EXPECT_EQ(rt, t);
    EXPECT_EQ(rf, f);
}

TEST(SerializationRoundTripTest, String)
{
    std::string str = "Hello, serialization!";

    WriteArchive writer;
    writer.process(str);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    std::string result;
    reader.process(result);

    EXPECT_EQ(result, str);
}

TEST(SerializationRoundTripTest, EmptyString)
{
    std::string empty;

    WriteArchive writer;
    writer.process(empty);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    std::string result = "not empty";
    reader.process(result);

    EXPECT_TRUE(result.empty());
}

TEST(SerializationRoundTripTest, LongString)
{
    std::string longStr(10000, 'x');

    WriteArchive writer;
    writer.process(longStr);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    std::string result;
    reader.process(result);

    EXPECT_EQ(result, longStr);
}

TEST(SerializationRoundTripTest, StringWithSpecialCharacters)
{
    std::string special = "null\0byte";
    std::string newlines = "line1\nline2\r\nline3";
    std::string unicode = "こんにちは世界";

    WriteArchive writer;
    writer.process(newlines);
    writer.process(unicode);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    std::string rNewlines;
    std::string rUnicode;

    reader.process(rNewlines);
    reader.process(rUnicode);

    EXPECT_EQ(rNewlines, newlines);
    EXPECT_EQ(rUnicode, unicode);
}

TEST(SerializationRoundTripTest, MixedTypes)
{
    int32_t id = 42;
    std::string name = "Player1";
    float health = 100.0f;
    bool alive = true;
    double score = 12345.6789;

    WriteArchive writer;
    writer.process(id);
    writer.process(name);
    writer.process(health);
    writer.process(alive);
    writer.process(score);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    int32_t rId = 0;
    std::string rName;
    float rHealth = 0.0f;
    bool rAlive = false;
    double rScore = 0.0;

    reader.process(rId);
    reader.process(rName);
    reader.process(rHealth);
    reader.process(rAlive);
    reader.process(rScore);

    EXPECT_EQ(rId, id);
    EXPECT_EQ(rName, name);
    EXPECT_FLOAT_EQ(rHealth, health);
    EXPECT_EQ(rAlive, alive);
    EXPECT_DOUBLE_EQ(rScore, score);
}

TEST(SerializationRoundTripTest, ZeroValues)
{
    int zero_int = 0;
    float zero_float = 0.0f;
    double zero_double = 0.0;

    WriteArchive writer;
    writer.process(zero_int);
    writer.process(zero_float);
    writer.process(zero_double);
    auto bytes = writer.getBytes();

    ReadArchive reader(bytes.data(), bytes.size());
    int ri = 999;
    float rf = 999.0f;
    double rd = 999.0;

    reader.process(ri);
    reader.process(rf);
    reader.process(rd);

    EXPECT_EQ(ri, 0);
    EXPECT_FLOAT_EQ(rf, 0.0f);
    EXPECT_DOUBLE_EQ(rd, 0.0);
}