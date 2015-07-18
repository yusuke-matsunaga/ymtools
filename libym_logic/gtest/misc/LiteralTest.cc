
/// @file LiteralTest.cc
/// @brief LiteralTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmLogic/Literal.h"
#include "YmUtils/CodecType.h"
#include "YmUtils/FileIDO.h"
#include "YmUtils/FileODO.h"


BEGIN_NAMESPACE_YM

TEST(LiteralTest, default_constructor)
{
  Literal litx;

  EXPECT_EQ( kLiteralX, litx );
}

TEST(LiteralTest, var_constructor)
{
  VarId v1(1);
  Literal lit1(v1);

  EXPECT_EQ( v1, lit1.varid() );
  EXPECT_TRUE( lit1.is_positive() );
  EXPECT_FALSE( lit1.is_negative() );
  EXPECT_EQ( 2, lit1.index() );

  Literal lit2 = ~lit1;
  EXPECT_EQ( v1, lit2.varid() );
  EXPECT_FALSE( lit2.is_positive() );
  EXPECT_TRUE( lit2.is_negative() );
  EXPECT_EQ( 3, lit2.index() );

  EXPECT_EQ( lit1, lit1.make_positive() );

  EXPECT_EQ( lit2, lit1.make_negative() );
}

TEST(LiteralTest, var_inv_constructor)
{
  VarId v1(1);
  Literal lit1(v1, true);

  EXPECT_EQ( v1, lit1.varid() );
  EXPECT_FALSE( lit1.is_positive() );
  EXPECT_TRUE( lit1.is_negative() );
  EXPECT_EQ( 3, lit1.index() );

  Literal lit2 = ~lit1;
  EXPECT_EQ( v1, lit2.varid() );
  EXPECT_TRUE( lit2.is_positive() );
  EXPECT_FALSE( lit2.is_negative() );
  EXPECT_EQ( 2, lit2.index() );

  EXPECT_EQ( lit2, lit1.make_positive() );

  EXPECT_EQ( lit1, lit1.make_negative() );
}

TEST(LiteralTest, compare)
{
  VarId v1(1);
  VarId v2(2);
  Literal lit1(v1);
  Literal lit2(v2, true);
  Literal lit3(v1, false);

  EXPECT_EQ( -1, compare( lit1, lit2 ) );
  EXPECT_EQ(  1, compare( lit2, lit1 ) );

  EXPECT_FALSE( lit1 == lit2 );
  EXPECT_TRUE ( lit1 != lit2 );
  EXPECT_TRUE ( lit1 <  lit2 );
  EXPECT_TRUE ( lit1 <= lit2 );
  EXPECT_FALSE( lit1 >  lit2 );
  EXPECT_FALSE( lit1 >= lit2 );

  EXPECT_EQ( 0, compare( lit1, lit3 ) );

  EXPECT_TRUE ( lit1 == lit3 );
  EXPECT_FALSE( lit1 != lit3 );
  EXPECT_FALSE( lit1 <  lit3 );
  EXPECT_TRUE ( lit1 <= lit3 );
  EXPECT_FALSE( lit1 >  lit3 );
  EXPECT_TRUE ( lit1 >= lit3 );
}

TEST(LiteralTest, store_restore)
{
  VarId v1(1);
  VarId v2(2);
  Literal lit1(v1);
  Literal lit2(v2, true);

  {
    FileODO ofd(kCodecThrough);
    ASSERT_TRUE( ofd.open("test.dat") ) << "Could not open file";

    lit1.store(ofd);
    lit2.store(ofd);
  }

  {
    FileIDO ifd(kCodecThrough);
    ASSERT_TRUE( ifd.open("test.dat") ) << "Could not open file";

    Literal r_lit1;
    Literal r_lit2;
    r_lit1.restore(ifd);
    r_lit2.restore(ifd);

    EXPECT_EQ( lit1, r_lit1 );
    EXPECT_EQ( lit2, r_lit2 );
  }
}

TEST(LiteralTest, store_restore2)
{
  VarId v1(1);
  VarId v2(2);
  Literal lit1(v1);
  Literal lit2(v2, true);

  {
    FileODO ofd(kCodecThrough);
    ASSERT_TRUE( ofd.open("test.dat") ) << "Could not open file";

    ofd << lit1 << lit2;
  }

  {
    FileIDO ifd(kCodecThrough);
    ASSERT_TRUE( ifd.open("test.dat") ) << "Could not open file";

    Literal r_lit1;
    Literal r_lit2;

    ifd >> r_lit1 >> r_lit2;

    EXPECT_EQ( lit1, r_lit1 );
    EXPECT_EQ( lit2, r_lit2 );
  }
}

END_NAMESPACE_YM
