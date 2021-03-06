﻿
/// @file EiDeclArray.cc
/// @brief EiDeclArray の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "EiFactory.h"
#include "EiDeclArray.h"
#include "ElbDelay.h"
#include "ElbExpr.h"

#include "YmVerilog/pt/PtDecl.h"
#include "YmVerilog/pt/PtItem.h"
#include "YmVerilog/pt/PtMisc.h"


BEGIN_NAMESPACE_YM_VERILOG

//////////////////////////////////////////////////////////////////////
// EiFactory の生成関数
//////////////////////////////////////////////////////////////////////

// @brief 宣言要素の配列を生成する．
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] range_src 範囲の配列
ElbDeclArray*
EiFactory::new_DeclArray(ElbDeclHead* head,
			 const PtNamedBase* pt_item,
			 const vector<ElbRangeSrc>& range_src)
{
  ymuint dim_size = range_src.size();
  void* q = mAlloc.get_memory(sizeof(EiRange) * dim_size);
  EiRange* range_array = new (q) EiRange[dim_size];
  ymuint elem_size = 1;
  for (ymuint i = 0; i < dim_size; ++ i) {
    range_array[i].set(range_src[i]);
    elem_size *= range_array[i].size();
  }

  EiDeclArray* decl = nullptr;
  void* p = nullptr;
  void* r = nullptr;
  switch ( head->type() ) {
  case kVpiReg:
  case kVpiNet:
    if ( head->bit_size() == 1 ) {
      r = mAlloc.get_memory(sizeof(VlScalarVal) * elem_size);
      VlScalarVal* varray = new (r) VlScalarVal[elem_size];
      p = mAlloc.get_memory(sizeof(EiDeclArrayS));
      decl = new (p) EiDeclArrayS(head, pt_item, dim_size, range_array,
				  varray);
      break;
    }
    // わざと次に続く．

  case kVpiIntegerVar:
  case kVpiTimeVar:
    {
      r = mAlloc.get_memory(sizeof(BitVector) * elem_size);
      BitVector* varray = new (r) BitVector[elem_size];
      p = mAlloc.get_memory(sizeof(EiDeclArrayV));
      decl = new (p) EiDeclArrayV(head, pt_item, dim_size, range_array,
				  varray);
    }
    break;

  case kVpiRealVar:
    {
      r = mAlloc.get_memory(sizeof(double) * elem_size);
      double* varray = new (r) double[elem_size];
      p = mAlloc.get_memory(sizeof(EiDeclArrayR));
      decl = new (p) EiDeclArrayR(head, pt_item, dim_size, range_array,
				  varray);
    }
    break;

  case kVpiNamedEvent:
    p = mAlloc.get_memory(sizeof(EiDeclArrayN));
    decl = new (p) EiDeclArrayN(head, pt_item, dim_size, range_array);
    break;

  case kVpiParameter:
  case kVpiSpecParam:
  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return decl;
}


//////////////////////////////////////////////////////////////////////
// クラス EiDeclArray
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] dim_size 次元数
// @param[in] range_array 範囲の配列
EiDeclArray::EiDeclArray(ElbDeclHead* head,
			 const PtNamedBase* pt_item,
			 ymuint dim_size,
			 EiRange* range_array) :
  mHead(head),
  mPtItem(pt_item),
  mRangeList(dim_size, range_array)
{
}

// @brief デストラクタ
EiDeclArray::~EiDeclArray()
{
}

// @brief 型の取得
// @return vpi_user.h で定義された型 (vpiModule など)
tVpiObjType
EiDeclArray::type() const
{
  switch ( mHead->type() ) {
  case kVpiNet:        return kVpiNetArray;
  case kVpiReg:        return kVpiRegArray;
  case kVpiNamedEvent: return kVpiNamedEventArray;
  case kVpiIntegerVar: return kVpiIntegerVar;
  case kVpiRealVar:    return kVpiRealVar;
  case kVpiTimeVar:    return kVpiTimeVar;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return kVpiNetArray;
}

// @brief 要素の型を返す．
tVpiObjType
EiDeclArray::elem_type() const
{
  return mHead->type();
}

// @brief ファイル位置を返す．
FileRegion
EiDeclArray::file_region() const
{
  return mPtItem->file_region();
}

// @brief このオブジェクトの属しているスコープを返す．
const VlNamedObj*
EiDeclArray::parent() const
{
  return mHead->parent();
}

// @brief 名前の取得
const char*
EiDeclArray::name() const
{
  return mPtItem->name();
}

// @breif 値の型を返す．
// @note 値を持たないオブジェクトの場合には kVpiValueNone を返す．
VlValueType
EiDeclArray::value_type() const
{
  return VlValueType();
}

// @brief 符号の取得
// @retval true 符号つき
// @retval false 符号なし
bool
EiDeclArray::is_signed() const
{
  return mHead->is_signed();
}

// @brief 範囲指定を持つとき true を返す．
bool
EiDeclArray::has_range() const
{
  return mHead->has_range();
}

// @brief 範囲の MSB の値を返す．
// @note 範囲を持たないときの値は不定
int
EiDeclArray::left_range_val() const
{
  return mHead->left_range_val();
}

// @brief 範囲の LSB の値を返す．
// @note 範囲を持たないときの値は不定
int
EiDeclArray::right_range_val() const
{
  return mHead->right_range_val();
}

// @brief 範囲のMSBを表す文字列の取得
// @note 範囲を持たない時の値は不定
string
EiDeclArray::left_range_string() const
{
  return mHead->left_range_string();
}

// @brief 範囲のLSBを表す文字列の取得
// @note 範囲を持たない時の値は不定
string
EiDeclArray::right_range_string() const
{
  return mHead->right_range_string();
}

// @brief left_range >= right_range の時に true を返す．
bool
EiDeclArray::is_big_endian() const
{
  return mHead->is_big_endian();
}

// @brief left_range <= right_range の時に true を返す．
bool
EiDeclArray::is_little_endian() const
{
  return mHead->is_little_endian();
}

// @brief ビット幅を返す．
ymuint
EiDeclArray::bit_size() const
{
  return mHead->bit_size();
}

// @brief オフセット値の取得
// @param[in] index インデックス
// @param[out] offset インデックスに対するオフセット値
// @retval true インデックスが範囲内に入っている時
// @retval false インデックスが範囲外の時
bool
EiDeclArray::calc_bit_offset(int index,
			     ymuint& offset) const
{
  return mHead->calc_bit_offset(index, offset);
}

// @brief データ型の取得
// @retval データ型 kParam, kLocalParam, kVar の場合
// @retval kVpiVarNone 上記以外
tVpiVarType
EiDeclArray::data_type() const
{
  return mHead->data_type();
}

// @brief net 型の取得
// @retval net 型 net 型の要素の場合
// @retval kVpiNone net 型の要素でない場合
tVpiNetType
EiDeclArray::net_type() const
{
  return mHead->net_type();
}

// @brief vectored|scalared 属性の取得
// @retval kVpiVsNone vectored|scalared 指定なし
// @retval kVpiVectored vectored 指定あり
// @retval kVpiScalared scalared 指定あり
tVpiVsType
EiDeclArray::vs_type() const
{
  return mHead->vs_type();
}

// @brief drive0 strength の取得
// @retval 0 の強度
// @retval kVpiNoStrength strength の指定なし
tVpiStrength
EiDeclArray::drive0() const
{
  return mHead->drive0();
}

// @brief drive1 strength の取得
// @retval 1 の強度
// @retval kVpiNoStrength strength の指定なし
tVpiStrength
EiDeclArray::drive1() const
{
  return mHead->drive1();
}

// @brief charge strength の取得
// @retval 電荷の強度
// @retval kVpiNoStrength strength の指定なし
tVpiStrength
EiDeclArray::charge() const
{
  return mHead->charge();
}

// @brief delay の取得
// @retval delay
// @retval nullptr delay の指定なし
const VlDelay*
EiDeclArray::delay() const
{
  return mHead->delay();
}

// @brief 配列型オブジェクトの時に true を返す．
bool
EiDeclArray::is_array() const
{
  return true;
}

// @brief 多次元の配列型オブジェクトの時に true を返す．
bool
EiDeclArray::is_multi_array() const
{
  return dimension() > 1;
}

// @brief 配列型オブジェクトの場合の次元数の取得
ymuint
EiDeclArray::dimension() const
{
  return mRangeList.size();
}

// @brief 範囲の取得
// @param[in] pos 位置 ( 0 <= pos < dimension() )
const VlRange*
EiDeclArray::range(ymuint pos) const
{
  return mRangeList.range(pos);
}

// @brief 配列の要素数の取得
ymuint
EiDeclArray::array_size() const
{
  return mRangeList.elem_size();
}

// @brief 1次元配列の場合にインデックスからオフセットを計算する．
// @param[in] index インデックス
// @param[out] offset index に対するオフセット値
// @retval true index が範囲内だった．
// @retval false index が範囲外だった．
bool
EiDeclArray::calc_array_offset(int index,
			       ymuint& offset) const
{
  if ( mRangeList.size() == 1 ) {
    return mRangeList.range(0)->calc_offset(index, offset);
  }
  return false;
}

// @brief 他次元配列の場合にインデックスのリストからオフセットを計算する．
// @param[in] index_list インデックスのリスト
// @param[out] offset index_list に対するオフセット値
// @retval true オフセットが正しく計算できた．
// @retval false index_list のいずれかの値が範囲外だった．
bool
EiDeclArray::calc_array_offset(const vector<int>& index_list,
			       ymuint& offset) const
{
  return mRangeList.calc_offset(index_list, offset);
}


//////////////////////////////////////////////////////////////////////
/// クラス EiDeclArrayN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] dim_size 次元数
// @param[in] range_array 範囲の配列
EiDeclArrayN::EiDeclArrayN(ElbDeclHead* head,
			   const PtNamedBase* pt_item,
			   ymuint dim_size,
			   EiRange* range_array) :
  EiDeclArray(head, pt_item, dim_size, range_array)
{
}

// @brief デストラクタ
EiDeclArrayN::~EiDeclArrayN()
{
}

// @brief スカラー値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayN::get_scalar(ymuint offset) const
{
  ASSERT_NOT_REACHED;
  return VlScalarVal::x();
}

// @brief スカラー値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayN::set_scalar(ymuint offset,
			 const VlScalarVal& val)
{
  ASSERT_NOT_REACHED;
}

// @brief 論理値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayN::get_logic(ymuint offset) const
{
  ASSERT_NOT_REACHED;
  return VlScalarVal::x();
}

// @brief real 型の値を返す．
// @param[in] offset 要素のオフセット
double
EiDeclArrayN::get_real(ymuint offset) const
{
  ASSERT_NOT_REACHED;
  return 0.0;
}

// @brief real 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayN::set_real(ymuint offset,
		       double val)
{
  ASSERT_NOT_REACHED;
}

// @brief bitvector 型の値を返す．
// @param[in] offset 要素のオフセット
void
EiDeclArrayN::get_bitvector(ymuint offset,
			    BitVector& bitvector,
			    const VlValueType& req_type) const
{
  ASSERT_NOT_REACHED;
}

// @brief bitvector 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayN::set_bitvector(ymuint offset,
			    const BitVector& val)
{
  ASSERT_NOT_REACHED;
}

// @brief ビット選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
VlScalarVal
EiDeclArrayN::get_bitselect(ymuint offset,
			    int index) const
{
  ASSERT_NOT_REACHED;
  return VlScalarVal::x();
}

// @brief ビット値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
// @param[in] val 値
void
EiDeclArrayN::set_bitselect(ymuint offset,
			    int index,
			    const VlScalarVal& val)
{
  ASSERT_NOT_REACHED;
}

// @brief 範囲選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayN::get_partselect(ymuint offset,
			     int left,
			     int right,
			     BitVector& val) const
{
  ASSERT_NOT_REACHED;
}

// @brief 範囲値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayN::set_partselect(ymuint offset,
			     int left,
			     int right,
			     const BitVector& val)
{
  ASSERT_NOT_REACHED;
}


//////////////////////////////////////////////////////////////////////
// クラス EiDeclArrayS
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] dim_size 次元数
// @param[in] range_array 範囲の配列
// @param[in] varray 値を納める配列
EiDeclArrayS::EiDeclArrayS(ElbDeclHead* head,
			   const PtNamedBase* pt_item,
			   ymuint dim_size,
			   EiRange* range_array,
			   VlScalarVal* varray) :
  EiDeclArray(head, pt_item, dim_size, range_array),
  mValArray(varray)
{
}

// @brief デストラクタ
EiDeclArrayS::~EiDeclArrayS()
{
}

// @brief スカラー値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayS::get_scalar(ymuint offset) const
{
  return mValArray[offset];
}

// @brief スカラー値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayS::set_scalar(ymuint offset,
			 const VlScalarVal& val)
{
  mValArray[offset] = val;
}

// @brief 論理値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayS::get_logic(ymuint offset) const
{
  return get_scalar(offset).to_logic();
}

// @brief real 型の値を返す．
// @param[in] offset 要素のオフセット
double
EiDeclArrayS::get_real(ymuint offset) const
{
  return mValArray[offset].to_real();
}

// @brief real 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayS::set_real(ymuint offset,
		       double val)
{
  mValArray[offset] = VlScalarVal(val);
}

// @brief bitvector 型の値を返す．
// @param[in] offset 要素のオフセット
void
EiDeclArrayS::get_bitvector(ymuint offset,
			    BitVector& bitvector,
			    const VlValueType& req_type) const
{
  bitvector = mValArray[offset];
  bitvector.coerce(req_type);
}

// @brief bitvector 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayS::set_bitvector(ymuint offset,
			    const BitVector& val)
{
  mValArray[offset] = val.to_scalar();
}

// @brief ビット選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
VlScalarVal
EiDeclArrayS::get_bitselect(ymuint offset,
			    int index) const
{
  ymuint bpos;
  if ( calc_bit_offset(index, bpos) ) {
    // bpos == 0 のはず
    return mValArray[offset];
  }
  else {
    // 範囲外は X
    return VlScalarVal::x();
  }
}

// @brief ビット値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
// @param[in] val 値
void
EiDeclArrayS::set_bitselect(ymuint offset,
			    int index,
			    const VlScalarVal& val)
{
  ymuint bpos;
  if ( calc_bit_offset(index, bpos) ) {
    // bpos == 0 のはず．
    mValArray[offset] = val;
  }
}

// @brief 範囲選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayS::get_partselect(ymuint offset,
			     int left,
			     int right,
			     BitVector& val) const
{
  ymuint bpos1;
  ymuint bpos2;
  if ( calc_bit_offset(left, bpos1) &&
       calc_bit_offset(right, bpos2) ) {
    // bpos1 == bpos2 == 0 のはず
    val = mValArray[offset];
  }
}

// @brief 範囲値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayS::set_partselect(ymuint offset,
			     int left,
			     int right,
			     const BitVector& val)
{
  ymuint bpos1;
  ymuint bpos2;
  if ( calc_bit_offset(left, bpos1) &&
       calc_bit_offset(right, bpos2) ) {
    // bpos1 == bpos2 == 0 のはず
    mValArray[offset] = val.to_scalar();
  }
}


//////////////////////////////////////////////////////////////////////
// クラス EiDeclArrayR
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] dim_size 次元数
// @param[in] range_array 範囲の配列
// @param[in] varray 値を納める配列
EiDeclArrayR::EiDeclArrayR(ElbDeclHead* head,
			   const PtNamedBase* pt_item,
			   ymuint dim_size,
			   EiRange* range_array,
			   double* varray) :
  EiDeclArray(head, pt_item, dim_size, range_array),
  mValArray(varray)
{
}

// @brief デストラクタ
EiDeclArrayR::~EiDeclArrayR()
{
}

// @brief スカラー値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayR::get_scalar(ymuint offset) const
{
  return VlScalarVal(mValArray[offset]);
}

// @brief スカラー値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayR::set_scalar(ymuint offset,
			 const VlScalarVal& val)
{
  mValArray[offset] = val.to_real();
}

// @brief 論理値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayR::get_logic(ymuint offset) const
{
  return VlScalarVal(get_real(offset));
}

// @brief real 型の値を返す．
// @param[in] offset 要素のオフセット
double
EiDeclArrayR::get_real(ymuint offset) const
{
  return mValArray[offset];
}

// @brief real 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayR::set_real(ymuint offset,
		       double val)
{
  mValArray[offset] = val;
}

// @brief bitvector 型の値を返す．
// @param[in] offset 要素のオフセット
void
EiDeclArrayR::get_bitvector(ymuint offset,
			    BitVector& bitvector,
			    const VlValueType& req_type) const
{
  bitvector = mValArray[offset];
}

// @brief bitvector 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayR::set_bitvector(ymuint offset,
			    const BitVector& val)
{
  mValArray[offset] = val.to_real();
}

// @brief ビット選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
VlScalarVal
EiDeclArrayR::get_bitselect(ymuint offset,
			    int index) const
{
  ASSERT_NOT_REACHED;
  return VlScalarVal::x();
}

// @brief ビット値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
// @param[in] val 値
void
EiDeclArrayR::set_bitselect(ymuint offset,
			    int index,
			    const VlScalarVal& val)
{
  ASSERT_NOT_REACHED;
}

// @brief 範囲選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayR::get_partselect(ymuint offset,
			     int left,
			     int right,
			     BitVector& val) const
{
  ASSERT_NOT_REACHED;
}

// @brief 範囲値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayR::set_partselect(ymuint offset,
			     int left,
			     int right,
			     const BitVector& val)
{
  ASSERT_NOT_REACHED;
}


//////////////////////////////////////////////////////////////////////
// クラス EiDeclArrayV
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] parent 親のスコープ
// @param[in] head ヘッダ
// @param[in] pt_item パース木の宣言要素
// @param[in] dim_size 次元数
// @param[in] range_array 範囲の配列
// @param[in] varray 値を納める配列
EiDeclArrayV::EiDeclArrayV(ElbDeclHead* head,
			   const PtNamedBase* pt_item,
			   ymuint dim_size,
			   EiRange* range_array,
			   BitVector* varray) :
  EiDeclArray(head, pt_item, dim_size, range_array),
  mValArray(varray)
{
}

// @brief デストラクタ
EiDeclArrayV::~EiDeclArrayV()
{
}

// @brief スカラー値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayV::get_scalar(ymuint offset) const
{
  return mValArray[offset].to_scalar();
}

// @brief スカラー値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayV::set_scalar(ymuint offset,
			 const VlScalarVal& val)
{
  mValArray[offset] = val;
}

// @brief 論理値を返す．
// @param[in] offset 要素のオフセット
VlScalarVal
EiDeclArrayV::get_logic(ymuint offset) const
{
  return mValArray[offset].to_logic();
}

// @brief real 型の値を返す．
// @param[in] offset 要素のオフセット
double
EiDeclArrayV::get_real(ymuint offset) const
{
  return mValArray[offset].to_real();
}

// @brief real 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayV::set_real(ymuint offset,
		       double val)
{
  mValArray[offset] = val;
}

// @brief bitvector 型の値を返す．
// @param[in] offset 要素のオフセット
void
EiDeclArrayV::get_bitvector(ymuint offset,
			    BitVector& bitvector,
			    const VlValueType& req_type) const
{
  bitvector = mValArray[offset];
  bitvector.coerce(req_type);
}

// @brief bitvector 型の値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] val 値
void
EiDeclArrayV::set_bitvector(ymuint offset,
			    const BitVector& val)
{
  mValArray[offset] = val;
}

// @brief ビット選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
VlScalarVal
EiDeclArrayV::get_bitselect(ymuint offset,
			    int index) const
{
  ymuint bpos;
  if ( calc_bit_offset(index, bpos) ) {
    return mValArray[offset].bit_select(bpos);
  }
  else {
    return VlScalarVal::x();
  }
}

// @brief ビット値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] index ビット位置
// @param[in] val 値
void
EiDeclArrayV::set_bitselect(ymuint offset,
			    int index,
			    const VlScalarVal& val)
{
  ymuint bpos;
  if ( calc_bit_offset(index, bpos) ) {
    mValArray[offset].bit_select(bpos, val);
  }
}

// @brief 範囲選択値を返す．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayV::get_partselect(ymuint offset,
			     int left,
			     int right,
			     BitVector& val) const
{
  ymuint bpos1;
  ymuint bpos2;
  if ( calc_bit_offset(left, bpos1) &&
       calc_bit_offset(right, bpos2) ) {
    val = mValArray[offset].part_select(bpos1, bpos2);
  }
  else {
    int w;
    if ( left > right ) {
      w = left - right + 1;
    }
    else {
      w = right - left + 1;
    }
    val = BitVector(VlScalarVal::x(), w);
  }
}

// @brief 範囲値を設定する．
// @param[in] offset 要素のオフセット
// @param[in] left 範囲の MSB
// @param[in] right 範囲の LSB
// @param[in] val 値
void
EiDeclArrayV::set_partselect(ymuint offset,
			     int left,
			     int right,
			     const BitVector& val)
{
  ymuint bpos1;
  ymuint bpos2;
  if ( calc_bit_offset(left, bpos1) &&
       calc_bit_offset(right, bpos2) ) {
    mValArray[offset].part_select(bpos1, bpos2, val);
  }
}

END_NAMESPACE_YM_VERILOG
