﻿
/// @file IdMgrTest.cc
/// @brief IdMgr のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "CppUTest/TestHarness.h"

#include "SmtId.h"
#include "IdMgr.h"
#include "YmUtils/SimpleAlloc.h"


using namespace nsYm;
using namespace nsYm::nsSmtLibV2;

TEST_GROUP(libym_logic_IdMgrTestGroup)
{
  SimpleAlloc* alloc;
  IdMgr* mIdMgr;

  TEST_SETUP() {
    alloc = new SimpleAlloc(4096);
    mIdMgr = new IdMgr(*alloc);
  }

  TEST_TEARDOWN()
  {
    delete mIdMgr;
    delete alloc;
    ShString::free_all_memory();
  }
};

// setup(), teardown() でメモリリークが発生していないことを確認するためのテスト
TEST(libym_logic_IdMgrTestGroup, empty)
{
}

// 単純な形式の識別子を作るテスト
TEST(libym_logic_IdMgrTestGroup, make_id_simple)
{
  // 単純な形式の識別子を作る．
  const SmtId* id1 = mIdMgr->make_id(ShString("a"));

  // これは成功するはず．
  CHECK( id1 != NULL );

  // 名前が等しいかチェック
  STRCMP_EQUAL( "a", static_cast<const char*>(id1->name()) );

  // ShString として等しいかチェック
  LONGS_EQUAL( ShString("a").id(), id1->name().id() );

  // インデックスのサイズをチェック
  LONGS_EQUAL( 0, id1->index_size() );

  // id_str() のチェック
  string s = id_str(id1);
  STRCMP_EQUAL( "a", s.c_str() );
}

// 単純な形式の識別子の同一性テスト
TEST(libym_logic_IdMgrTestGroup, make_id_identity)
{
  // 単純な形式の識別子を作る．
  const SmtId* id1 = mIdMgr->make_id(ShString("a"));

  // これは成功するはず．
  CHECK( id1 != NULL );

  // 同じ名前の識別子を作る．
  const SmtId* id2 = mIdMgr->make_id(ShString("a"));

  // これは成功するはず．
  CHECK( id2 != NULL );

  // id1 と id2 は同じもののはず．
  LONGS_EQUAL( id1->id(), id2->id() );
}

// インデックス付きの識別子を作るテストその1
TEST(libym_logic_IdMgrTestGroup, make_id_complex1)
{
  // インデックス付きの識別子を作る．
  const SmtId* id1 = mIdMgr->make_id(ShString("x"), vector<ymuint32>(1, 0));

  // これは成功するはず．
  CHECK( id1 != NULL );

  // 名前が等しいかチェック
  STRCMP_EQUAL( "x", static_cast<const char*>(id1->name()) );

  // ShString として等しいかチェック
  LONGS_EQUAL( ShString("x").id(), id1->name().id() );

  // インデックスのサイズをチェック
  LONGS_EQUAL( 1, id1->index_size() );

  // インデックスをチェック
  LONGS_EQUAL( 0, id1->index(0) );

  // id_str() のチェック
  string s= id_str(id1);
  STRCMP_EQUAL( "x[0]", s.c_str() );
}

// インデックス付きの識別子を作るテストその2
TEST(libym_logic_IdMgrTestGroup, make_id_complex2)
{
  // インデックス付きの識別子を作る．
  const ymuint index_size = 2;
  const ymuint index0 = 2;
  const ymuint index1 = 3;
  vector<ymuint32> index_list(index_size);
  index_list[0] = index0;
  index_list[1] = index1;
  const SmtId* id1 = mIdMgr->make_id(ShString("x"), index_list);

  // これは成功するはず．
  CHECK( id1 != NULL );

  // 名前が等しいかチェック
  STRCMP_EQUAL( "x", static_cast<const char*>(id1->name()) );

  // ShString として等しいかチェック
  LONGS_EQUAL( ShString("x").id(), id1->name().id() );

  // インデックスのサイズをチェック
  LONGS_EQUAL( index_size, id1->index_size() );

  // インデックスをチェック
  LONGS_EQUAL( index0, id1->index(0) );
  LONGS_EQUAL( index1, id1->index(1) );

  // id_str() のチェック
  string s = id_str(id1);
  STRCMP_EQUAL( "x[2][3]", s.c_str() );
}

// インデックス付きの識別子の同一性テスト
TEST(libym_logic_IdMgrTestGroup, make_id_identity2)
{
  // インデックス付きの識別子を作る．
  const ymuint index_size = 2;
  const ymuint index0 = 2;
  const ymuint index1 = 3;
  vector<ymuint32> index_list(index_size);
  index_list[0] = index0;
  index_list[1] = index1;
  const SmtId* id1 = mIdMgr->make_id(ShString("x"), index_list);

  // これは成功するはず．
  CHECK( id1 != NULL );

  vector<ymuint32> index_list2(index_size);
  index_list2[0] = index0;
  index_list2[1] = index1;
  const SmtId* id2 = mIdMgr->make_id(ShString("x"), index_list2);

  // これは成功するはず．
  CHECK( id2 != NULL );

  // id1 と id2 は同じもののはず．
  LONGS_EQUAL( id1->id(), id2->id() );

  // 別の識別子を作る．
  const SmtId* id3 = mIdMgr->make_id(ShString("x"));

  // これは成功するはず．
  CHECK( id3 != NULL );

  // id1 と id3 は別のもののはず
  CHECK( id1->id() != id3->id() );

  // もう一つ別の識別子を作る．
  vector<ymuint32> index_list3(2);
  index_list3[0] = 0;
  index_list3[1] = 1;
  const SmtId* id4 = mIdMgr->make_id(ShString("x"), index_list3);

  // これは成功するはず．
  CHECK( id4 != NULL );

  // id1 と id4 は別のもののはず
  CHECK( id1->id() != id4->id() );

}
