#! /bin/sh

# mk_builddir.sh
#
# コンパイル用のディレクトリを作るシェルスクリプト
#
# Copyright (C) 2015 Yusuke Matsunaga
# All rights reserved


# このファイルのあるディレクトリを srcdir にセットする．
basedir=`dirname $0`
srcdir=`cd $basedir; pwd`

case $# in
    2)
    ;;

    *)
	echo "USAGE mk_builddir.sh <compiledir> <installdir>"
	exit 1;;
esac

# ビルド用のディレクトリ名
builddir=$1

# インストール先のディレクトリ名
installdir=$2

echo "****"
echo "source  directory: $srcdir"
echo "build   directory: $builddir"
echo "install directory: $installdir"
echo "****"
echo -n "continue with the above configuration ? (yes/no): "
while read confirmation; do
    case $confirmation in
	"yes")
	    break;;
	"no")
	    echo "Setup canceled."
	    exit 0;;
	*)
	    echo "Please answer 'yes' or 'no'"
	    echo -n "continue ? (yes/no)"
	    ;;
    esac
done

# ビルドディレクトリはなければ作る．
test -d $builddir || mkdir -p $builddir

# do_cmake ファイルを作る．
do_cmake="do_cmake.sh"
sed -e s!__SRC_DIR__!$srcdir! \
    -e s!__INSTALL_DIR__!$installdir! \
    $srcdir/etc/${do_cmake}.in > $builddir/${do_cmake}
chmod +x $builddir/${do_cmake}

echo "Build directory setup completed."
echo "Move to '$builddir', and execute './do_cmake.sh'"
