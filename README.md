# iP6 GTK   (PC-6000/6600 Series Emulator) 

by windy6001
Last Update: 2021/4/29

## 概要

isio さんの PC-6000/6600 シリーズエミュレータの iP6 を、GTK3 でメニュー付きにしてみました。
あくまで、コンセプトモデルです。ソースリストがすごいことになっています。正確な処理ができてない部分がありますが、ご容赦ください。

## ビルドの方法

macOS Catalina でビルドしました。
Linuxでもビルドできるかどうかはまだ試してません。
macOS (home brew) の場合、gtk+3 をインストールしてください。

$ make で、ビルドできます。


## 使い方

1. 実行ファイルのあるディレクトリに、rom というディレクトリを作成してください。
2. 必要なROMファイルを、rom ディレクトリの中にコピーしてください。
3. ターミナルから、下記のコマンドを打ち込んでください。

$ ./iP6 

で起動できます。



## 主なできないこと 

- キー入力できません。
- Quitと、About以外のメニューは反応しません。
- 音楽が鳴りません。
- 起動中に設定をいじれません。機種変更もできません。
- その他、iP6 がサポートしていない機能は、使えません。

　一番下以外は、なるべく対応したいです。気長にお待ちください。

## ライセンス

This emulator software is licensed under Marat Fayzullin's fmsx license.
commercial use is prohibited.

The GTK menu program is written by michinari nukazawa.
THe GTK menu program is licensed under BSD Clause-2.


This software has no warranty. The author assumes no responsibility whatsoever for any problems that may arise from using this software.


## 免責事項

このソフトウエアには保証がありません。このソフトウエアを使用したことで問題が発生したとしても、作者は責任を負いません。

## 謝辞

- エミュレータを作っていただいた、isio さんに感謝します。
- Marat Fayzullin さんに感謝します。
- GTK3のメニューのサンプルプログラムを書かれた michinari nukazawa さんに感謝します。
