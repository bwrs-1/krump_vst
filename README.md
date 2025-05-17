# CMake Template for JUCE

## Requirements
- CMake 3.22 and above
- Visual Studio 2022
- Visual Studio Code
  - [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
  - [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

Select `Visual Studio 17 2022` in the CMake configuration to build.

# KrumpVST / BUCK REVERB

## プラグインをDAWに反映する方法（インストール手順）

### macOS
1. ビルド後、`KrumpVST.vst3`（または`BUCK REVERB.vst3`）が `~/Library/Audio/Plug-Ins/VST3/` または `/Library/Audio/Plug-Ins/VST3/` にインストールされます。
2. DAW（例: FL Studio, Ableton Live, Reaper等）を再起動します。
3. DAWの「プラグインマネージャ」や「プラグインリスト」でVST3フォルダを再スキャンします。
4. プラグインリストに「KrumpVST」または「BUCK REVERB」が表示されることを確認し、トラックに追加してください。

#### トラブルシューティング
- プラグインが表示されない場合は、DAWのVST3フォルダ設定が正しいか確認し、再スキャン・再起動を試してください。
- macOSでは「リリースビルド」でのみDAWに表示される場合があります（[参考](https://forum.juce.com/t/vst3-plugin-not-visible-in-daw/45691)）。

---

### Windows
1. ビルド後、`KrumpVST.vst3`ファイルを `C:\Program Files\Common Files\VST3\` フォルダにコピーします。
2. DAW（例: FL Studio, Ableton Live, Reaper等）を再起動します。
3. DAWの「プラグインマネージャ」や「プラグインリスト」でVST3フォルダを再スキャンします。
4. プラグインリストに「KrumpVST」または「BUCK REVERB」が表示されることを確認し、トラックに追加してください。

#### トラブルシューティング
- プラグインが表示されない場合は、VST3フォルダのパスがDAWの設定と一致しているか確認し、再スキャン・再起動を試してください。
- 詳細な手順や他のDAW向けの説明は[HipHopMakers: How to Install VST Plugins](https://hiphopmakers.com/how-to-install-vst-plugins-on-windows-fl-studio)も参考にしてください。

---

## How to Install and Use the Plugin in Your DAW

### macOS
1. After building, `KrumpVST.vst3` (or `BUCK REVERB.vst3`) will be installed to `~/Library/Audio/Plug-Ins/VST3/` or `/Library/Audio/Plug-Ins/VST3/`.
2. Restart your DAW (e.g. FL Studio, Ableton Live, Reaper, etc.).
3. Open your DAW's "Plugin Manager" or "Plugin List" and rescan the VST3 folder.
4. Confirm that "KrumpVST" or "BUCK REVERB" appears in the plugin list, then add it to a track.

#### Troubleshooting
- If the plugin does not appear, check your DAW's VST3 folder settings, rescan, and restart the DAW.
- On macOS, sometimes only the Release build is recognized by DAWs ([reference](https://forum.juce.com/t/vst3-plugin-not-visible-in-daw/45691)).

---

### Windows
1. After building, copy the `KrumpVST.vst3` file to `C:\Program Files\Common Files\VST3\`.
2. Restart your DAW (e.g. FL Studio, Ableton Live, Reaper, etc.).
3. Open your DAW's "Plugin Manager" or "Plugin List" and rescan the VST3 folder.
4. Confirm that "KrumpVST" or "BUCK REVERB" appears in the plugin list, then add it to a track.

#### Troubleshooting
- If the plugin does not appear, make sure the VST3 folder path matches your DAW's settings, rescan, and restart the DAW.
- For more details and DAW-specific instructions, see [HipHopMakers: How to Install VST Plugins](https://hiphopmakers.com/how-to-install-vst-plugins-on-windows-fl-studio).