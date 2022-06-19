# Tekkon Engine 鐵恨引擎

- Gitee: [Swift](https://gitee.com/vChewing/Tekkon) | [C#](https://gitee.com/vChewing/TekkonNT) | [C++](https://gitee.com/vChewing/TekkonCC)
- GitHub: [Swift](https://github.com/ShikiSuen/Tekkon) | [C#](https://github.com/ShikiSuen/TekkonNT) | [C++](https://github.com/ShikiSuen/TekkonCC)

相關使用說明請參見 Swift 版的倉庫的 README.MD。函式用法完全一致。

鐵恨引擎的 Cpp 版本，依 Cpp 20 標準編寫完成。

該專案雖使用 Xcode 開發維護+單元測試，但不妨礙在任何平台使用，因為實體只有「Tekkon.hh」這一個檔案（格式：UTF8 無 BOM）。

該專案推薦至少使用 Xcode 13.4.1。更舊版的 Xcode 的話，只要你能跑 Cpp 20 沒問題就行。**任何使用 Xcode 以外的工具鏈導致 CMake 無視你對 Cpp20 的指定的情況，不在敝專案的應對範圍內**。

---

鐵恨引擎是用來處理注音輸入法並擊行為的一個模組。該倉庫乃威注音專案的弒神行動（Operation Longinus）的一部分。

Tekkon Engine is a module made for processing combo-composition of stroke-based Mandarin Chinese phonetics (i.e. Zhuyin / Bopomofo). This repository is part of Operation Longinus of The vChewing Project.

羅馬拼音輸入目前僅支援漢語拼音、國音二式、耶魯拼音、華羅拼音、通用拼音。

- 因為**韋氏拼音（威妥瑪拼音）輔音清濁不分的問題非常嚴重**、無法與注音符號形成逐一對應，故鐵恨引擎在技術上無法實現對韋氏拼音的支援。

Regarding pinyin input support, we only support: Hanyu Pinyin, Secondary Pinyin, Yale Pinyin, Hualuo Pinyin, and Universal Pinyin.

- **Tekkon is unable to provide Wade–Giles Pinyin support** since it is impossible to make one-to-one mappings to all possible phonabet combinations (especially it cannot distinguish "k" and "g").


## 著作權 (Credits)

- Development by (c) 2022 and onwards The vChewing Project (MIT-NTL License).
	- Original Swift developer: Shiki Suen
	- C# and Cpp version developer: Shiki Suen
