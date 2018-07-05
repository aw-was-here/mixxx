
#include <iostream>

#include "broadcast/networkmanager.h"
#include "broadcast/networkrequest.h"
#include "broadcast/networkreply.h"


NetworkReply* FakeNetworkManager::post(const NetworkRequest *request, const QByteArray &data) {
    NetworkReply *reply = new FakeNetworkReply;
    FakeNetworkReply *fakeReply = qobject_cast<FakeNetworkReply*>(reply);
    fakeReply->setNetworkError(QNetworkReply::NoError);
    fakeReply->setHttpError(200);
    qDebug() << "Fake network manager sending POST request.";
    qDebug() << "Headers:";
    for (auto header : request->rawHeaderList()) {
        qDebug() << header;
    }
    qDebug() << "POST Body:";
    qDebug() << data;
    return reply;
}