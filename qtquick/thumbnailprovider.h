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
#include <qquickimageprovider.h>

class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider()
        : QQuickImageProvider(QQuickImageProvider::Image
                              , QQuickImageProvider::ForceAsynchronousImageLoading)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        int width = requestedSize.width() > 0 ? requestedSize.width() : 200;
        int height = requestedSize.height() > 0 ? requestedSize.height() : 120;

        if (size)
            *size = QSize(width, height);

        QImage image(id);
        if (!image.isNull())
            image = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        return image;
    }
};

