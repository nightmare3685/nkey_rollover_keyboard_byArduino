<div align="center">

# NKeyboard

**Arduino Leonardoで、6キーを超える同時入力を。**

Arduino標準のKeyboardライブラリをベースにした、Nキーロールオーバー向けの拡張ライブラリです。

![Board: Arduino Leonardo](https://img.shields.io/badge/Board-Arduino%20Leonardo-00979D?style=flat-square)
![Interface: USB HID](https://img.shields.io/badge/Interface-USB%20HID-475569?style=flat-square)
![Key slots: 100](https://img.shields.io/badge/Key%20slots-100-2563EB?style=flat-square)

[導入](#導入) · [使い方](#使い方) · [API](#api) · [仕様と注意点](#仕様と注意点)

</div>

---

## このライブラリについて

Nキーロールオーバー（NKRO）は、複数のキーを同時に押した状態を扱うための仕組みです。NKeyboardは、Arduino LeonardoをUSBキーボードとして使う際のキーレポートを拡張しています。

- **100キー分の配列** — `NKEYBOARD_MAX_KEYS = 100` を使い、キーの保持領域とHIDレポートのキー数を定義。
- **使い慣れたAPI** — `Keyboard.press()`、`Keyboard.release()`、`Keyboard.releaseAll()` で押下状態を操作。
- **修飾キー・特殊キー** — 左右のCtrl / Shift / Alt / GUI、矢印、F1〜F24などの定数を用意。

> 「100」は実装上のキー格納数です。すべての環境で100キーの同時入力が認識されることを保証するものではありません。

## 導入

### 用意するもの

- Arduino Leonardo
- データ通信に対応したUSBケーブル
- Arduino IDEと、Leonardoを選択できるボード環境

本ライブラリは `HID.h` と `_USING_HID` に依存します。Leonardo以外のボードでの互換性は、別途確認してください。

### スケッチと同じフォルダに置く（手軽な方法）

`examples/NKRO` フォルダに `NKeyboard.h` と `NKeyboard.cpp` をコピーし、`NKRO.ino` をArduino IDEで開きます。ボードとポートでArduino Leonardoを選択してください。

```text
NKRO/
├── NKRO.ino
├── NKeyboard.h
└── NKeyboard.cpp
```

この方法では `#include "NKeyboard.h"` を使います。標準ライブラリのファイルを上書きする必要はありません。

### ライブラリとして配置する方法

1. このリポジトリの **Code → Download ZIP** からソースを取得し、展開します。
2. Arduino IDEの設定で「スケッチブックの場所」を確認します。
3. その中の `libraries` フォルダに `NKeyboard` フォルダを作り、次のように配置します。
4. Arduino IDEを再起動し、ボードとポートでArduino Leonardoを選択します。

```text
<スケッチブック>/
└── libraries/
    └── NKeyboard/
        ├── NKeyboard.h
        ├── NKeyboard.cpp
        └── examples/
            └── NKRO/
                └── NKRO.ino
```

スケッチでは、次のヘッダーを読み込みます。操作に使うオブジェクト名は `Keyboard` です。

```cpp
#include <NKeyboard.h>
```

標準の `Keyboard.h` は同時に読み込まないでください。クラス名とオブジェクト名が重複します。

## 使い方

### ボタンを押している間、8キーを保持する

デジタルピン **0** と **GND** の間に押しボタンを接続します。内蔵プルアップを使うため、この例では外付けのプルアップ抵抗は不要です。

```text
D0 ─── 押しボタン ─── GND
```

以下は、ボタンを押したときに `a`〜`h` を順に押下状態にし、離したときにすべて解放する例です。押し続けている間は同じキーを再送しません。

```cpp
#include <NKeyboard.h>

const uint8_t BUTTON_PIN = 0;
bool wasPressed = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Keyboard.begin();
}

void loop() {
  const bool isPressed = digitalRead(BUTTON_PIN) == LOW;

  if (isPressed && !wasPressed) {
    for (char key = 'a'; key <= 'h'; ++key) {
      Keyboard.press(key);
    }
  } else if (!isPressed && wasPressed) {
    Keyboard.releaseAll();
  }

  wasPressed = isPressed;
  delay(10);
}
```

入力先をキー状態の確認ツールなどに切り替えてから、ボタンを押してください。接続先のPCには実際のキーボード入力として送信されます。上記は基本例で、厳密なチャタリング対策は含みません。

### 同梱サンプル

[`examples/NKRO/NKRO.ino`](examples/NKRO/NKRO.ino) は、ピン0とGNDの間のボタンで `a`〜`h` の8キーを保持するサンプルです。20 msのチャタリング対策を含み、状態が変化したときだけ押下・解放を送信します。元のサンプルと同じ配線を使えます。`HELD_KEYS` を変更すると送るキーを選べます。
## API

| 呼び出し | 動作 |
| --- | --- |
| `Keyboard.begin()` | 初期化用のAPI。現在の実装では処理はありません。 |
| `Keyboard.press(key)` | キーを押下状態にし、レポートを送信します。 |
| `Keyboard.release(key)` | 指定キーを解放し、レポートを送信します。 |
| `Keyboard.releaseAll()` | 全キーと修飾キーを解放します。 |
| `Keyboard.write(key)` | 1キーを押して、すぐに解放します。 |
| `Keyboard.print(text)` | `Print` 継承による文字列出力。文字を順に送信します。 |
| `Keyboard.end()` | 終了用のAPI。現在の実装では処理はなく、キーの解放も行いません。 |

`key` には `'a'` のような文字、または `KEY_RETURN`、`KEY_LEFT_CTRL` などの定数を渡します。使用できる定数は [`NKeyboard.h`](NKeyboard.h) を参照してください。

## キー数の設定

使用する `NKeyboard.h` の次の値を変更して、再コンパイルしてください。

```cpp
#define NKEYBOARD_MAX_KEYS 100
```

配列サイズとHIDレポートのキー数が、この値に追従します。設定範囲は1〜255ですが、USB接続先での動作を保証する範囲ではありません。スケッチだけに `#define` を書くと、別にコンパイルされる `.cpp` と設定が一致しないため、ヘッダー側で変更してください。

## 仕様と注意点

- **レポート構造**：修飾キー1バイト、予約領域1バイト、キー配列100バイト。レポートIDは `2` です。
- **文字配列**：ASCIIからの変換はUSキーボード配列を前提としています。OS側の配列設定によって記号などの入力結果が変わります。日本語文字列の直接入力には対応していません。
- **同じキーの再押下**：登録済みのキーは重複追加されません。修飾キーは通常キーのスロットを消費しません。
- **Shiftの共有**：大文字や一部の記号は左Shiftを使います。それらの文字を解放すると左Shiftも解除されるため、組み合わせに注意してください。
- **動作確認**：実際に認識される同時入力数は、利用するボード・USB環境・OS・アプリケーションで確認してください。

## ファイル構成

| ファイル | 内容 |
| --- | --- |
| [`NKeyboard.h`](NKeyboard.h) | キー定数、キー数、レポート構造、公開API |
| [`NKeyboard.cpp`](NKeyboard.cpp) | HIDレポート定義、文字変換、キー入力処理 |
| [`examples/NKRO/NKRO.ino`](examples/NKRO/NKRO.ino) | ピン0を使った入力サンプル |

## ベースとなったコード・ライセンス

ArduinoのKeyboardライブラリを改造したものです。`NKeyboard.h` と `NKeyboard.cpp` のヘッダーには、Arduino LLC（2015）およびPeter Barrett（2011）の著作権表示と、**GNU LGPL 2.1またはそれ以降**のライセンス表記があります。
