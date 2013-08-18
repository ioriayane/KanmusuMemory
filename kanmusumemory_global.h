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

#define KANMEMO_VERSION "0.3"
#define KANMEMO_DEVELOPERS (QStringList() \
    << "@IoriAYANE"\
    << "@task_jp"\
    )

#define SETTING_FILE_NAME       "settings.ini"
#define SETTING_FILE_FORMAT     QSettings::IniFormat

#define SETTING_GENERAL_TOKEN       "token"
#define SETTING_GENERAL_TOKENSECRET "tokenSecret"
#define SETTING_GENERAL_USER_ID     "user_id"
#define SETTING_GENERAL_SCREEN_NAME "screen_name"


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



#endif // KANMUSUMEMORY_GLOBAL_H
