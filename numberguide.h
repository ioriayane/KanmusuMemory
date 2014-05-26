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
#ifndef NUMBERGUIDE_H
#define NUMBERGUIDE_H

#include <QString>
#include <QImage>

//数字を判定するための情報を管理するクラス
class NumberGuide{
public:
    NumberGuide(QString path, int number, int border){
        image = QImage(path);
        this->number = number;
        this->border = border;
    }
    QImage image;
    int number;
    int border;
};

#endif // NUMBERGUIDE_H
