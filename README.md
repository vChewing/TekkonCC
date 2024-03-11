# Tekkon Engine 鐵恨引擎

- Gitee: [Swift](https://gitee.com/vChewing/Tekkon) | [C#](https://gitee.com/vChewing/TekkonNT) | [C++](https://gitee.com/vChewing/TekkonCC)
- GitHub: [Swift](https://github.com/vChewing/Tekkon) | [C#](https://github.com/vChewing/TekkonNT) | [C++](https://github.com/vChewing/TekkonCC)

相關使用說明請參見 Swift 版的倉庫的 README.MD。函式用法完全一致。

鐵恨引擎的 Cpp 版本，依 Cpp 17 標準編寫完成。

該專案雖使用 Swift Package Manager (SPM) 開發維護+單元測試，但不妨礙在任何平台使用，因為**實體只有「Tekkon.hh」這一個檔案（格式：UTF8 無 BOM）**。
- 敝倉庫的 ObjC 檔案全都是給 SPM 專用的單元測試腳本。

該專案推薦使用的建置手段：
- [跨平台 OS] 任何有支援 Cpp17 的 compiler，比如符合這種要求的 Clang 或 GCC。
- [跨平台 OS] Swift 5.9，以 SPM 的形式建置。
  - 因為沒有用到任何 Swift 原始碼，所以建置產物對 Swift 不會構成依賴。SPM 在這裡充當了 CMake 的替代角色。 
- [僅 Apple 平台] 使用 Xcode 的話，須至少 Xcode 15.1。 // Xcode 15.0 因為 Bug 太多的原因不推薦使用。

該專案推薦使用的單元測試手段：
- [僅 Apple 平台] SPM 單元測試，直接使用上文提到的 SPM 或 Xcode 即可。
- [跨平台 OS] GoogleTests，詳見 GTests 目錄下的內容。
  - 但因為 GoogleTests 官方程式包 Archive 無法在某些國家和地區穩定獲取的緣故，敝倉庫不推薦在這些市場使用這個測試手段。
  - SPM 的單元測試是 Objective-C 寫的，但 SPM 在非 Apple 平台下無法使用 ObjC 來完成單元測試。

> 注意：該引擎會將「ㄅㄨㄥ ㄆㄨㄥ ㄇㄨㄥ ㄈㄨㄥ」這四種讀音自動轉換成「ㄅㄥ ㄆㄥ ㄇㄥ ㄈㄥ」、將「ㄅㄨㄛ ㄆㄨㄛ ㄇㄨㄛ ㄈㄨㄛ」這四種讀音自動轉換成「ㄅㄛ ㄆㄛ ㄇㄛ ㄈㄛ」。如果您正在開發的輸入法的詞庫內的「甮」字的讀音沒有從「ㄈㄨㄥˋ」改成「ㄈㄥˋ」、或者說需要保留「ㄈㄨㄥˋ」的讀音的話，請按需修改「receiveKeyfromPhonabet()」函式當中的相關步驟、來跳過該轉換。該情形為十分罕見之情形。類似情形則是台澎金馬審音的慣用讀音「ㄌㄩㄢˊ」，因為使用者眾、所以不會被該引擎自動轉換成「ㄌㄨㄢˊ」。威注音輸入法內部已經從辭典角度做了處理、允許在敲「ㄌㄨㄢˊ」的時候出現以「ㄌㄩㄢˊ」為讀音的漢字。我們鼓勵輸入法開發者們使用 [威注音語彙庫](https://gitee.com/vChewing/libvchewing-data) 來實現對兩岸讀音習慣的同時兼顧。

---

鐵恨引擎是用來處理注音輸入法並擊行為的一個模組。該倉庫乃威注音專案的弒神行動（Operation Longinus）的一部分。

Tekkon Engine is a module made for processing combo-composition of stroke-based Mandarin Chinese phonetics (i.e. Zhuyin / Bopomofo). This repository is part of Operation Longinus of The vChewing Project.

羅馬拼音輸入目前僅支援漢語拼音、國音二式、耶魯拼音、華羅拼音、通用拼音、韋氏拼音（威妥瑪拼音）。

- 因為**趙元任國語羅馬字拼音「無法製作通用的聲調確認鍵」**，故鐵恨引擎在技術上無法實現對趙元任國語羅馬字拼音的支援。

Regarding pinyin input support, we only support: Hanyu Pinyin, Secondary Pinyin, Yale Pinyin, Hualuo Pinyin, Wade-Giles Pinyin and Universal Pinyin.

- **Tekkon is unable to provide support for Zhao Yuan-Ren's Gwoyeu Romatzyh at this moment** because there is no consistent method to check whether the intonation key has been pressed. Tekkon is designed to confirm input with intonation keys.

## 著作權 (Credits)

- Development by (c) 2022 and onwards The vChewing Project (MIT-NTL License).
	- Original Swift developer: Shiki Suen
	- C# and Cpp version developer: Shiki Suen
