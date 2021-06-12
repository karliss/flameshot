// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include "src/widgets/capture/capturetoolbutton.h"
#include "src/widgets/capture/captureconfig.h"
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QVector>

class QFileSystemWatcher;
class ValueHandler;
template<class T>
class QSharedPointer;
class QTextStream;

/**
 * Declare and implement a getter for a config option. `KEY` is the option key
 * as it appears in the config file, `TYPE` is the C++ type. At the same time
 * `KEY` is the name of the generated getter function.
 */
#define CONFIG_GETTER(KEY, TYPE)                                               \
    TYPE KEY() { return value(QStringLiteral(#KEY)).value<TYPE>(); }

/**
 * Declare and implement a setter for a config option. `FUNC` is the name of the
 * generated function, `KEY` is the option key as it appears in the config file
 * and `TYPE` is the C++ type.
 */
#define CONFIG_SETTER(FUNC, KEY, TYPE)                                         \
    void FUNC(const TYPE& value)                                               \
    {                                                                          \
        setValue(QStringLiteral(#KEY), QVariant::fromValue(value));            \
    }

/**
 * Combines the functionality of `CONFIG_GETTER` and `CONFIG_SETTER`. `GETFUNC`
 * is simultaneously the name of the getter function and the option key as it
 * appears in the config file. `SETFUNC` is the name of the setter function.
 * `TYPE` is the C++ type of the value.
 */
#define CONFIG_GETTER_SETTER(GETFUNC, SETFUNC, TYPE)                           \
    CONFIG_GETTER(GETFUNC, TYPE)                                               \
    CONFIG_SETTER(SETFUNC, GETFUNC, TYPE)

class ConfigHandler : public QObject
{
    Q_OBJECT

public:
    explicit ConfigHandler(bool skipInitialErrorCheck = false);

    static ConfigHandler* getInstance();

    // Definitions of getters and setters for config options
    // Some special cases are implemented regularly, without the macro
    // NOTE: When adding new options, make sure to add an entry in
    // recognizedGeneralOptions in the cpp file.
    CONFIG_GETTER_SETTER(userColors, setUserColors, QVector<QColor>);
    CONFIG_GETTER_SETTER(savePath, setSavePath, QString)
    CONFIG_GETTER_SETTER(savePathFixed, setSavePathFixed, bool)
    CONFIG_GETTER_SETTER(uiColor, setUiColor, QColor)
    CONFIG_GETTER_SETTER(contrastUiColor, setContrastUiColor, QColor)
    CONFIG_GETTER_SETTER(drawColor, setDrawColor, QColor)
    CONFIG_GETTER_SETTER(fontFamily, setFontFamily, QString)
    CONFIG_GETTER_SETTER(showHelp, setShowHelp, bool)
    CONFIG_GETTER_SETTER(showSidePanelButton, setShowSidePanelButton, bool)
    CONFIG_GETTER_SETTER(showDesktopNotification,
                         setShowDesktopNotification,
                         bool)
    CONFIG_GETTER_SETTER(filenamePattern, setFilenamePattern, QString)
    CONFIG_GETTER_SETTER(disabledTrayIcon, setDisabledTrayIcon, bool)
    CONFIG_GETTER_SETTER(drawThickness, setDrawThickness, int)
    CONFIG_GETTER_SETTER(drawFontSize, setDrawFontSize, int)
    CONFIG_GETTER_SETTER(keepOpenAppLauncher, setKeepOpenAppLauncher, bool)
    CONFIG_GETTER_SETTER(checkForUpdates, setCheckForUpdates, bool)
    CONFIG_GETTER_SETTER(showStartupLaunchMessage,
                         setShowStartupLaunchMessage,
                         bool)
    CONFIG_GETTER_SETTER(contrastOpacity, setContrastOpacity, int)
    CONFIG_GETTER_SETTER(copyAndCloseAfterUpload,
                         setCopyAndCloseAfterUpload,
                         bool)
    CONFIG_GETTER_SETTER(historyConfirmationToDelete,
                         setHistoryConfirmationToDelete,
                         bool)
    CONFIG_GETTER_SETTER(uploadHistoryMax, setUploadHistoryMax, int)
    CONFIG_GETTER_SETTER(saveAfterCopy, setSaveAfterCopy, bool)
    CONFIG_GETTER_SETTER(copyPathAfterSave, setCopyPathAfterSave, bool)
    CONFIG_GETTER_SETTER(useJpgForClipboard, setUseJpgForClipboard, bool)
    CONFIG_GETTER_SETTER(ignoreUpdateToVersion,
                         setIgnoreUpdateToVersion,
                         QString)
    CONFIG_GETTER_SETTER(undoLimit, setUndoLimit, int)
    CONFIG_GETTER_SETTER(buttons, setButtons, QList<CaptureTool::Type>)

    // SPECIAL CASES
    bool startupLaunch();
    void setStartupLaunch(const bool);
    QString saveAsFileExtension();
    CONFIG_SETTER(setSaveAsFileExtension, setSaveAsFileExtension, QString)
    void setAllTheButtons();
    CaptureConfig::CaptureWindowMode windowMode();
    void setWindowMode(CaptureConfig::CaptureWindowMode);

    // DEFAULTS
    QString filenamePatternDefault();
    void setDefaultSettings();
    QString configFilePath() const;

    // GENERIC GETTERS AND SETTERS
    bool setShortcut(const QString&, const QString&);
    QString shortcut(const QString&);
    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key) const;

    // INFO
    const QSet<QString>& recognizedGeneralOptions() const;
    const QSet<QString>& recognizedShortcutNames() const;
    QSet<QString> keysFromGroup(const QString& group) const;

    // ERROR HANDLING
    bool checkForErrors(QTextStream* log = nullptr) const;
    bool checkUnrecognizedSettings(QTextStream* log = nullptr) const;
    bool checkShortcutConflicts(QTextStream* log = nullptr) const;
    bool checkSemantics(QTextStream* log = nullptr) const;
    void checkAndHandleError() const;
    void setErrorState(bool error) const;
    bool hasError() const;
    QString errorMessage() const;

signals:
    void error() const;
    void errorResolved() const;
    void fileChanged() const;

private:
    mutable QSettings m_settings;

    static bool m_hasError, m_errorCheckPending, m_skipNextErrorCheck;
    static QSharedPointer<QFileSystemWatcher> m_configWatcher;

    void ensureFileWatched() const;
    QSharedPointer<ValueHandler> valueHandler(const QString& key) const;
    void assertKeyRecognized(const QString& key) const;
    bool isShortcut(const QString& key) const;
    QString baseName(QString key) const;
};
