#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include <QFontDatabase>
#include "backend.h"
#include "mclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");
    QFontDatabase::addApplicationFont(":/fonts/fontello.ttf");

    QQmlApplicationEngine engine;

    qmlRegisterType<Backend>("com.mas.pedant", 1, 0, "Backend");
    qmlRegisterType<MClient>("com.mas.mclient", 1, 0, "MClient");

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
