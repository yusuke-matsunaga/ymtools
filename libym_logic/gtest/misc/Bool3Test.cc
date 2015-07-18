
/// @file Bool3Test.cc
/// @brief Bool3Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmLogic/Bool3.h"


BEGIN_NAMESPACE_YM

TEST(Bool3Test, inverse)
{
  EXPECT_EQ( kB3False, ~kB3True  );
  EXPECT_EQ( kB3True,  ~kB3False );
  EXPECT_EQ( kB3X,     ~kB3X     );
}

TEST(Bool3Test, xor_op)
{
  EXPECT_EQ( kB3False, kB3False ^ kB3False );
  EXPECT_EQ( kB3True,  kB3False ^ kB3True  );
  EXPECT_EQ( kB3X,     kB3False ^ kB3X     );
  EXPECT_EQ( kB3True,  kB3True  ^ kB3False );
  EXPECT_EQ( kB3False, kB3True  ^ kB3True  );
  EXPECT_EQ( kB3X,     kB3True  ^ kB3X     );
  EXPECT_EQ( kB3X,     kB3X     ^ kB3False );
  EXPECT_EQ( kB3X,     kB3X     ^ kB3True  );
  EXPECT_EQ( kB3X,     kB3X     ^ kB3X     );
}

TEST(Bool3Test, inplace_xor)
{
  {
    Bool3 b3(kB3False);
    EXPECT_EQ( kB3False, b3 ^= kB3False );
  }
  {
    Bool3 b3(kB3False);
    EXPECT_EQ( kB3True,  b3 ^= kB3True  );
  }
  {
    Bool3 b3(kB3False);
    EXPECT_EQ( kB3X,     b3 ^= kB3X      );
  }

  {
    Bool3 b3(kB3True);
    EXPECT_EQ( kB3True, b3 ^= kB3False );
  }
  {
    Bool3 b3(kB3True);
    EXPECT_EQ( kB3False,  b3 ^= kB3True  );
  }
  {
    Bool3 b3(kB3True);
    EXPECT_EQ( kB3X,     b3 ^= kB3X      );
  }

  {
    Bool3 b3(kB3X);
    EXPECT_EQ( kB3X,     b3 ^= kB3False );
  }
  {
    Bool3 b3(kB3X);
    EXPECT_EQ( kB3X,     b3 ^= kB3True  );
  }
  {
    Bool3 b3(kB3X);
    EXPECT_EQ( kB3X,     b3 ^= kB3X      );
  }
}

TEST(Bool3Test, b2B3)
{
  EXPECT_EQ( kB3False, b2B3(false) );
  EXPECT_EQ( kB3True,  b2B3(true)  );
}

TEST(Bool3Test, streamout)
{
  {
    ostringstream buf;

    buf << kB3False;

    EXPECT_EQ( "false", buf.str() );
  }

  {
    ostringstream buf;

    buf << kB3True;

    EXPECT_EQ( "true", buf.str() );
  }

  {
    ostringstream buf;

    buf << kB3X;

    EXPECT_EQ( "unknown", buf.str() );
  }
}

END_NAMESPACE_YM
