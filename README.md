# ymtools

YmTools は論理回路の検証・合成アルゴリズムの研究・開発のためのライブラリ群です．
ビルドに関しては BUILDING.md ファイルを参照してください．

## 内容

YmTools は以下のライブラリからなります．

- [libym_utils](libym_utils)

  汎用のユーティリティ関数

- libym_algo

  (どちらかというと)汎用のアルゴリズム

- libym_logic

  論理関数処理用のクラス

- libym_cell

  セルライブラリを扱うクラス

- libym_verilog

  Verilog-HDL を扱うクラス

- libym_networks

  論理回路の構造(ネットワーク)を扱うクラス


## libym_utils

   汎用のユーティリティクラスを集めたライブラリです．
   以下のサブグループより構成されています．

   - alloc

   メモリアロケータ

   - file

   ファイル操作

   - gen

   組み合わせ/順列生成器

   - io

   ファイル入出力

   - itvl

   区間マネージャ

   - misc

   その他

   - msg

   メッセージハンドラ

   - popt

   popt 関数の C++ ラッパ

   - textproc

   テキスト処理


## libym_algo

   アルゴリズムの教科書に載っているような汎用のアルゴリズムを実装したライブラリです．
   以下のサブグループより構成されます．

   - btg

   二部グラフの最適マッチング

   - graph

   グラフの最大クリーク，彩色問題

   - mfset

   併合/検査を行う集合のADT

   - mincov

   最小被覆問題


## libym_logic

   論理関数処理を行うクラスのライブラリです．
   以下のサブグループより構成されます．

   - aig

   AIG(AND INVERTER Graph)

   - bdd

   BDD(Binary Decision Diagram)

   - cnfdd

   CNF(Conjunctive Normal Form) を表す xDD．
   試験的な実装です．

   - expr

   一般の論理式

   - npn

   NPN同値類判定アルゴリズム．
   正確にはNPN同値類の正規形を求めます．

   - sat

   SAT(Satisfiability)問題のソルバ

   - tvfunc

   真理値表を用いた論理関数処理

   - zdd

   ZDD(Zero-surpressed BDD)


## libym_cell

   セルライブラリを扱うクラスライブラリです．
   liberty 形式と genlib(mislib) 形式のファイルを読むことができます．


## libym_verilog

   Verilog-HDL を扱うクラスライブラリです．
   Verilog-1995 と Verilog-2001 の両方に対応しています．
   parse だけでなく，elaboration も行えます．

## libym_networks

   論理回路のネットワークを表すクラスです．
   ファイル形式としては以下の形式をサポートしています．

   - blif
   - .bench (ISCAS89)
   - Verilog-HDL

   内部のデータ構造としては数種類の実装があります．

   - BNetwork

   UCB の mis/sis で用いられている Boolean Network の概念に基づいた実装．
   ただし，mis/sis の Boolean Network がノード内の論理式を積和系論理式
   で持っていたのに対してこちらは一般の論理式(factored form)で持ちます．
   将来的には廃止する方向です．

   - BDN

   一応 Binary Decomposed Network の略．AIG と似ているが，こちらは 2入力XOR
   も用いている．また，ラッチやD-FFといった実際の回路に必要なノードも含んでいます．
   将来的にはメインのデータ構造になるはずです．

   - TgNetwork

   Technology dependent Gate Network の略．
   いわゆるセルのネットリストを表すデータ構造．
   テスト生成の入力やテクノロジマッピングの出力として用いられます．
   仕様としてはあまり煮詰まっていないので変更があるかもしれません．

   - MVN

   Multi-Valued Network
   これも概念は UCB の MVSIS に似ているけど細部は異なります．
   どちらかというと論理合成可能な Verilog-HDL 記述を elaborate した
   結果を格納するためのデータ構造です．
   いわゆる RTL 合成とはこの MVN を BDN に変換する処理のこととなります．
