/*
 * Copyright 2013-2014 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef KANMUSUMEMORY_GLOBAL_H
#define KANMUSUMEMORY_GLOBAL_H

#define KANMEMO_NAME "KanmusuMemory"
#define KANMEMO_NAME_FAV "KanmusuMemoryFav"
#define KANMEMO_PROJECT "KanmemoProject"
#define KANMEMO_VERSION "0.15 beta"
#define KANMEMO_VERSION_CODE 16
#define KANMEMO_DEVELOPERS (QStringList() \
    << "@IoriAYANE"\
    << "@task_jp"\
    << "@nagakenjs"\
    << "@androidsola"\
    << "@tkhshmsy"\
    << "@nowsprinting"\
    << "@EnderAlice"\
    << "@hermit4"\
    << "@Kaikias_lys"\
    )

//キャシュの内容がQtのバージョンで互換性が無いみたいなので
//標準のパスに↓の文字列を追加したフォルダにする
#define CACHE_LOCATION_SUFFIX       "2"

//設定ファイル
//#define SETTING_FILE_NAME       "settings.ini"
//#define SETTING_FILE_FORMAT     QSettings::IniFormat
//設定：一般
#define SETTING_GENERAL_TOKEN               "token"
#define SETTING_GENERAL_TOKENSECRET         "tokenSecret"
#define SETTING_GENERAL_USER_ID             "user_id"
#define SETTING_GENERAL_SCREEN_NAME         "screen_name"
#define SETTING_GENERAL_UNUSED_TWITTER      "unused_twitter"
#define SETTING_GENERAL_SAVE_PNG            "save_png"
#define SETTING_GENERAL_MASK_ADMIRAL_NAME   "mask_admiral_name"
#define SETTING_GENERAL_MASK_HQ_LEVEL       "mask_hq_level"
#define SETTING_GENERAL_PROXY_ENABLE        "proxy_enable"
#define SETTING_GENERAL_PROXY_HOST          "proxy_host"
#define SETTING_GENERAL_PROXY_PORT          "proxy_port"
#define SETTING_GENERAL_ZOOM_FACTOR         "zoom_factor"
#define SETTING_GENERAL_USE_COOKIE          "use_cookie"
#define SETTING_GENERAL_DISABLE_CONTEXT_MENU    "disable_context_menu"
#define SETTING_GENERAL_DISABLE_EXIT        "disable_exit"
#define SETTING_GENERAL_VIEW_BUTTLE_RESULT  "view_buttle_result"
#define SETTING_GENERAL_VIEW_BUTTLE_RESULT_OPACITY  "view_buttle_result_opacity"
#define SETTING_GENERAL_BUTTLE_RESULT_POSITION  "buttle_result_position"
#define SETTING_GENERAL_TIMER_AUTO_START    "timer_auto_start"
//設定：ウインドウ位置
#define SETTING_MAINWINDOW          "mainwindow"
#define SETTING_TIMERDIALOG         "timerdialog"
#define SETTING_FLEETDETAILDIALOG   "fleetdetaildialog"
#define SETTING_WINDOW_GEO          "geometry"
#define SETTING_WINDOW_STATE        "windowState"
#define SETTING_SPLITTER_SIZES      "splitterSizes"
#define SETTING_SPLITTER_ON         "splitterOn"
#define SETTING_SPLITTER_ORIENTATION    "splitterorientation"
//設定：タイマー
#define SETTING_TIMER                       "timer"
#define SETTING_TIMER_DOCKING_TIME          "dockingTime"
#define SETTING_TIMER_DOCKING_START         "dockingStart"
#define SETTING_TIMER_DOCKING_RUNNING       "dockingRunning"
#define SETTING_TIMER_EXPEDITION_TIME       "expeditionTime"
#define SETTING_TIMER_EXPEDITION_START      "expeditionStart"
#define SETTING_TIMER_EXPEDITION_RUNNING    "expeditionRunning"
#define SETTING_TIMER_CONSTRUCTION_TIME     "constructionTime"
#define SETTING_TIMER_CONSTRUCTION_START    "constructionStart"
#define SETTING_TIMER_CONSTRUCTION_RUNNING  "constructionRunning"
#define SETTING_TIMER_TWEETFINISHED         "tweetFinished"
#define SETTING_TIMER_DOCKING_CLOSE         "dockingClose"
#define SETTING_TIMER_EXPEDITION_CLOSE      "expeditionClose"
#define SETTING_TIMER_CONSTRUCTION_CLOSE    "constructionClose"
//設定：タブ関連
#define SETTING_TAB                         "tab"
#define SETTING_TAB_OPEN_PAGES              "openPages"
#define SETTING_TAB_OPEN_PAGES_MOBILE_MODE  "openPagesMobileMode"
#define SETTING_TAB_SAVE_OPEN_PAGES         "saveOpenPages"
#define SETTING_TAB_OPEN_AND_NEWTAB         "openAndNewTab"
//設定：更新確認
#define SETTING_UPDATE_CHECK                "updateCheck"
#define SETTING_UPDATE_CHECK_HIDE           "hideUntilNextUpdate"
#define SETTING_UPDATE_CHECK_HIDE_VERSION   "hideUntilNextUpdateVersion"
//設定：録画
#define SETTING_RECORD                      "record"
#define SETTING_RECORD_FPS                  "fps"
#define SETTING_RECORD_AUDIO_SOURCE         "audioSource"
#define SETTING_RECORD_SAVE_PATH            "savePath"
#define SETTING_RECORD_TOOL_PATH            "toolPath"
#define SETTING_RECORD_TEMP_PATH            "tempPath"
#define SETTING_RECORD_SOUND_OFFSET         "soundOffset"
#define SETTING_RECORD_DONT_VIEW_BUTTLE     "dontViewButtleResult"
#define SETTING_RECORD_MUTE_TIMER_SOUND     "muteNotificationSound"

//お気に入り
//#define FAV_FILE_NAME                       "favorite.ini"
//#define FAV_FILE_FORMAT                     QSettings::IniFormat
#define FAV_USER                            "user"
#define FAV_USER_BOOKMARK                   "bookmark"


#define CLICK_EVENT_INTERVAL            1500
#define CLICK_EVENT_INTERVAL_LITTLE     500
#define CLICK_EVENT_FLUCTUATION         500

//Ctrl+Qで閉じるのデフォルト値
#ifdef Q_OS_MAC
#define DISABLE_EXIT_DEFAULT             false
#else
#define DISABLE_EXIT_DEFAULT             true
#endif

//カタログと艦隊詳細を無効化
#define DISABLE_CATALOG_AND_DETAIL_FLEET

//カタログ
#define CATALOG_CHECK_COLOR     (qRgb(45, 43, 43))
#define CATALOG_CHECK_RECT      (QRect(0, 0, 5, 5))
#define CATALOG_RECT_CAPTURE    (QRect(150, 60, 610, 380))
#define CATALOG_RECT_SHIP1      (QRect(0, 90, 105, 25))
#define CATALOG_RECT_SHIP2      (QRect(0, 130, 105, 25))
#define CATALOG_RECT_SHIP3      (QRect(0, 170, 105, 25))
#define CATALOG_RECT_PAGE1      (QRect(140, 18, 120, 20))
#define CATALOG_RECT_PAGE2      (QRect(268, 18, 120, 20))
#define CATALOG_RECT_PAGE3      (QRect(396, 18, 120, 20))
#define CATALOG_RECT_PAGE4      (QRect(524, 18, 120, 20))
#define CATALOG_RECT_PAGE5      (QRect(652, 18, 120, 20))

//艦隊詳細
#define DETAIL_CHECK_COLOR1     qRgb(163, 158, 94)
#define DETAIL_CHECK_COLOR2     qRgb(102, 154, 112)
#define DETAIL_RECT_CAPTURE     (QRect(327, 105, 460, 360))
#define DETAIL_RECT_SHIP1_1     QRect(282, 197, 80, 30)
#define DETAIL_RECT_SHIP1_2     QRect(365, 197, 80, 30)
#define DETAIL_RECT_SHIP2_1     QRect(624, 197, 80, 30)
#define DETAIL_RECT_SHIP2_2     QRect(707, 197, 80, 30)
#define DETAIL_RECT_SHIP3_1     QRect(282, 310, 80, 30)
#define DETAIL_RECT_SHIP3_2     QRect(365, 310, 80, 30)
#define DETAIL_RECT_SHIP4_1     QRect(624, 310, 80, 30)
#define DETAIL_RECT_SHIP4_2     QRect(707, 310, 80, 30)
#define DETAIL_RECT_SHIP5_1     QRect(282, 424, 80, 30)
#define DETAIL_RECT_SHIP5_2     QRect(365, 424, 80, 30)
#define DETAIL_RECT_SHIP6_1     QRect(624, 424, 80, 30)
#define DETAIL_RECT_SHIP6_2     QRect(707, 424, 80, 30)
#define DETAIL_RECT_CLOSE       (QRect(320, 170, 24, 280))

//艦隊リスト
#define FLEETLIST_RECT_CAPTURE  (QRect(366, 100, 434, 365))

//提督名と司令部レベル
#define HOME_PORT_CHECK_COLOR   (qRgb(42, 150, 163))
#define HOME_PORT_RECT_CAPTURE  (QRect(300, 3, 490, 4))
#define ADMIRAL_RECT_HEADER     (QRect(124, 8, 146, 18))
#define HQ_LEVEL_RECT_HEADER    (QRect(392, 13, 88, 18))

//戦闘
#define BUTTLE_RECT1            (QRect(0, 0, 400, 15))
#define BUTTLE_RECT2            (QRect(400, 365, 400, 15))
//昼戦
#define BUTTLE_DAYTIME_CHECK_COLOR1     (qRgb(65 , 164 , 209))
#define BUTTLE_DAYTIME_CHECK_COLOR2     (qRgb(104 , 157 , 176))
//夜戦
#define BUTTLE_NIGHT_CHECK_COLOR1     (qRgb(46 , 140 , 182))
#define BUTTLE_NIGHT_CHECK_COLOR2     (qRgb(63 , 88 , 100))

//戦果画面
#define BUTTLE_RESULT_CHECK_COLOR1      (qRgb(30, 50, 52)) //左上のタイトル
#define BUTTLE_RESULT_RECT1             (QRect(0, 10, 300, 60))
#define BUTTLE_RESULT_CHECK_COLOR2      (qRgb(199, 199, 201))  //真ん中の白い線
#define BUTTLE_RESULT_RECT2             (QRect(394, 77, 4, 370))
//大破判定
#define BUTTLE_RESULT_MAJOR_DAMAGE_CHECK_COLOR (qRgb(27, 15, 14))
#define BUTTLE_RESULT_MAJOR_DAMAGE_RECT        (QRect(110, 0, 50, 40))
//進撃or撤退
#define BUTTLE_GO_OR_BACK_CHECK_COLOR   (qRgb(80, 164, 196))
#define BUTTLE_GO_OR_BACK_RECT          (QRect(200, 180, 400, 120))
#define BUTTLE_GO_BUTTON_RECT           (QRect(220, 190, 150, 110))
#define BUTTLE_BACK_BUTTON_RECT         (QRect(435, 190, 150, 110))
//羅針盤を回す
#define BUTTLE_COMPASS_CHECK_COLOR      (qRgb(51 , 24 , 24))
#define BUTTLE_COMPASS_RECT             (QRect(380, 140, 40, 100))

//遠征画面
#define EXPEDITION_CHECK_COLOR          (qRgb(179, 154, 118))
#define EXPEDITION_RECT                 (QRect(305, 100, 250, 25))
//遠征項目領域
#define EXPEDITION_ITEM1_RECT           (QRect(117, 158, 440, 30))
#define EXPEDITION_ITEM2_RECT           (QRect(117, 188, 440, 30))
#define EXPEDITION_ITEM3_RECT           (QRect(117, 218, 440, 30))
#define EXPEDITION_ITEM4_RECT           (QRect(117, 248, 440, 30))
#define EXPEDITION_ITEM5_RECT           (QRect(117, 278, 440, 30))
#define EXPEDITION_ITEM6_RECT           (QRect(117, 308, 440, 30))
#define EXPEDITION_ITEM7_RECT           (QRect(117, 338, 440, 30))
#define EXPEDITION_ITEM8_RECT           (QRect(117, 368, 440, 30))

#define EXPEDITION_ITEM_COMFIRM_RECT    (QRect(626, 424, 104, 40))      //遠征の項目決定ボタンの位置
#define EXPEDITION_ITEM_START_RECT      (QRect(546, 424, 167, 43))      //遠征開始ボタン
#define EXPEDITION_ITEM_START_OUT_RECT  (QRect(0, 0,0, 0))   //遠征出発の有効エリア（コレを外れると戻る）

//遠征開始時の艦隊番号
#define EXPEDITION_SELECT_FLEET_2_RECT  (QRect(381, 108, 23, 20))
#define EXPEDITION_SELECT_FLEET_3_RECT  (QRect(411, 108, 23, 20))
#define EXPEDITION_SELECT_FLEET_4_RECT  (QRect(441, 108, 23, 20))
#define EXPEDITION_SELECT_FLEET_RECT    (QRect(381, 108, 84, 21))

//遠征開始ボタン
#define EXPEDITION_START_BUTTON_RECT    (QRect(546, 424, 167, 43))
#define EXPEDITION_START_BUTTON_COLOR   (qRgb(210 , 157 , 90))

#endif // KANMUSUMEMORY_GLOBAL_H
