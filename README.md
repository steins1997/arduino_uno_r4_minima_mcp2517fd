<概要>
このライブラリは、Arduino UNO R4 MinimaとLongan Labs製CAN FD Shield for Arduinoを使用して、PCからCAN2.0およびCAN FDの通信を行うためのものです。

<特徴>
・Arduino IDE対応: このライブラリとソースコードはArduino IDEで使用可能です。
・ライブラリマネージャー対応: Arduino IDEのライブラリマネージャーから簡単に追加できます。
・Arduino UNO R4 Minima専用: 他のArduinoボードでは動作しない可能性があります。
・対応シールド: Longan Labs製 CAN FD Shield for Arduino (製品ページ)と組み合わせて使用します。
・CAN2.0 & CAN FD対応: PCからコマンドを送信することで、CAN2.0およびCAN FDの通信を制御できます。

<通信仕様>
通信プロトコル：USB-CDC (仮想COMポート)
ボーレート：115200
データビット：8ビット
パリティ：なし
ストップビット：1bit
フロー制御：なし

＜動作モードについて＞
本ファームウェアには「CAN2.0」と「CANFD」の２つの動作モードが存在します。
デフォルトでは、「CANFD」モードになっており、CAN2.0とCANFDの通信が可能です。
動作モードの確認は「@p」コマンドで確認でき、以下のように表示されます。
[INFO] CAN OPERATION : CANFD

<コマンド>
【CAN2.0動作モード設定：@can2.0】 
　動作モードが「CAN2.0」になります。

【CANFD動作モード設定：@canfd】 
　動作モードが「CANFD」になります。

【CAN2.0送信：<ID>#<DATA>】
　CAN 2.0プロトコルを使用してフレームを送信します。
  <ID>　0x00～0x1FFFFFFFまでのIDを16進数で指定 (0x1A, 1A, 1a 形式)
  <DATA>　0x00～0xFFまでのデータを最大8バイト指定 (.（ドット）区切り)
　AA#11.22.33.44.55.66.77.88
  0xAA#0x11.0x22.0x33.0x44.0x55.0x66.0x77.0x88

【CANFD送信：<ID>##<DATA>】
  CANFDプロトコルを使用してフレームを送信します。
  ＊動作モードが「CANFD」の時のみ使用可能です
  <ID>　0x00～0x1FFFFFFFまでのIDを16進数で指定 (0x1A, 1A, 1a 形式)
  <DATA>　0x00～0xFFまでのデータを最大64バイト指定 (.（ドット）区切り)
  AA##11.22.33.44.55.66.77.88
  0xAA##0x11.0x22.0x33.0x44.0x55.0x66.0x77.0x88.0x99

【CAN2.0ビットレート設定：@b=<BITRATE>】
　ビットレートを設定
  <BITRATE>　ビットレート	
  @b=5KBPS　@b=1000KBPS
  ＊「CAN2.0送信：<ID>#<DATA>」使用時は、本コマンドで設定したビットレートで送信されます。

  コマンド引数/ビットレート
  5KBPS	      5 kbit/s
  10KBPS	    10 kbit/s
  20KBPS	    20 kbit/s
  25KBPS	    25 kbit/s
  31K25BPS	  31.25 kbit/s
  33KBPS	    33 kbit/s
  40KBPS	    40 kbit/s
  50KBPS	    50 kbit/s
  80KBPS	    80 kbit/s
  83K3BPS	    83.3 kbit/s
  95KBPS	    95 kbit/s
  100KBPS	    100 kbit/s
  125KBPS	    125 kbit/s
  200KBPS	    200 kbit/s
  250KBPS	    250 kbit/s
  500KBPS	    500 kbit/s
  666KBPS	    666 kbit/s
  800KBPS	    800 kbit/s
  1000KBPS	  1000 kbit/s


【CANFDビットレート設定：@fb=<BITRATE>】
　ビットレートを設定
  <BITRATE>　ビットレート	
  @fb=250K3M	@fb=500K6M5	@fb=1000K8M
  ＊「CAN2.0送信：<ID>#<DATA>」使用時は、本コマンドで設定したビットレートで送信されます。
  ＊「CANFD送信：<ID>##<DATA>」使用時は、本コマンドで設定したビットレート・データビットレートで送信されます。
  
  コマンド引数/ビットレート/データビットレート　
	125K500K	  125 kbit/s	  500 kbit/s
	250K500K	  250 kbit/s	  500 kbit/s
	250K750K	  250 kbit/s	  750 kbit/s
	250K1M	    250 kbit/s	  1 Mbit/s
	250K1M5	    250 kbit/s	  1.5 Mbit/s
	250K2M	    250 kbit/s	  2 Mbit/s
	250K3M	    250 kbit/s	  3 Mbit/s
	250K4M	    250 kbit/s	  4 Mbit/s
	500K1M	    500 kbit/s	  1 Mbit/s
	500K2M	    500 kbit/s	  2 Mbit/s
	500K3M	    500 kbit/s	  3 Mbit/s
	500K4M	    500 kbit/s	  4 Mbit/s
	500K5M	    500 kbit/s	  5 Mbit/s
	500K6M5	    500 kbit/s	  6.5 Mbit/s
	500K8M	    500 kbit/s	  8 Mbit/s
	500K10M	    500 kbit/s	  10 Mbit/s
	1000K4M	    1000 kbit/s	  4 Mbit/s
	1000K8M	    1000 kbit/s	  8 Mbit/s


<CAN2.0 / CANFDフレームの受信について>
本システムは、同一バス上に接続された他のノードから送信されるフレームを受信する際に、
ACKスロットにおいて信号の駆動を行う機能を持つ。

　


