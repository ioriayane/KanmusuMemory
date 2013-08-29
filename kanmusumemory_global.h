/*
 * Copyright 2013 KanMemo Project.
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

#define KANMEMO_VERSION "0.5"
#define KANMEMO_DEVELOPERS (QStringList() \
    << "@IoriAYANE"\
    << "@task_jp"\
    << "@nagakenjs"\
    << "@androidsola"\
    << "@tkhshmsy"\
    )

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

#define SETTING_GENERAL_TOKEN       "token"
#define SETTING_GENERAL_TOKENSECRET "tokenSecret"
#define SETTING_GENERAL_USER_ID     "user_id"
#define SETTING_GENERAL_SCREEN_NAME "screen_name"
#define SETTING_GENERAL_UNUSED_TWITTER  "unused_twitter"
#define SETTING_GENERAL_SAVE_PNG    "save_png"
#define SETTING_GENERAL_MASK_ADMIRAL_NAME   "mask_admiral_name"
#define SETTING_GENERAL_MASK_HQ_LEVEL       "mask_hq_level"

#define SETTING_MAINWINDOW          "mainwindow"
#define SETTING_TIMERDIALOG         "timerdialog"
#define SETTING_WINDOW_GEO          "geometry"
#define SETTING_WINDOW_STATE        "windowState"

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

#define DETAIL_CHECK_COLOR1     (qRgb(210, 185, 100))
#define DETAIL_CHECK_COLOR2     (qRgb(135, 185, 130))
#define DETAIL_RECT_CAPTURE     (QRect(325, 100, 465, 370))
#define DETAIL_RECT_SHIP1       (QRect(300, 200, 50, 20))
#define DETAIL_RECT_SHIP1_2     (QRect(380, 200, 50, 20))
#define DETAIL_RECT_SHIP2       (QRect(640, 200, 50, 20))
#define DETAIL_RECT_SHIP2_2     (QRect(720, 200, 50, 20))
#define DETAIL_RECT_SHIP3       (QRect(300, 315, 50, 20))
#define DETAIL_RECT_SHIP3_2     (QRect(380, 315, 50, 20))
#define DETAIL_RECT_SHIP4       (QRect(640, 315, 50, 20))
#define DETAIL_RECT_SHIP4_2     (QRect(720, 315, 50, 20))
#define DETAIL_RECT_SHIP5       (QRect(300, 430, 50, 20))
#define DETAIL_RECT_SHIP5_2     (QRect(380, 430, 50, 20))
#define DETAIL_RECT_SHIP6       (QRect(640, 430, 50, 20))
#define DETAIL_RECT_SHIP6_2     (QRect(720, 430, 50, 20))

#endif // KANMUSUMEMORY_GLOBAL_H
