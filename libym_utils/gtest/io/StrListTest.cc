
/// @file StrListTest.cc
/// @brief StrListTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmUtils/StrListIDO.h"
#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


BEGIN_NAMESPACE_YM

class StrListIDOTest :
  public testing::Test
{
public:

  // コンストラクタ
  StrListIDOTest();

  // デストラクタ
  virtual
  ~StrListIDOTest();


private:

  StreamMsgHandler mMH;

};

StrListIDOTest::StrListIDOTest() :
  mMH(&cout)
{
  MsgMgr::reg_handler(&mMH);
}

StrListIDOTest::~StrListIDOTest()
{
}

TEST_F(StrListIDOTest, read_8)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      ymuint8 val = ido.read_8();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_16)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      ymuint16 val = ido.read_16();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_32)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      ymuint32 val = ido.read_32();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_64)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      ymuint64 val = ido.read_64();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_float)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      float val = ido.read_float();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_double)
{
  StrListIDO ido(vector<string>(0));

  EXPECT_THROW({
      double val = ido.read_double();
    }, AssertError);
}

TEST_F(StrListIDOTest, read_string)
{
  const char* data_list[] = {
    "",
    "abc",
    "a quick fox jumped over a lazy sheep"
  };

  ymuint n = sizeof(data_list) / sizeof(const char*);

  vector<string> str_list(n);
  for (ymuint i = 0; i < n; ++ i) {
    str_list[i] = data_list[i];
  }

  {
    StrListIDO ido(str_list);

    for (ymuint i = 0; i < n; ++ i) {
      string val1 = ido.read_str();
      EXPECT_EQ( data_list[i], val1 ) << "data_list[" << i << "]";
    }
  }
}

END_NAMESPACE_YM
