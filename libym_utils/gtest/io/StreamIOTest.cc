
/// @file FileIOTest.cc
/// @brief FileIOTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmUtils/StreamIDO.h"
#include "YmUtils/StreamODO.h"
#include "YmUtils/MsgMgr.h"
#include "YmUtils/MsgHandler.h"


BEGIN_NAMESPACE_YM

class StreamIOTest :
  public testing::Test
{
public:

  // コンストラクタ
  StreamIOTest();

  // デストラクタ
  virtual
  ~StreamIOTest();


private:

  StreamMsgHandler mMH;

};

StreamIOTest::StreamIOTest() :
  mMH(&cout)
{
  MsgMgr::reg_handler(&mMH);
}

StreamIOTest::~StreamIOTest()
{
}

TEST_F(StreamIOTest, ODO_auto_close)
{
  // open() のみでデストラクタで自動的に close() されるかテスト
  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);
    ofd.write_32(0);
  }
}

TEST_F(StreamIOTest, rw_8)
{
  ymuint8 data_list[] = {
    0xFF, 0x00, 0xAA, 0x55
  };

  ymuint n = sizeof(data_list) / sizeof(ymuint8);

  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);

    for (ymuint i = 0; i < n; ++i) {
      ofd.write_8(data_list[i]);
    }
  }

  {
    ifstream ifs("test.dat");
    ASSERT_TRUE( ifs.good() ) << "Could not open file";
    StreamIDO ifd(ifs);

    for (ymuint i = 0; i < n; ++ i) {
      ymuint8 val1 = ifd.read_8();
      EXPECT_EQ( data_list[i], val1 ) << "data_list[" << i << "]";
    }
  }
}

TEST_F(StreamIOTest, rw_misc)
{
  const ymuint8  data1 = 0x01U;
  const ymuint16 data2 = 0xFEDCU;
  const ymuint32 data3 = 0xABCD9876UL;
  const ymuint64 data4 = 0x0123456789ABCDEFULL;

  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);

    ofd.write_8(data1);
    ofd.write_16(data2);
    ofd.write_32(data3);
    ofd.write_64(data4);
  }

  {
    ifstream ifs("test.dat");
    ASSERT_TRUE( ifs.good() ) << "Could not open file";
    StreamIDO ifd(ifs);

    ymuint8 val1 = ifd.read_8();
    EXPECT_EQ( data1, val1 );

    ymuint16 val2 = ifd.read_16();
    EXPECT_EQ( data2, val2 );

    ymuint32 val3 = ifd.read_32();
    EXPECT_EQ( data3, val3 );

    ymuint64 val4 = ifd.read_64();
    EXPECT_EQ( data4, val4 );

  }
}

TEST_F(StreamIOTest, rw_float)
{
  const float data_list[] = {
    0.0, 1.0, 1.23456789, 9.8765432e-30
  };

  ymuint n = sizeof(data_list) / sizeof(float);

  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);

    for (ymuint i = 0; i < n; ++ i) {
      ofd.write_float(data_list[i]);
    }
  }

  {
    ifstream ifs("test.dat");
    ASSERT_TRUE( ifs.good() ) << "Could not open file";
    StreamIDO ifd(ifs);

    for (ymuint i = 0; i < n; ++ i) {
      float val1 = ifd.read_float();
      EXPECT_EQ( data_list[i], val1 ) << "data_list[" << i << "]";
    }
  }
}

TEST_F(StreamIOTest, rw_double)
{
  const double data_list[] = {
    0.0, 1.0, 1.23456789, 9.8765432e-30
  };

  ymuint n = sizeof(data_list) / sizeof(double);

  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);

    for (ymuint i = 0; i < n; ++ i) {
      ofd.write_double(data_list[i]);
    }
  }

  {
    ifstream ifs("test.dat");
    ASSERT_TRUE( ifs.good() ) << "Could not open file";
    StreamIDO ifd(ifs);

    for (ymuint i = 0; i < n; ++ i) {
      double val1 = ifd.read_double();
      EXPECT_EQ( data_list[i], val1 ) << "data_list[" << i << "]";
    }
  }
}

TEST_F(StreamIOTest, rw_string)
{
  const char* data_list[] = {
    "",
    "abc",
    "a quick fox jumped over a lazy sheep"
  };

  ymuint n = sizeof(data_list) / sizeof(const char*);

  {
    ofstream ofs("test.dat");
    ASSERT_TRUE( ofs.good() ) << "Could not open file";
    StreamODO ofd(ofs);

    for (ymuint i = 0; i < n; ++ i) {
      ofd.write_str(data_list[i]);
    }
  }

  {
    ifstream ifs("test.dat");
    ASSERT_TRUE( ifs.good() ) << "Could not open file";
    StreamIDO ifd(ifs);

    for (ymuint i = 0; i < n; ++ i) {
      string val1 = ifd.read_str();
      EXPECT_EQ( data_list[i], val1 ) << "data_list[" << i << "]";
    }
  }
}

END_NAMESPACE_YM
