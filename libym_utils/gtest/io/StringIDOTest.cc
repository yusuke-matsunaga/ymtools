
/// @file StringIDOTest.cc
/// @brief StringIDOTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmUtils/StringIDO.h"
#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


BEGIN_NAMESPACE_YM

class StringIDOTest :
  public testing::Test
{
public:

  // コンストラクタ
  StringIDOTest();

  // デストラクタ
  virtual
  ~StringIDOTest();


private:

  StreamMsgHandler mMH;

};

StringIDOTest::StringIDOTest() :
  mMH(&cout)
{
  MsgMgr::reg_handler(&mMH);
}

StringIDOTest::~StringIDOTest()
{
}

TEST_F(StringIDOTest, read_8)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      ymuint8 val = ido.read_8();
    }, AssertError);
}

TEST_F(StringIDOTest, read_16)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      ymuint16 val = ido.read_16();
    }, AssertError);
}

TEST_F(StringIDOTest, read_32)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      ymuint32 val = ido.read_32();
    }, AssertError);
}

TEST_F(StringIDOTest, read_64)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      ymuint64 val = ido.read_64();
    }, AssertError);
}

TEST_F(StringIDOTest, read_float)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      float val = ido.read_float();
    }, AssertError);
}

TEST_F(StringIDOTest, read_double)
{
  StringIDO ido("test string");

  EXPECT_THROW({
      double val = ido.read_double();
    }, AssertError);
}

TEST_F(StringIDOTest, read_string)
{
  const char* data_str = "a quick fox jumped over a lazy sheep";

  {
    StringIDO ido(data_str);

    string val1 = ido.read_str();
    EXPECT_EQ( data_str, val1 );
  }
}

END_NAMESPACE_YM
