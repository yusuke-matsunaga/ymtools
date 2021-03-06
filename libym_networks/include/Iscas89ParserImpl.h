﻿#ifndef ISCAS89PARSERIMPL_H
#define ISCAS89PARSERIMPL_H

/// @file Iscas89ParserImpl.h
/// @brief Iscas89ParserImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "iscas89_nsdef.h"
#include "Iscas89IdHash.h"


BEGIN_NAMESPACE_YM_ISCAS89

class Iscas89Handler;
class Iscas89Scanner;

//////////////////////////////////////////////////////////////////////
/// @class Iscas89ParserImpl Iscas89ParserImpl.h "Iscas89ParserImpl.h"
/// @brief Iscas89Parser の実装クラス
//////////////////////////////////////////////////////////////////////
class Iscas89ParserImpl
{
public:

  /// @brief コンストラクタ
  Iscas89ParserImpl();

  /// @brief デストラクタ
  ~Iscas89ParserImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @param[in] filename ファイル名
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(const string& filename);

  /// @brief イベントハンドラの登録
  void
  add_handler(Iscas89Handler* handler);

  /// @brief ID 番号から文字列を得る．
  const char*
  id2str(ymuint id) const;

  /// @brief ID 番号から位置情報を得る．
  FileRegion
  id2loc(ymuint id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲート型を読み込む．
  /// @param[out] gate_type ゲート型を格納する変数．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_gate_type(tIscas89GateType& gate_type);

  /// @brief '(' ')' で囲まれた名前を読み込む．
  /// @param[in] name_id 名前の識別子番号を格納する変数．
  /// @param[in] last_loc 末尾のファイル位置
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name(ymuint& name_id,
	     FileRegion& last_loc);

  /// @brief '(' ')' で囲まれた名前のリストを読み込む．
  /// @param[in] name_id_list 名前の識別子番号を格納するリスト．
  /// @param[in] last_loc 末尾のファイル位置
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  ///
  /// エラーが起きたらエラーメッセージをセットする．
  bool
  parse_name_list(vector<ymuint>& name_id_list,
		  FileRegion& last_loc);

  /// @brief INPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 入力ピン名の ID 番号
  /// @return エラーが起きたら false を返す．
  bool
  read_input(const FileRegion& loc,
	     ymuint name_id);

  /// @brief OUTPUT 文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] name_id 出力ピン名の ID 番号
  /// @return エラーが起きたら false を返す．
  bool
  read_output(const FileRegion& loc,
	      ymuint name_id);

  /// @brief ゲート文を読み込む．
  /// @param[in] loc ファイル位置
  /// @param[in] oname_id 出力名の ID 番号
  /// @param[in] type ゲートタイプ
  /// @return エラーが起きたら false を返す．
  /// @note 入力名のリストは push_str() で積まれている．
  bool
  read_gate(const FileRegion& loc,
	    ymuint oname_id,
	    tIscas89GateType type,
	    const vector<ymuint>& iname_id_list);

  /// @brief 次のトークンが期待されている型か調べる．
  /// @param[in] exp_token トークンの期待値
  /// @param[out] lval トークンの値を格納する変数
  /// @param[out] loc トークンのファイル位置を格納する変数．
  /// @retval true トークンの型が一致した．
  /// @retval false トークンの方が一致しなかった．
  ///
  /// トークンの方が一致しなかった場合にはエラーメッセージをセットする．
  bool
  expect(Token exp_token,
	 ymuint& lval,
	 FileRegion& loc);

  /// @brief トークンを一つ読みだす．
  /// @param[out] lval トークンの値を格納する変数
  /// @param[out] lloc トークンの位置を格納する変数
  /// @return トークンの型を返す．
  ///
  /// lval に値が入るのはトークンが kToken_NAME の時だけ
  Token
  read_token(ymuint& lval,
	     FileRegion& lloc);

  /// @brief ID 番号から IdCell を得る．
  IdCell*
  id2cell(ymuint id) const;

  /// @brief 文字列用の領域を確保する．
  /// @param[in] src_str ソース文字列
  /// @param[in] loc 文字列の位置情報
  /// @return 文字列の ID 番号
  ymuint
  reg_str(const char* src_str,
	  const FileRegion& loc);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 字句解析器
  Iscas89Scanner* mScanner;

  // イベントハンドラのリスト
  list<Iscas89Handler*> mHandlerList;

  // 文字列のハッシュ
  Iscas89IdHash mIdHash;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID 番号から IdCell を得る．
inline
IdCell*
Iscas89ParserImpl::id2cell(ymuint id) const
{
  return mIdHash.cell(id);
}

// @brief ID 番号から文字列を得る．
inline
const char*
Iscas89ParserImpl::id2str(ymuint id) const
{
  return mIdHash.str(id);
}

// @brief ID 番号から位置情報を得る．
inline
FileRegion
Iscas89ParserImpl::id2loc(ymuint id) const
{
  return mIdHash.loc(id);
}

END_NAMESPACE_YM_ISCAS89

#endif // ISCAS89PARSERIMPL_H
