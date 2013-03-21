
/// @file tseconv.cc
/// @brief tseconev の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#if HAVE_POPT
#include <popt.h>
#else
#error "<popt.h> not found."
#endif

#include "igf_nsdef.h"


BEGIN_NAMESPACE_YM_IGF

// usage を出力する．
void
usage(poptContext optCon,
      int exitcode,
      const char* error = NULL,
      const char* addl = NULL)
{
  poptPrintUsage(optCon, stderr, 0);
  if ( error ) {
    fprintf(stderr, "%s: %s\n", error, addl);
  }
  exit(exitcode);
}

int
tseconv(int argc,
	const char** argv)
{

  // オプション解析用のデータ
  const struct poptOption options[] = {
    // long-option
    // short-option
    // argument type
    // variable address
    // option tag
    // docstr
    // argstr
    { "digit", 'd', POPT_ARG_NONE, NULL, 'd',
      "digit mode", NULL},
    { "ascii", 'a', POPT_ARG_NONE, NULL, 'a',
      "ascii mode", NULL},

    POPT_AUTOHELP

    { NULL, '\0', 0, NULL, 0, NULL, NULL }
  };

  // オプション解析用のコンテキストを生成する．
  poptContext popt_context = poptGetContext(NULL, argc, argv, options, 0);
  poptSetOtherOptionHelp(popt_context, " <filename>");

  // オプション解析行う．
  bool digit_mode = false;
  bool ascii_mode = false;
  for ( ; ; ) {
    int rc = poptGetNextOpt(popt_context);
    if ( rc == -1 ) {
      break;
    }
    if ( rc < -1 ) {
      // エラーが起きた．
      cerr << poptBadOption(popt_context, POPT_BADOPTION_NOALIAS)
	   << ": " << poptStrerror(rc) << endl;
      return -1;
    }
    if ( rc == 'd' ) {
      digit_mode = true;
    }
    else if ( rc == 'a' ) {
      ascii_mode = true;
    }
  }
  if ( digit_mode && ascii_mode ) {
    cerr << "Both --digit and --ascii cannot be specified at the same time." << endl
	 << "--digit assumed" <<endl;
    ascii_mode = false;
  }
  if ( !digit_mode && !ascii_mode ) {
    digit_mode = true;
  }

  const char* f_str = poptGetArg(popt_context);
  if ( f_str == NULL ) {
    usage(popt_context, 2);
    return -1;
  }

  const char* dummy = poptGetArg(popt_context);
  if ( dummy != NULL ) {
    usage(popt_context, 2);
    return -1;
  }

  ifstream ifs(f_str);
  if ( !ifs ) {
    cerr << argv[1] << " : No such file" << endl;
    return -1;
  }

  ymuint line = 1;
  string buf;
  while ( getline(ifs, buf) ) {
    if ( buf.size() != 4 ) {
      cerr << "Illegal format string at line " << line << endl;
      return -1;
    }
    ymuint val[4];
    for (ymuint i = 0; i < 4; ++ i) {
      char c = buf[i];
      if ( c < '0' || c > '9' ) {
	cerr << "Illegal character '" << c << "' at line " << line << endl;
	return -1;
      }
      val[i] = static_cast<ymuint>(c - '0');
    }
    if ( digit_mode ) {
      for (ymuint i = 0; i < 4; ++ i) {
	for (ymuint j = 4; j > 0; -- j) {
	  if ( val[i] & (1 << (j - 1)) ) {
	    cout << '1';
	  }
	  else {
	    cout << '0';
	  }
	}
      }
      cout << endl;
    }
    else {
      for (ymuint i = 0; i < 4; ++ i) {
	ymuint aval = val[i] + '0';
	for (ymuint j = 7; j > 0; -- j) {
	  if ( aval & (1 << (j - 1)) ) {
	    cout << '1';
	  }
	  else {
	    cout << '0';
	  }
	}
      }
      cout << endl;
    }
    ++ line;
  }

  return 0;
}

END_NAMESPACE_YM_IGF

int
main(int argc,
     const char** argv)
{
  using nsYm::nsIgf::tseconv;

  return tseconv(argc, argv);
}
