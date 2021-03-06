﻿
/// @file GzCoder_test.cc
/// @brief GzCoder のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013 Yusuke Matsunaga
/// All rights reserved.


#include "io/GzCoder.h"
#include <fcntl.h>


BEGIN_NAMESPACE_YM

int
GzCoder_test(int argc,
	   const char** argv)
{
  GzCoder coder;

  if ( argc != 3 ) {
    cerr << "USAGE: " << argv[0] << " <input-filename> <output-filename>" << endl;
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if ( fd < 0 ) {
    cerr << argv[1] << ": No such file" << endl;
    return -1;
  }

  if ( !coder.open(argv[2]) ) {
    cerr << argv[2] << ": Could not create" << endl;
    return -1;
  }

  for ( ; ; ) {
    const ymuint BUFF_SIZE = 4096;
    ymuint8 buff[BUFF_SIZE];
    ssize_t n = read(fd, buff, BUFF_SIZE);
    if ( n < 0 ) {
      cerr << "ERROR in read(): n = " << n << endl;
      return -1;
    }
    if ( n == 0 ) {
      break;
    }
    ssize_t n2 = coder.write(buff, n);
    if ( n2 != n ) {
      cerr << "ERROR in GzCoder::write(): n2 = " << n2 << endl;
      return -2;
    }
  }

  return 0;
}

END_NAMESPACE_YM


int
main(int argc,
     const char** argv)
{
  using namespace nsYm;

  return GzCoder_test(argc, argv);
}
