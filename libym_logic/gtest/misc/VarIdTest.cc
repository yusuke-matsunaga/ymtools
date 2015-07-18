
/// @file VarIdTest.cc
/// @brief VarIdTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmLogic/VarId.h"
#include "YmUtils/FileIDO.h"
#include "YmUtils/FileODO.h"


BEGIN_NAMESPACE_YM

TEST(VarIdTest, default_constructor)
{
  VarId v1;

  EXPECT_EQ( kVarIdIllegal, v1 );
}

TEST(VarIdTest, ymuint_constructor)
{
  VarId v1(0);
  VarId v2(1);
  VarId v3(0);

  EXPECT_EQ( 0, v1.val() );
  EXPECT_EQ( 1, v2.val() );
  EXPECT_EQ( 0, v3.val() );

  EXPECT_FALSE( v1 == v2 );
  EXPECT_TRUE( v1 != v2 );
  EXPECT_TRUE( v1 == v3 );
  EXPECT_FALSE( v1 != v3 );
  EXPECT_EQ( -1, compare(v1, v2) );
  EXPECT_EQ(  1, compare(v2, v1) );
  EXPECT_EQ(  0, compare(v1, v3) );
  EXPECT_TRUE( v1 < v2 );
  EXPECT_TRUE( v1 <= v2 );
  EXPECT_FALSE( v1 > v2 );
  EXPECT_FALSE( v1 >= v2 );
  EXPECT_FALSE( v1 < v3 );
  EXPECT_TRUE( v1 <= v3 );
  EXPECT_FALSE( v1 > v3 );
  EXPECT_TRUE( v1 >= v3 );
}

TEST(VarIdTest, streamout)
{
  ostringstream buf;

  VarId v1(1);

  buf << v1;

  string str = buf.str();
  EXPECT_EQ( "1", str );
}

TEST(VarIdTest, store_restore)
{
  VarId v1(1);
  VarId v2(2);

  {
    FileODO ofd(kCodecThrough);
    ASSERT_TRUE( ofd.open("test.dat") ) << "Could not open file";

    v1.store(ofd);
    v2.store(ofd);
  }

  {
    FileIDO ifd(kCodecThrough);
    ASSERT_TRUE( ifd.open("test.dat") ) << "Could not open file";

    VarId r_v1;
    VarId r_v2;
    r_v1.restore(ifd);
    r_v2.restore(ifd);

    EXPECT_EQ( v1, r_v1 );
    EXPECT_EQ( v2, r_v2 );
  }
}

TEST(VarIdTest, store_restore2)
{
  VarId v1(1);
  VarId v2(2);

  {
    FileODO ofd(kCodecThrough);
    ASSERT_TRUE( ofd.open("test.dat") ) << "Could not open file";

    ofd << v1 << v2;
  }

  {
    FileIDO ifd(kCodecThrough);
    ASSERT_TRUE( ifd.open("test.dat") ) << "Could not open file";

    VarId r_v1;
    VarId r_v2;

    ifd >> r_v1 >> r_v2;

    EXPECT_EQ( v1, r_v1 );
    EXPECT_EQ( v2, r_v2 );
  }
}

END_NAMESPACE_YM
