
/// @file SatSolverTest.cc
/// @brief SatSolverTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "YmLogic/SatSolver.h"


BEGIN_NAMESPACE_YM

class SatSolverTest :
  public testing::TestWithParam<const char*>
{
public:

  // コンストラクタ
  SatSolverTest();


public:

  SatSolver mSolver;

};

SatSolverTest::SatSolverTest() :
  mSolver(GetParam())
{
}

TEST_P(SatSolverTest, new_var)
{
  VarId v1 = mSolver.new_var();

  EXPECT_EQ( 1, mSolver.variable_num() );
  EXPECT_EQ( 0, mSolver.clause_num() );
  EXPECT_EQ( 0, mSolver.literal_num() );

  EXPECT_EQ( 0, v1.val() );
}

TEST_P(SatSolverTest, add_clause1)
{
  VarId v1 = mSolver.new_var();
  Literal lit1(v1);
  mSolver.add_clause(lit1);

  vector<Bool3> model;
  Bool3 ans = mSolver.solve(model);

  EXPECT_EQ( kB3True,  ans );
  EXPECT_EQ( kB3True,  model[0] );
}

TEST_P(SatSolverTest, add_clause2)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  mSolver.add_clause(lit1, ~lit2);

  EXPECT_EQ( 1, mSolver.clause_num() );
  EXPECT_EQ( 2, mSolver.literal_num() );
}

TEST_P(SatSolverTest, add_clause3)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);
  mSolver.add_clause(lit1, ~lit2, lit3);

  EXPECT_EQ( 1, mSolver.clause_num() );
  EXPECT_EQ( 3, mSolver.literal_num() );
}

TEST_P(SatSolverTest, add_clause4)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  VarId v4 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);
  Literal lit4(v4);
  mSolver.add_clause(lit1, ~lit2, lit3, lit4);

  EXPECT_EQ( 1, mSolver.clause_num() );
  EXPECT_EQ( 4, mSolver.literal_num() );
}

TEST_P(SatSolverTest, add_clause5)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  VarId v4 = mSolver.new_var();
  VarId v5 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);
  Literal lit4(v4);
  Literal lit5(v5);
  mSolver.add_clause(lit1, ~lit2, lit3, lit4, ~lit5);

  EXPECT_EQ( 1, mSolver.clause_num() );
  EXPECT_EQ( 5, mSolver.literal_num() );
}

TEST_P(SatSolverTest, add_clauseN)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  VarId v4 = mSolver.new_var();
  VarId v5 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);
  Literal lit4(v4);
  Literal lit5(v5);
  vector<Literal> lits;
  lits.push_back(lit1);
  lits.push_back(lit2);
  lits.push_back(lit3);
  lits.push_back(lit4);
  lits.push_back(lit5);
  mSolver.add_clause(lits);

  EXPECT_EQ( 1, mSolver.clause_num() );
  EXPECT_EQ( 5, mSolver.literal_num() );
}

TEST_P(SatSolverTest, add_clauseN2)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  VarId v4 = mSolver.new_var();
  VarId v5 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);
  Literal lit4(v4);
  Literal lit5(v5);
  vector<Literal> lits;
  lits.push_back(lit1);
  lits.push_back(lit2);
  lits.push_back(lit3);
  lits.push_back(lit4);
  lits.push_back(lit5);
  mSolver.add_clause(lits);

  lits.clear();
  lits.push_back(~lit1);
  lits.push_back(~lit2);
  lits.push_back(~lit3);
  lits.push_back(~lit4);
  lits.push_back(~lit5);
  mSolver.add_clause(lits);

  EXPECT_EQ( 2, mSolver.clause_num() );
  EXPECT_EQ( 10, mSolver.literal_num() );
}

TEST_P(SatSolverTest, solve)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);

  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit2, ~lit3);
  mSolver.add_clause( lit1,  lit2,  lit3);

  mSolver.add_clause( lit1,  lit2);
  mSolver.add_clause( lit1,  lit3);

  vector<Bool3> model;
  Bool3 ans = mSolver.solve(model);

  EXPECT_EQ( kB3True,  ans );
  EXPECT_EQ( kB3True,  model[0] );
  EXPECT_EQ( kB3False, model[1] );
  EXPECT_EQ( kB3False, model[2] );
}

TEST_P(SatSolverTest, solve2)
{
  VarId v1 = mSolver.new_var();
  VarId v2 = mSolver.new_var();
  VarId v3 = mSolver.new_var();
  Literal lit1(v1);
  Literal lit2(v2);
  Literal lit3(v3);

  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit2, ~lit3);
  mSolver.add_clause( lit1,  lit2,  lit3);

  vector<Literal> assumption;
  assumption.push_back(lit1);

  vector<Bool3> model;
  Bool3 ans = mSolver.solve(assumption, model);

  EXPECT_EQ( kB3True,  ans );
  EXPECT_EQ( kB3True,  model[0] );
  EXPECT_EQ( kB3False, model[1] );
  EXPECT_EQ( kB3False, model[2] );
}

INSTANTIATE_TEST_CASE_P(AllSat, SatSolverTest, testing::Values("", "minisat", "minisat2", "glueminisat2"));

END_NAMESPACE_YM
