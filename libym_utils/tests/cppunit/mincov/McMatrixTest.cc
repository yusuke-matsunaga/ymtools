﻿
/// @file MincovMatrixTest.cc
/// @brief MincovMatrixTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "McMatrixTest.h"


BEGIN_NAMESPACE_YM_MINCOV

//////////////////////////////////////////////////////////////////////
// クラス McMatrixTest
//////////////////////////////////////////////////////////////////////

// @brief setup 関数
void
McMatrixTest::setUp()
{
  mMatrix1 = NULL;
  mRowNum = 0;
  mColNum = 0;
  mVerMatrix.clear();
}

// @brief teardown 関数
void
McMatrixTest::tearDown()
{
  delete mMatrix1;
  mVerMatrix.clear();
}

// @brief 本物と検証用の配列を用意する．
// @param[in] row_num 行数
// @param[in] col_num 列数
void
McMatrixTest::new_matrix(ymuint row_num,
			 ymuint col_num)
{
  mMatrix1 = new McMatrix(row_num, col_num, NULL);
  mRowNum = row_num;
  mColNum = col_num;
  mVerMatrix.resize(mRowNum * mColNum, false);
}

// @brief McMatrix の内容を一次元ベクタに変換する．
void
McMatrixTest::conv_matrix(const McMatrix& matrix,
			  vector<bool>& bitvector)
{
  ymuint nr = matrix.row_size();
  ymuint nc = matrix.col_size();
  bitvector.clear();
  bitvector.resize(nr * nc, false);
  for (const McRowHead* rh = matrix.row_front();
       !matrix.is_row_end(rh); rh = rh->next()) {
    ymuint r = rh->pos();
    ymuint n = 0;
    const McCell* prev = NULL;
    for (const McCell* cell = rh->front();
	 !rh->is_end(cell); cell = cell->row_next(), ++ n) {
      CPPUNIT_ASSERT_EQUAL( r, cell->row_pos() );
      bitvector[r * nc + cell->col_pos()] = true;
      if ( prev != NULL ) {
	CPPUNIT_ASSERT_EQUAL( prev, cell->row_prev() );
      }
      prev = cell;
    }
    CPPUNIT_ASSERT_EQUAL( n, rh->num() );
  }

  vector<bool> bitvector2(nr * nc, false);
  for (const McColHead* ch = matrix.col_front();
       !matrix.is_col_end(ch); ch = ch->next()) {
    ymuint c = ch->pos();
    ymuint n = 0;
    const McCell* prev = NULL;
    for (const McCell* cell = ch->front();
	 !ch->is_end(cell); cell = cell->col_next(), ++ n) {
      CPPUNIT_ASSERT_EQUAL( c, cell->col_pos() );
      bitvector2[cell->row_pos() * nc + c] = true;
      if ( prev != NULL ) {
	CPPUNIT_ASSERT_EQUAL( prev, cell->col_prev() );
      }
      prev = cell;
    }
    CPPUNIT_ASSERT_EQUAL( n, ch->num() );
  }
  for (ymuint i = 0; i < nr * nc; ++ i) {
    CPPUNIT_ASSERT_EQUAL( bitvector[i], bitvector2[i] );
  }
}

// @brief 空の行列のテスト
void
McMatrixTest::test_empty_matrix()
{
  ymuint row_num = 10;
  ymuint col_num = 20;
  McMatrix* matrix = new McMatrix(row_num, col_num, NULL);
  CPPUNIT_ASSERT( matrix != NULL );
  CPPUNIT_ASSERT_EQUAL( row_num, matrix->row_size() );
  CPPUNIT_ASSERT_EQUAL( col_num, matrix->col_size() );

  vector<bool> bitvector;
  conv_matrix(*matrix, bitvector);
  for (ymuint i = 0; i < row_num * col_num; ++ i) {
    bool value = bitvector[i];
    CPPUNIT_ASSERT_EQUAL( false, value );
  }

  delete matrix;
}

// @brief clear のテスト
void
McMatrixTest::test_clear()
{
  ymuint row_num = 10;
  ymuint col_num = 10;
  McMatrix* matrix = new McMatrix(row_num, col_num, NULL);
  CPPUNIT_ASSERT( matrix != NULL );

  ymuint32 pos_pair[] = {
    0, 0,
    0, 3,
    1, 2,
    1, 1,
    2, 4,
    0, 5,
    4, 0,
    4, 4,
    3, 1,
    3, 3,
    5, 0,
    2, 1,
    4, 4
  };
  ymuint nd = sizeof(pos_pair) / (sizeof(ymuint32) * 2);
  for (ymuint i = 0; i < nd; ++ i) {
    ymuint r = pos_pair[i * 2 + 0];
    ymuint c = pos_pair[i * 2 + 1];
    matrix->insert_elem(r, c);
  }
  matrix->clear();
  vector<bool> bitvector;
  conv_matrix(*matrix, bitvector);
  for (ymuint i = 0; i < row_num * col_num; ++ i) {
    bool value = bitvector[i];
    CPPUNIT_ASSERT_EQUAL( false, value );
  }
}

// @brief 簡単な行列のテスト
void
McMatrixTest::test_simple_matrix()
{
  ymuint row_num = 10;
  ymuint col_num = 10;
  McMatrix* matrix = new McMatrix(row_num, col_num, NULL);
  CPPUNIT_ASSERT( matrix != NULL );
  vector<bool> ver_matrix(row_num * col_num, false);

  ymuint32 pos_pair[] = {
    0, 0,
    0, 3,
    1, 2,
    1, 1,
    2, 4,
    0, 5,
    4, 0,
    4, 4,
    3, 1,
    3, 3,
    5, 0,
    2, 1,
    4, 4
  };
  ymuint nd = sizeof(pos_pair) / (sizeof(ymuint32) * 2);
  for (ymuint i = 0; i < nd; ++ i) {
    ymuint r = pos_pair[i * 2 + 0];
    ymuint c = pos_pair[i * 2 + 1];
    matrix->insert_elem(r, c);
    ver_matrix[r * col_num + c] = true;
  }
  vector<bool> bitvector;
  conv_matrix(*matrix, bitvector);

  for (ymuint i = 0; i < row_num * col_num; ++ i) {
    CPPUNIT_ASSERT_EQUAL( bitvector[i], ver_matrix[i] );
  }

  delete matrix;
}

// @breif delete_row のテスト
void
McMatrixTest::test_delete_row()
{
  ymuint row_num = 10;
  ymuint col_num = 10;
  McMatrix* matrix = new McMatrix(row_num, col_num, NULL);
  CPPUNIT_ASSERT( matrix != NULL );
  vector<bool> ver_matrix(row_num * col_num, false);

  ymuint32 pos_pair[] = {
    0, 0,
    0, 3,
    1, 2,
    1, 1,
    2, 4,
    0, 5,
    4, 0,
    4, 4,
    3, 1,
    3, 3,
    5, 0,
    2, 1,
    4, 4
  };
  ymuint nd = sizeof(pos_pair) / (sizeof(ymuint32) * 2);
  for (ymuint i = 0; i < nd; ++ i) {
    ymuint r = pos_pair[i * 2 + 0];
    ymuint c = pos_pair[i * 2 + 1];
    matrix->insert_elem(r, c);
    ver_matrix[r * col_num + c] = true;
  }

  matrix->delete_row(1);
  matrix->delete_row(3);

  for (ymuint c = 0; c < col_num; ++ c) {
    ver_matrix[1 * col_num + c] = false;
    ver_matrix[3 * col_num + c] = false;
  }

  vector<bool> bitvector;
  conv_matrix(*matrix, bitvector);

  for (ymuint i = 0; i < row_num * col_num; ++ i) {
    CPPUNIT_ASSERT_EQUAL( bitvector[i], ver_matrix[i] );
  }

  delete matrix;
}

// @brief delete_col のテスト
void
McMatrixTest::test_delete_col()
{
  ymuint row_num = 10;
  ymuint col_num = 10;
  McMatrix* matrix = new McMatrix(row_num, col_num, NULL);
  CPPUNIT_ASSERT( matrix != NULL );
  vector<bool> ver_matrix(row_num * col_num, false);

  ymuint32 pos_pair[] = {
    0, 0,
    0, 3,
    1, 2,
    1, 1,
    2, 4,
    0, 5,
    4, 0,
    4, 4,
    3, 1,
    3, 3,
    5, 0,
    2, 1,
    4, 4
  };
  ymuint nd = sizeof(pos_pair) / (sizeof(ymuint32) * 2);
  for (ymuint i = 0; i < nd; ++ i) {
    ymuint r = pos_pair[i * 2 + 0];
    ymuint c = pos_pair[i * 2 + 1];
    matrix->insert_elem(r, c);
    ver_matrix[r * col_num + c] = true;
  }

  matrix->delete_col(2);
  matrix->delete_col(4);

  for (ymuint r = 0; r < row_num; ++ r) {
    ver_matrix[r * col_num + 2] = false;
    ver_matrix[r * col_num + 4] = false;
  }

  vector<bool> bitvector;
  conv_matrix(*matrix, bitvector);

  for (ymuint i = 0; i < row_num * col_num; ++ i) {
    CPPUNIT_ASSERT_EQUAL( bitvector[i], ver_matrix[i] );
  }

  delete matrix;
}

END_NAMESPACE_YM_MINCOV
