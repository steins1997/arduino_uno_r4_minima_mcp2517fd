# Arduino UNO R4 Minima + CAN FD Shield ライブラリ

## 概要
このライブラリは、**Arduino UNO R4 Minima** と **Longan Labs製 CAN FD Shield for Arduino** を使用し、PCから **CAN2.0 および CAN FD** の通信を行うためのものです。

## 特徴
- **Arduino IDE 対応**: このライブラリは **Arduino IDE** で使用可能です。
- **ライブラリマネージャー対応**: Arduino IDEのライブラリマネージャーから簡単に追加できます。
- **Arduino UNO R4 Minima 専用**: 他のArduinoボードでは動作しない可能性があります。
- **対応シールド**: Longan Labs製 CAN FD Shield for Arduino と組み合わせて使用します。
- **CAN2.0 & CAN FD 対応**: PCからコマンドを送信することで、**CAN2.0 および CAN FD** の通信を制御できます。

## 通信仕様
- **通信プロトコル**: USB-CDC (仮想COMポート)
- **ボーレート**: 115200
- **データビット**: 8ビット
- **パリティ**: なし
- **ストップビット**: 1ビット
- **フロー制御**: なし

## 動作モード
このファームウェアには **「CAN2.0」** と **「CANFD」** の2つの動作モードが存在します。
デフォルトでは「CANFD」モードになっており、**CAN2.0とCANFDの通信が可能** です。

- **動作モード確認**: `@p` コマンドを使用
  ```
  [INFO] CAN OPERATION : CANFD
  ```

## コマンド一覧
### 動作モードの切り替え
| コマンド | 説明 |
|----------|------|
| `@can2.0` | 動作モードを **CAN2.0** に設定 |
| `@canfd`  | 動作モードを **CANFD** に設定 |

### CAN2.0 フレーム送信
| コマンド | 説明 |
|----------|------|
| `#` | CAN 2.0 プロトコルを使用してフレームを送信 |

**フォーマット:**
```
AA#11.22.33.44.55.66.77.88
0xAA#0x11.0x22.0x33.0x44.0x55.0x66.0x77.0x88
```

### CANFD フレーム送信
| コマンド | 説明 |
|----------|------|
| `##` | CANFDプロトコルを使用してフレームを送信（動作モードが CANFD のときのみ） |

**フォーマット:**
```
AA##11.22.33.44.55.66.77.88
0xAA##0x11.0x22.0x33.0x44.0x55.0x66.0x77.0x88.0x99
```

### ビットレート設定
#### CAN2.0 ビットレート設定
| コマンド | 説明 |
|----------|------|
| `@b=値` | CAN2.0 のビットレートを設定 |

**例:**
```
@b=500KBPS  // 500 kbit/s に設定
```

#### CANFD ビットレート設定
| コマンド | 説明 |
|----------|------|
| `@fb=値` | CANFD のビットレートを設定 |

**例:**
```
@fb=500K3M  // 500 kbit/s, データビットレート 3 Mbit/s
```

### ビットレート一覧
#### CAN2.0
5KBPS, 10KBPS, 20KBPS, 25KBPS, 31K25BPS, 33KBPS, 40KBPS, 50KBPS, 80KBPS, 83K3BPS, 95KBPS, 100KBPS, 125KBPS, 200KBPS, 250KBPS, 500KBPS, 666KBPS, 800KBPS, 1000KBPS

#### CANFD
125K500K, 250K500K, 250K750K, 250K1M, 250K1M5, 250K2M, 250K3M, 250K4M, 500K1M, 500K2M, 500K3M, 500K4M, 500K5M, 500K6M5, 500K8M, 500K10M, 1000K4M, 1000K8M

## フレームの受信
本システムは、同一バス上の他ノードから送信されるフレームを受信し、**ACKスロットで信号の駆動を行います**。

---

このようにフォーマットを整えると、**情報が整理され、可読性が向上** します。
何か追加・修正したい点があれば教えてください！




