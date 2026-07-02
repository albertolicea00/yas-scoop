#include "scoopadapter.h"

using yas::CliAction;
using yas::CliCommand;
using yas::Package;

// scoop is a PowerShell function, not an executable — every command goes
// through powershell.exe. Package ids/queries are sanitized before being
// embedded in the -Command string (PowerShell would otherwise interpret
// metacharacters).
namespace {

QString sanitize(const QString &token)
{
    QString out;
    out.reserve(token.size());
    for (const QChar c : token) {
        if (c.isLetterOrNumber() || c == QLatin1Char('.') || c == QLatin1Char('_')
            || c == QLatin1Char('-') || c == QLatin1Char('+') || c == QLatin1Char('/')
            || c == QLatin1Char('@') || c == QLatin1Char('*'))
            out.append(c);
    }
    return out;
}

CliCommand scoop(const QStringList &scoopArgs)
{
    QStringList safe;
    for (const QString &arg : scoopArgs)
        safe << sanitize(arg);
    return {QStringLiteral("powershell.exe"),
            {QStringLiteral("-NoProfile"), QStringLiteral("-ExecutionPolicy"),
             QStringLiteral("Bypass"), QStringLiteral("-Command"),
             QStringLiteral("scoop ") + safe.join(QLatin1Char(' '))}};
}

// Scoop prints whitespace-aligned tables:
//   Name  Version  Source ...
//   ----  -------  ------
//   7zip  25.00    main
// Values never contain spaces, so token splitting per row is safe.
QList<QStringList> parseTableRows(const QString &stdOut)
{
    QList<QStringList> rows;
    bool inTable = false;
    const QStringList lines = stdOut.split(QLatin1Char('\n'));
    for (const QString &raw : lines) {
        const QString line = raw.trimmed();
        if (line.startsWith(QStringLiteral("----"))) {
            inTable = true;
            continue;
        }
        if (!inTable || line.isEmpty())
            continue;
        rows.append(line.split(QLatin1Char(' '), Qt::SkipEmptyParts));
    }
    return rows;
}

} // namespace

QString ScoopAdapter::displayName() const { return QStringLiteral("Scoop"); }
QString ScoopAdapter::cliProgram() const { return QStringLiteral("powershell.exe"); }
QStringList ScoopAdapter::cliSearchPaths() const { return {}; }
QStringList ScoopAdapter::cliVersionArguments() const
{
    return {QStringLiteral("-NoProfile"), QStringLiteral("-Command"),
            QStringLiteral("scoop --version")};
}

CliCommand ScoopAdapter::searchCommand(const QString &query) const
{
    return scoop({QStringLiteral("search"), query});
}

CliCommand ScoopAdapter::infoCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("info"), packageId});
}

CliCommand ScoopAdapter::listInstalledCommand() const
{
    return scoop({QStringLiteral("list")});
}

CliCommand ScoopAdapter::listOutdatedCommand() const
{
    return scoop({QStringLiteral("status")});
}

CliCommand ScoopAdapter::installCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("install"), packageId});
}

CliCommand ScoopAdapter::uninstallCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("uninstall"), packageId});
}

CliCommand ScoopAdapter::upgradeCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("update"), packageId});
}

CliCommand ScoopAdapter::upgradeAllCommand() const
{
    return scoop({QStringLiteral("update"), QStringLiteral("*")});
}

CliCommand ScoopAdapter::pinCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("hold"), packageId});
}

CliCommand ScoopAdapter::unpinCommand(const QString &packageId, const QString &) const
{
    return scoop({QStringLiteral("unhold"), packageId});
}

QList<Package> ScoopAdapter::parseSearch(const QString &stdOut) const
{
    // Columns: Name Version Source [Binaries]
    QList<Package> result;
    const auto rows = parseTableRows(stdOut);
    for (const QStringList &row : rows) {
        if (row.isEmpty())
            continue;
        Package p;
        p.id = row.value(0);
        p.name = row.value(0);
        p.version = row.value(1);
        p.source = row.value(2);
        result.append(p);
    }
    return result;
}

QList<Package> ScoopAdapter::parseInfo(const QString &stdOut) const
{
    // "Key : Value" lines.
    Package p;
    const QStringList lines = stdOut.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const qsizetype colon = line.indexOf(QLatin1Char(':'));
        if (colon <= 0)
            continue;
        const QString key = line.left(colon).trimmed().toLower();
        const QString value = line.mid(colon + 1).trimmed();
        if (key == QStringLiteral("name")) { p.id = value; p.name = value; }
        else if (key == QStringLiteral("description")) p.description = value;
        else if (key == QStringLiteral("version")) p.version = value;
        else if (key == QStringLiteral("bucket")) p.source = value;
        else if (key == QStringLiteral("website")) p.homepage = value;
        else if (key == QStringLiteral("installed")) p.installedVersion = value;
    }
    if (p.id.isEmpty())
        return {};
    return {p};
}

QList<Package> ScoopAdapter::parseInstalled(const QString &stdOut) const
{
    // Columns: Name Version Source Updated Info
    QList<Package> result;
    const auto rows = parseTableRows(stdOut);
    for (const QStringList &row : rows) {
        if (row.isEmpty())
            continue;
        Package p;
        p.id = row.value(0);
        p.name = row.value(0);
        p.installedVersion = row.value(1);
        p.version = row.value(1);
        p.source = row.value(2);
        result.append(p);
    }
    return result;
}

QList<Package> ScoopAdapter::parseOutdated(const QString &stdOut) const
{
    // scoop status columns: Name  Installed Version  Latest Version  ...
    QList<Package> result;
    const auto rows = parseTableRows(stdOut);
    for (const QStringList &row : rows) {
        if (row.size() < 3)
            continue;
        Package p;
        p.id = row.value(0);
        p.name = row.value(0);
        p.installedVersion = row.value(1);
        p.version = row.value(2);
        result.append(p);
    }
    return result;
}

QList<CliAction> ScoopAdapter::actionCatalog() const
{
    return {
        {QStringLiteral("update"), tr("Update buckets"),
         tr("Fetch the newest manifests from every bucket"),
         scoop({QStringLiteral("update")}), false, false, true},
        {QStringLiteral("checkup"), tr("Run diagnostics"),
         tr("Check the system for potential scoop problems"),
         scoop({QStringLiteral("checkup")}), false, false, false},
        {QStringLiteral("cleanup"), tr("Clean old versions"),
         tr("Remove outdated versions of all installed apps"),
         scoop({QStringLiteral("cleanup"), QStringLiteral("*")}), false, true, false},
        {QStringLiteral("cache-clear"), tr("Clear download cache"),
         tr("Delete every cached installer download"),
         scoop({QStringLiteral("cache"), QStringLiteral("rm"), QStringLiteral("*")}),
         false, true, false},
        {QStringLiteral("buckets"), tr("List buckets"),
         tr("Show the buckets currently configured"),
         scoop({QStringLiteral("bucket"), QStringLiteral("list")}), false, false, false},
        {QStringLiteral("buckets-known"), tr("List known buckets"),
         tr("Show the well-known buckets you can add"),
         scoop({QStringLiteral("bucket"), QStringLiteral("known")}), false, false, false},
        {QStringLiteral("status"), tr("Show status"),
         tr("Apps with pending updates or missing dependencies"),
         scoop({QStringLiteral("status")}), false, false, false},
        {QStringLiteral("which"), tr("Locate executable"),
         tr("Show the path of an app's executable"),
         scoop({QStringLiteral("which")}), true, false, false},
        {QStringLiteral("depends"), tr("Show dependencies"),
         tr("List everything an app depends on"),
         scoop({QStringLiteral("depends")}), true, false, false},
    };
}
