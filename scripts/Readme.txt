主に配布パッケージを作るスクリプト関連のファイル置き場


deploymac.sh		Macでqmakeから出来上がりのzipを作るところまで
deploymaclibs.txt	*.appに含まれるライブラリの一覧
macdeployhelper510.sh	install_name_toolを簡単にできるやつ


使い方
１．スクリプトファイルのパスの修正
　　Qtのインストールフォルダが以下のファイルに書いてあるので修正。
　　環境によってパス違うのでそこを吸収できるようにはできてないです。
　　Qt5.1.0に含まれるバイナリのリンクパスがおかしくてスラッシュが重なってるところがあるので変な感じになってます。
	deplymac.sh
	macdeployhelper510.sh
２．プロジェクトのフォルダで実行
　　scripts/deploymac.sh

終了

