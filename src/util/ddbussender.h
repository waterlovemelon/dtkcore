#ifndef DDBUSSENDER_H
#define DDBUSSENDER_H

#include "dtkcore_global.h"

#include <QObject>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusInterface>

#include <memory>

class LIBDTKCORESHARED_EXPORT DDBusData
{
public:
    DDBusData();

    QString service;
    QString path;
    QString interface;
    QString queryName;
    QDBusConnection connection;
};

class LIBDTKCORESHARED_EXPORT DDBusCaller
{
    friend class DDBusSender;

public:
    QDBusPendingCall call();

    template <typename T>
    DDBusCaller arg(const T &argument);

private:
    explicit DDBusCaller(const QString &method, std::shared_ptr<DDBusData> data);

private:
    std::shared_ptr<DDBusData> m_dbusData;
    QString m_methodName;
    QVariantList m_arguments;
};

template <typename T>
DDBusCaller DDBusCaller::arg(const T &argument)
{
    m_arguments << QVariant::fromValue(argument);

    return *this;
}

class LIBDTKCORESHARED_EXPORT DDBusProperty
{
    friend class DDBusSender;

public:
    QDBusPendingCall get();
    template <typename T>
    QDBusPendingCall set(const T &value);

private:
    explicit DDBusProperty(const QString &property, std::shared_ptr<DDBusData> data);

private:
    std::shared_ptr<DDBusData> m_dbusData;
    QString m_propertyName;
};

template <typename T>
QDBusPendingCall DDBusProperty::set(const T &value)
{
    QDBusInterface iface(m_dbusData->service, m_dbusData->path, QStringLiteral("org.freedesktop.DBus.Properties"), m_dbusData->connection);

    const QVariantList args = { QVariant::fromValue(m_dbusData->interface), QVariant::fromValue(m_propertyName), QVariant::fromValue(QDBusVariant(value)) };

    return iface.asyncCallWithArgumentList(QStringLiteral("Set"), args);
}

class LIBDTKCORESHARED_EXPORT DDBusSender
{
public:
    explicit DDBusSender();

    DDBusSender service(const QString &service);
    DDBusSender interface(const QString &interface);
    DDBusSender path(const QString &path);
    DDBusCaller method(const QString &method);
    DDBusProperty property(const QString &property);

private:
    DDBusSender type(const QDBusConnection::BusType busType);

private:
    std::shared_ptr<DDBusData> m_dbusData;
};

#endif // DDBUSSENDER_H