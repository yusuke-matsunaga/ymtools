
/// @file UnitAllocTest.cc
/// @brief UnitAlloc のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "CppUTest/TestHarness.h"

#include "ym_utils/UnitAlloc.h"


using namespace nsYm;

TEST_GROUP(UnitAllocTestGroup)
{
  #define UNIT_SIZE  560

  UnitAlloc* mAlloc;

  TEST_SETUP()
  {
    mAlloc = new UnitAlloc(UNIT_SIZE, 4096);
  }

  TEST_TEARDOWN()
  {
    delete mAlloc;
  }
};

// setup(), teardown() でメモリリークが発生していないことを確認するためのテスト
TEST(UnitAllocTestGroup, empty)
{
}

// メモリ領域の確保のテスト
TEST(UnitAllocTestGroup, simple_alloc)
{
  for (ymuint i = 0; i < 1000000; ++ i) {
    void* p = mAlloc->get_memory(UNIT_SIZE);

    CHECK( p != NULL );

    // ちなみに mAlloc->put_memory() を呼ばなくてもメモリリークは発生しない．

    mAlloc->put_memory(UNIT_SIZE, p);
  }
}
