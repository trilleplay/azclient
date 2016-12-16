/*
 * Copyright (C) 2017 Nessla AB
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "LogWindow.h"
#include "platform-constants.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QCloseEvent>
#include <QIcon>
#include <QPushButton>
#include <QThread>
#include <QFileDialog>
#include <QMetaMethod>
#include <QApplication>
#include <QDateTime>
#include <QTimeZone>
#include <QFile>
#include <QDebug>

static QtMessageHandler previousHandler = nullptr;

static void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	LogWindow::instance().appendLine(msg);
	if (previousHandler)
		previousHandler(type, context, msg);
}

LogWindow& LogWindow::instance()
{
	static LogWindow instance;
	if (!previousHandler)
		previousHandler = qInstallMessageHandler(logMessage);
	return instance;
}

void LogWindow::appendLine(const QString &str)
{
	if (qApp->closingDown())
		return;
	if (QThread::currentThread() != thread()) {
		static int methodIndex = metaObject()->indexOfMethod("appendLine(QString)");
		static QMetaMethod method = metaObject()->method(methodIndex);
		method.invoke(this, Qt::QueuedConnection, Q_ARG(QString, str));
		return;
	}
	QDateTime now(QDateTime::currentDateTimeUtc());
	now = now.toOffsetFromUtc(QTimeZone(QTimeZone::systemTimeZoneId()).offsetFromUtc(now));
	m_text->appendPlainText(now.toString(Qt::ISODate).toUtf8() + " | " + str);
	m_text->ensureCursorVisible();
}

LogWindow::LogWindow(QDialog *parent) :
	QDialog(parent)
{
	setWindowTitle(tr("%1 - Message Log").arg(qApp->applicationName()));
	setWindowIcon(QIcon(":icons/" PLATFORM_ICONS "/log-view.svg"));
#ifdef Q_OS_MACOS
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif

	QVBoxLayout *layout = new QVBoxLayout;
	m_text = new QPlainTextEdit;
	m_text->setReadOnly(true);
	QFont font("Courier");
	font.setStyleHint(QFont::TypeWriter);
	m_text->setFont(font);
	layout->addWidget(m_text);
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Save | QDialogButtonBox::Reset);
	buttons->button(QDialogButtonBox::Close)->setText(tr("&Hide Log Window"));
	buttons->button(QDialogButtonBox::Save)->setText(tr("&Save Log to File"));
	buttons->button(QDialogButtonBox::Reset)->setText(tr("&Clear Log"));
	connect(buttons->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &QWidget::hide);
	connect(buttons->button(QDialogButtonBox::Reset), &QPushButton::clicked, m_text, &QPlainTextEdit::clear);
	connect(buttons->button(QDialogButtonBox::Save), &QPushButton::clicked, this, &LogWindow::saveLog);
	layout->addWidget(buttons);
	setLayout(layout);
	resize(640, 480);
	connect(qApp, &QApplication::aboutToQuit, this, &QWidget::hide);
}

void LogWindow::saveLog()
{
	const QString fileName = QFileDialog::getSaveFileName(this, tr("Select the log file path"), QString(), "Logs (*.log)", nullptr, 0);
	if (fileName.isEmpty())
		return;
	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		qCritical() << "Log File Write Error" << f.errorString();
		return;
	}
	f.write(m_text->toPlainText().toLocal8Bit());
	f.close();
}

void LogWindow::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}
