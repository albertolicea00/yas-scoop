#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "appcontroller.h"
#include "bootstrap.h"
#include "thememanager.h"
#include "scoopadapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("YAS"));
    app.setApplicationName(QStringLiteral("yas-scoop"));
    app.setApplicationDisplayName(QStringLiteral("Yet Another Store for Scoop"));

    QQuickStyle::setStyle(QStringLiteral("Basic"));
    yas::loadBundledFonts();

    ScoopAdapter adapter;
    yas::AppController controller(&adapter);
    yas::ThemeManager themeManager;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("App"), &controller);
    engine.rootContext()->setContextProperty(QStringLiteral("YasManager"), &themeManager);
    engine.loadFromModule("YasScoop", "Main");
    return engine.rootObjects().isEmpty() ? 1 : app.exec();
}
