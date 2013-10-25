#ifndef SMTVARIMPL_H
#define SMTVARIMPL_H

/// @file SmtVarImpl.h
/// @brief SmtVarImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym_logic/SmtVar.h"


BEGIN_NAMESPACE_YM_SMT

//////////////////////////////////////////////////////////////////////
/// @class SmtVarImpl SmtVarImpl.h "SmtVarImpl.h"
/// @brief SmtVar の実装クラス
//////////////////////////////////////////////////////////////////////
class SmtVarImpl :
  public SmtVar
{
  friend class SmtSolverImpl;

protected:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] sort 変数の型
  SmtVarImpl(ymuint id,
	     const SmtSort* sort);

  /// @brief デストラクタ
  virtual
  ~SmtVarImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 型を返す．
  virtual
  const SmtSort*
  sort() const;

  /// @brief global 変数の場合に true を返す．
  virtual
  bool
  is_global() const;

  /// @brief forall 変数の場合に true を返す．
  virtual
  bool
  is_forall() const;

  /// @brief exists 変数の場合に true を返す．
  virtual
  bool
  is_exists() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint32 mId;

  // 型
  const SmtSort* mSort;

};


//////////////////////////////////////////////////////////////////////
/// @class SmtGlobalVar SmtVarImpl.h "SmtVarImpl.h"
/// @brief グローバル変数を表すクラス
//////////////////////////////////////////////////////////////////////
class SmtGlobalVar :
  public SmtVarImpl
{
  friend class SmtSolverImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] sort 変数の型
  SmtGlobalVar(ymuint id,
	       const SmtSort* sort);

  /// @brief デストラクタ
  virtual
  ~SmtGlobalVar();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の種類を返す．
  virtual
  tSmtVar
  type() const;

  /// @brief global 変数の場合に true を返す．
  virtual
  bool
  is_global() const;

};


//////////////////////////////////////////////////////////////////////
/// @class SmtForallVar SmtVarImpl.h "SmtVarImpl.h"
/// @brief forall 変数を表すクラス
//////////////////////////////////////////////////////////////////////
class SmtForallVar :
  public SmtVarImpl
{
  friend class SmtSolverImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] sort 変数の型
  SmtForallVar(ymuint id,
	       const SmtSort* sort);

  /// @brief デストラクタ
  virtual
  ~SmtForallVar();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の種類を返す．
  virtual
  tSmtVar
  type() const;

  /// @brief forall 変数の場合に true を返す．
  virtual
  bool
  is_forall() const;

};


//////////////////////////////////////////////////////////////////////
/// @class SmtExistsVar SmtVarImpl.h "SmtVarImpl.h"
/// @brief forall 変数を表すクラス
//////////////////////////////////////////////////////////////////////
class SmtExistsVar :
  public SmtVarImpl
{
  friend class SmtSolverImpl;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] sort 変数の型
  SmtExistsVar(ymuint id,
	       const SmtSort* sort);

  /// @brief デストラクタ
  virtual
  ~SmtExistsVar();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の種類を返す．
  virtual
  tSmtVar
  type() const;

  /// @brief exists 変数の場合に true を返す．
  virtual
  bool
  is_exists() const;

};

END_NAMESPACE_YM_SMT

#endif // SMTVARIMPL_H
