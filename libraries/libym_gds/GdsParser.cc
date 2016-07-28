
/// @file GdsParser.cc
/// @brief GdsParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2012 Yusuke Matsunaga
/// All rights reserved.


#include "ym_gds/GdsParser.h"
#include "ym_gds/Msg.h"
#include "GdsRecTable.h"

#include "ym_gds/GdsACL.h"
#include "ym_gds/GdsBoundary.h"
#include "ym_gds/GdsColRow.h"
#include "ym_gds/GdsDate.h"
#include "ym_gds/GdsProperty.h"
#include "ym_gds/GdsStrans.h"
#include "ym_gds/GdsString.h"
#include "ym_gds/GdsUnits.h"
#include "ym_gds/GdsXY.h"


BEGIN_NAMESPACE_YM_GDS_PARSER

#include "gds_grammer.hh"

extern
int yyparse(GdsParser&);

END_NAMESPACE_YM_GDS_PARSER


BEGIN_NAMESPACE_YM_GDS

// @brief コンストラクタ
GdsParser::GdsParser() :
  mAlloc(4096)
{
}

// @brief デストラクタ
GdsParser::~GdsParser()
{
}

// @brief ファイルを読み込む．
// @param[in] filename ファイル名
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
GdsParser::parse(const string& filename)
{
  if ( !mScanner.open_file(filename) ) {
    return false;
  }

  nsParser::yyparse(*this);

  mScanner.close_file();

  return true;
}

// @brief GdsBoundary の作成
GdsElement*
GdsParser::new_boundary(ymuint16 elflags,
			ymint32 plex,
			ymint16 layer,
			ymint16 datatype,
			GdsXY* xy)
{
  void* p = mAlloc.get_memory(sizeof(GdsBoundary));
  GdsBoundary* boundary = new (p) GdsBoundary(elflags, plex, layer, datatype, xy);

  return boundary;
}

// @brief GdsStrans の作成
GdsStrans*
GdsParser::new_strans(ymuint flags,
		      double mag,
		      double angle)
{
  void* p = mAlloc.get_memory(sizeof(GdsStrans));
  GdsStrans* strans = new (p) GdsStrans(flags, mag, angle);

  return strans;
}

// @brief property リストのクリア
void
GdsParser::clear_property()
{
  mPropList.clear();
}

// @brief GdsProperty の作成
// @param[in] attr PROPATTR の値
// @param[in] value PROPVALUE の値
void
GdsParser::add_property(ymuint attr,
			GdsString* value)
{
  void* p = mAlloc.get_memory(sizeof(GdsProperty));
  GdsProperty* prop = new (p) GdsProperty(attr, value);

  mPropList.push_back(prop);
}

// @brief property リストを GdsElement にセットする．
void
GdsParser::set_property(GdsElement* elem)
{
  assert_cond( elem != NULL, __FILE__, __LINE__);

  elem->mPropertyList = mPropList;

  clear_property();
}

// @brief yylex() の実装
int
GdsParser::yylex(nsParser::YYSTYPE& lval)
{
  bool stat = mScanner.read_rec();
  if ( !stat ) {
    // エラー
    return 0;
  }

  const GdsRecTable& table = GdsRecTable::obj();

  tGdsRtype rtype = mScanner.cur_rtype();
  switch ( rtype ) {
  case kGdsBGNLIB:        lval.date_type = new_date(); break;
  case kGdsUNITS:         lval.units_type = new_units(); break;
  case kGdsBGNSTR:        lval.date_type = new_date(); break;
  case kGdsXY:            lval.xy_type = new_xy(); break;
  case kGdsCOLROW:        lval.colrow_type = new_colrow(); break;
  case kGdsPRESENTATION:  lval.bitarray_type = new_bitarray(); break;
  case kGdsSTRANS:        lval.bitarray_type = new_bitarray(); break;
  case kGdsELFLAGS:       lval.bitarray_type = new_bitarray(); break;
  case kGdsLIBSECUR:      lval.acl_type = new_acl(); break;
  default:
    assert_cond( table.data_num(rtype) <= 1, __FILE__, __LINE__);
    {
      tGdsDtype dtype = mScanner.cur_dtype();
      switch ( dtype ) {
      case kGds2Int:   lval.int2_type = new_int2(); break;
      case kGds4Int:   lval.int4_type = new_int4(); break;
      case kGds8Real:  lval.real_type = new_real(); break;
      case kGdsString: lval.string_type = new_string(); break;
      case kGdsNodata: break;
      default:
	assert_not_reached(__FILE__, __LINE__);
	break;
      }
    }
    break;
  }

  int token = table.rtype_token(rtype);
  return token;
}

// @brief yyerror() の実装
int
GdsParser::yyerror(const char* s)
{
  error_header(__FILE__, __LINE__, "GdsParser", mScanner.cur_pos(), s);
  msg_end();

  return 1;
}

// @brief GdsACL の作成
GdsACL*
GdsParser::new_acl()
{
  ymuint dsize = mScanner.cur_dsize();
  assert_cond( dsize % 6 == 0, __FILE__, __LINE__);
  ymuint n = dsize / 6;
  GdsACL* top = NULL;
  GdsACL** pprev = &top;
  for (ymuint i = 0; i < n; ++ i) {
    ymuint group = mScanner.conv_2byte_int(i * 3 + 0);
    ymuint user = mScanner.conv_2byte_int(i * 3 + 1);
    ymuint access = mScanner.conv_2byte_int(i * 3 + 2);
    void* p = mAlloc.get_memory(sizeof(GdsACL));
    GdsACL* acl = new (p) GdsACL(group, user, access);
    *pprev = acl;
    pprev = &acl->mNext;
  }
  *pprev = NULL;

  return top;
}

// @brief GdsColRow の作成
GdsColRow*
GdsParser::new_colrow()
{
  ymint col = mScanner.conv_2byte_int(0);
  ymint row = mScanner.conv_2byte_int(1);
  void* p = mAlloc.get_memory(sizeof(GdsColRow));
  GdsColRow* colrow = new (p) GdsColRow(col, row);

  return colrow;
}

// @brief GdsDate の作成
GdsDate*
GdsParser::new_date()
{
  void* p = mAlloc.get_memory(sizeof(GdsDate[2]));
  GdsDate* date = new (p) GdsDate[2];
  ymuint year1 = mScanner.conv_2byte_int(0);
  ymuint month1 = mScanner.conv_2byte_int(1);
  ymuint day1 = mScanner.conv_2byte_int(2);
  ymuint hour1 = mScanner.conv_2byte_int(3);
  ymuint min1 = mScanner.conv_2byte_int(4);
  ymuint sec1 = mScanner.conv_2byte_int(5);
  date[0].set(year1, month1, day1, hour1, min1, sec1);

  ymuint year2 = mScanner.conv_2byte_int(6);
  ymuint month2 = mScanner.conv_2byte_int(7);
  ymuint day2 = mScanner.conv_2byte_int(8);
  ymuint hour2 = mScanner.conv_2byte_int(9);
  ymuint min2 = mScanner.conv_2byte_int(10);
  ymuint sec2 = mScanner.conv_2byte_int(11);
  date[1].set(year2, month2, day2, hour2, min2, sec2);

  return date;
}

// @brief GdsString の作成
GdsString*
GdsParser::new_string()
{
  char* src_str = reinterpret_cast<char*>(mScanner.cur_data());
  ymuint len = mScanner.cur_dsize();
  for (ymuint i = 0; i < len; ++ i) {
    if ( src_str[i] == '\0' ) {
      len = i;
      break;
    }
  }

  void* p = mAlloc.get_memory(sizeof(GdsString) + len);
  GdsString* str = new (p) GdsString;

  for (ymuint i = 0; i < len; ++ i) {
    str->mStr[i] = src_str[i];
  }
  str->mStr[len] = '\0';

  return str;
}

// @brief GdsUnits の作成
GdsUnits*
GdsParser::new_units()
{
  double user = mScanner.conv_8byte_real(0);
  double meter = mScanner.conv_8byte_real(1);

  void* p = mAlloc.get_memory(sizeof(GdsUnits));
  GdsUnits* units = new (p) GdsUnits(user, meter);

  return units;
}

// @brief GdsXY の作成
GdsXY*
GdsParser::new_xy()
{
  ymuint dsize = mScanner.cur_dsize();
  assert_cond( dsize % 8 == 0, __FILE__, __LINE__);
  ymuint num = dsize / 4;

  void* p = mAlloc.get_memory(sizeof(GdsXY) + sizeof(ymint32) * (num - 1));
  GdsXY* xy = new (p) GdsXY();

  xy->mNum = num / 2;
  for (ymuint i = 0; i < num; ++ i) {
    xy->mData[i] = mScanner.conv_4byte_int(i);
  }

  return xy;
}

// @brief int2 の作成
ymint16
GdsParser::new_int2()
{
  return mScanner.conv_2byte_int(0);
}

// @brief int4 の作成
ymint32
GdsParser::new_int4()
{
  return mScanner.conv_4byte_int(0);
}

// @brief real の作成
double
GdsParser::new_real()
{
  return mScanner.conv_8byte_real(0);
}

// @brief BitArray の作成
ymuint16
GdsParser::new_bitarray()
{
  return static_cast<ymuint16>(mScanner.conv_2byte_int(0));
}

END_NAMESPACE_YM_GDS