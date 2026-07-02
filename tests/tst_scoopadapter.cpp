#include <QTest>

#include "scoopadapter.h"

class TestScoopAdapter : public QObject {
    Q_OBJECT
private slots:
    void searchParsesTable()
    {
        ScoopAdapter adapter;
        const auto packages = adapter.parseSearch(QStringLiteral(
            "Results from local buckets...\n"
            "\n"
            "Name   Version Source Binaries\n"
            "----   ------- ------ --------\n"
            "7zip   25.00   main\n"
            "7ztm   1.1     extras\n"));
        QCOMPARE(packages.size(), 2);
        QCOMPARE(packages.at(0).id, QStringLiteral("7zip"));
        QCOMPARE(packages.at(0).version, QStringLiteral("25.00"));
        QCOMPARE(packages.at(1).source, QStringLiteral("extras"));
    }

    void statusParsesOutdated()
    {
        ScoopAdapter adapter;
        const auto packages = adapter.parseOutdated(QStringLiteral(
            "Name  Installed Version Latest Version Missing Dependencies Info\n"
            "----  ----------------- -------------- -------------------- ----\n"
            "7zip  24.08             25.00\n"));
        QCOMPARE(packages.size(), 1);
        QCOMPARE(packages.at(0).installedVersion, QStringLiteral("24.08"));
        QCOMPARE(packages.at(0).version, QStringLiteral("25.00"));
        QVERIFY(packages.at(0).outdated());
    }

    void commandsGoThroughPowershellAndAreSanitized()
    {
        ScoopAdapter adapter;
        const auto cmd = adapter.installCommand("7zip; rm -rf /", "");
        QCOMPARE(cmd.program, QStringLiteral("powershell.exe"));
        const QString script = cmd.arguments.last();
        QVERIFY(!script.contains(QLatin1Char(';')));
        QVERIFY(script.startsWith(QStringLiteral("scoop install")));
    }

    void infoParsesKeyValue()
    {
        ScoopAdapter adapter;
        const auto packages = adapter.parseInfo(QStringLiteral(
            "Name        : 7zip\n"
            "Description : A multi-format file archiver\n"
            "Version     : 25.00\n"
            "Bucket      : main\n"
            "Website     : https://www.7-zip.org\n"
            "Installed   : 24.08\n"));
        QCOMPARE(packages.size(), 1);
        QCOMPARE(packages.first().homepage, QStringLiteral("https://www.7-zip.org"));
        QCOMPARE(packages.first().installedVersion, QStringLiteral("24.08"));
    }
};

QTEST_MAIN(TestScoopAdapter)
#include "tst_scoopadapter.moc"
