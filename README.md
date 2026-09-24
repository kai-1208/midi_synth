# MIDI Synth v1.0.0 - Low-Latency Piano

Windows環境で電子キーボード（Yamaha P-45等）からの入力を低遅延で発音させる、スタンドアロンのピアノ音源アプリ。

## 主な特徴 (Features)
- **超低遅延オーディオ再生**: WASAPI排他モードと128サンプルのバッファにより、演奏時の遅延を感じにくい。
- **高音質サンプリング音源**: SoundFont (.sf2) に対応し、本格的なグランドピアノの音色を再生可能。
- **USB-MIDI プラグ＆プレイ**: 接続されたMIDI機器を自動検出し、ポート番号を選ぶだけで即座に演奏可能。
- **サステインペダル使用可能**: MIDI機器に接続されているサステインペダルを検出し、自動で使用可能。

---

## 使い方 (How to Use)

1. `Release v1.0.0`下部の **`Assets`** から `SalamanderGrandPiano-V3+20200602.sf2`と`midi_synth.exe` を同じフォルダにダウンロードする。
2. `SalamanderGrandPiano-V3+20200602.sf2`は、`piano.sf2`という名前にする。
3. 電子キーボードをUSBケーブルでPCに接続し、電源を入れる。
4. **`midi_synth.exe`** をダブルクリックして起動する。
5. コンソールに表示されるMIDIポート一覧から、お使いのキーボードの番号（例: `0`）を入力して `Enter` を押すと、演奏が可能になる。
6. 終了する際は、コンソールで `Enter` キーを押す。

### ※もし自前でsf2を用意する場合は、同じ手順で名前を`piano.sf2`という名前にしてください。

---

## ご利用にあたっての注意点 (Important Notes)
- **有線イヤホン・ヘッドホン / スピーカーを強く推奨**: 
  Bluetoothイヤホン・ヘッドホンは通信規格上200〜400ms程度の遅延が発生するため、必ずPCのジャックに有線接続してご使用ください。
- **対応OS**: Windows 10 / 11 (64-bit)
- 私が使用したのはYamaha P-45という電子キーボードであるため、他の機器ではうまく動作しない可能性があります。ご了承ください。

### ※重要1. <br>このGitHub Releasesは、1GB以上あるSoundFontファイルをアップロードするため作成したものになる。そのため、最新のMIDI Synthを使用する際は、このgitリポジトリからcmake buildを行って、生成されたmidi_synth.exeを使用するようにしてください。
### ※重要2. <br>これは、個人制作による単なる趣味程度のものになっております。ご了承ください。

---

## 謝辞・ライセンス (Credits & Third-Party)
- Audio Engine: [RtAudio](https://github.com/thestk/rtaudio) & [RtMidi](https://github.com/thestk/rtmidi)
- SoundFont Renderer: [TinySoundFont](https://github.com/schellingb/TinySoundFont)
- Piano SoundFont: Salamander Grand Piano (CC BY 3.0 by Alexander Holm) [FreeParts](https://freepats.zenvoid.org/Piano/acoustic-grand-piano.html)
