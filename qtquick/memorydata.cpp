#include "memorydata.h"

MemoryData::MemoryData(QObject *parent) :
    QObject(parent)
{
}

const QString &MemoryData::memoryPath() const
{
    return m_memoryPath;
}

void MemoryData::setMemoryPath(const QString &path)
{
    if(m_memoryPath.compare(path) == 0)
        return;

    m_memoryPath = path;
    emit memoryPathChanged(m_memoryPath);
}

const QString &MemoryData::imagePath() const
{
    return m_imagePath;
}

void MemoryData::setImagePath(const QString &path)
{
    if(m_imagePath.compare(path) == 0)
        return;

    m_imagePath = path;
    emit imagePathChanged(m_imagePath);
}
