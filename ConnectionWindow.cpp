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

#include "ConnectionWindow.h"
#include "LogWindow.h"
#include "VpnApi.h"
#include "OpenVpnRunner.h"
#include "StatusIcon.h"
#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QSvgWidget>
#include <QTimer>
#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#if QT_VERSION >= 0x050600
#include <QVersionNumber>
#elif defined(Q_OS_LINUX)
#include "dist/linux/supporting-old-ubuntu-is-a-horrible-experience/qversionnumber_p.h"
#endif

ConnectionWindow::ConnectionWindow(QWidget *parent)
	: QDialog(parent),
	  m_statusIcon(new StatusIcon(this)),
	  m_api(new VpnApi(this)),
	  m_updateGuard(false),
	  m_goingToSleepWhileConnected(false),
	  m_powerNotifier(new PowerNotifier(this))
{
	setWindowTitle(tr("%1 - Connect").arg(qApp->applicationName()));
	setWindowIcon(QIcon(":icons/app.svg"));
#ifdef Q_OS_MACOS
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#endif

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSizeConstraint(QLayout::SetFixedSize);

	m_status = new QLabel;
	m_status->setAlignment(Qt::AlignCenter);
	m_status->setObjectName("status");
	setStatusText();
	layout->addWidget(m_status);

	layout->addWidget(new QSvgWidget(":logo.svg"));

	m_username = new QLineEdit;
	m_username->setText(m_settings.value("LastUsername").toString());
	m_password = new QLineEdit;
	m_password->setEchoMode(QLineEdit::Password);
	m_region = new QComboBox;
	m_region->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_protocol = new QComboBox;
	m_protocol->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	QFormLayout *form = new QFormLayout;
	form->addRow(tr("Username:"), m_username);
	form->addRow(tr("Password:"), m_password);
	form->addRow(tr("Region:"), m_region);
	form->addRow(tr("Protocol:"), m_protocol);
	layout->addLayout(form);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
	m_connect = buttons->button(QDialogButtonBox::Ok);
	m_connect->setText(tr("&Connect"));
	layout->addWidget(buttons);

	setLayout(layout);

	connect(m_region, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int) {
		const QVariantList &protocols = m_region->currentData().toList();
		if (!protocols.count())
			return;
		m_protocol->clear();
		for (const QVariant &item : protocols) {
			const QVariantMap &protocol = item.toMap();
			const QString &name = protocol["name"].toString();
			const QString &path = protocol["url"].toString();
			if (name.isEmpty() || path.isEmpty())
				continue;
			m_protocol->addItem(name, path);
		}
		int saved = m_protocol->findText(m_settings.value("LastProtocol").toString());
		if (saved >= 0)
			m_protocol->setCurrentIndex(saved);
		validateFields();
	});
	connect(m_region, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConnectionWindow::validateFields);
	connect(m_username, &QLineEdit::textChanged, this, &ConnectionWindow::validateFields);
	connect(m_password, &QLineEdit::textChanged, this, &ConnectionWindow::validateFields);


	connect(m_connect, &QPushButton::clicked, this, [=]() {
		setStatusText(tr("Downloading configuration..."));
		setEnabled(false);
		m_settings.setValue("LastRegion", m_region->currentText());
		m_settings.setValue("LastProtocol", m_protocol->currentText());
		m_settings.setValue("LastUsername", m_username->text());
		m_api->ovpnConfig(this, m_protocol->currentData().toString(), [=](const QByteArray &data) {
			setStatusText();
			startOpenVpn(data);
		});
	});

	connect(m_powerNotifier, &PowerNotifier::resumed, this, [=]() {
		if (!m_goingToSleepWhileConnected)
			return;
		m_goingToSleepWhileConnected = false;
		if (m_lastUsername.isEmpty() || m_lastPassword.isEmpty())
			return;
		m_username->setText(m_lastUsername);
		m_password->setText(m_lastPassword);
		m_connect->click();
	});

	populateRegions();

	QTimer::singleShot(1000 * 10, this, &ConnectionWindow::checkForUpdates);
}

void ConnectionWindow::populateRegions()
{
	regionsLoading();
	m_api->locations(this, [=](const QVariantList &locations, const QString &minVersion) {
		if (!minVersion.isEmpty() && QVersionNumber::fromString(minVersion) > QVersionNumber::fromString(qApp->applicationVersion())) {
			setStatusText(tr("Client is out of date. Please update."));
			setEnabled(false);
			m_region->clear();
			m_protocol->clear();
			m_settings.remove("IgnoredVersion");
			checkForUpdates();
			qCritical() << "Client is out of date. Immediate update required.";
			return;
		}
		bool foundOne = false;
		m_region->clear();
		for (const QVariant &item : locations) {
			const QVariantMap &location = item.toMap();
			const QString &name = location["name"].toString();
			if (name.isEmpty())
				continue;
			foundOne = true;
			m_region->addItem(name, location["endpoints"]);
		}
		if (foundOne) {
			int saved = m_region->findText(m_settings.value("LastRegion").toString());
			if (saved >= 0)
				m_region->setCurrentIndex(saved);
			m_region->setEnabled(true);
			m_protocol->setEnabled(true);
			setStyleSheet(styleSheet());
			validateFields();
		}
		else {
			qCritical() << "Response Error:" << "No regions in locations endpoint.";
			regionsLoading();
			QTimer::singleShot(5000, this, &ConnectionWindow::populateRegions);
		}
	});
}

void ConnectionWindow::regionsLoading()
{
	m_region->clear();
	m_protocol->clear();
	m_connect->setEnabled(false);
	m_protocol->setEnabled(false);
	m_protocol->addItem(tr("Loading..."));
	m_region->setEnabled(false);
	m_region->addItem(tr("Loading..."));
	setStyleSheet(styleSheet());
}

void ConnectionWindow::validateFields()
{
	m_connect->setEnabled(!m_username->text().isEmpty() && !m_password->text().isEmpty() && !m_protocol->currentData().toString().isEmpty());
}

void ConnectionWindow::startOpenVpn(const QByteArray &config)
{
	if (config.length() == 0) {
		setStatusText();
		setEnabled(true);
		LogWindow::instance().show();
		return;
	}

	OpenVpnRunner *runner = new OpenVpnRunner(this);
	connect(runner, &OpenVpnRunner::transfer, m_statusIcon, &StatusIcon::setTransfer);
	connect(runner, &OpenVpnRunner::disconnected, this, [=]() {
		m_api->reinitConnection();
		show();
		setEnabled(true);
		m_statusIcon->setStatus(StatusIcon::Disconnected);
		setStatusText(runner->disconnectReason());
	});
	connect(runner, &OpenVpnRunner::connected, this, [=]() {
		m_api->reinitConnection();
		hide();
		m_password->clear();
		setStatusText();
		m_statusIcon->setStatus(StatusIcon::Connected);
	});
	connect(runner, &OpenVpnRunner::connecting, this, [=]() {
		m_api->reinitConnection();
		setStatusText(tr("Connecting..."));
		m_statusIcon->setStatus(StatusIcon::Connecting);
	});
	connect(m_statusIcon, &StatusIcon::disconnect, runner, &OpenVpnRunner::disconnect);
	connect(m_powerNotifier, &PowerNotifier::aboutToSleep, runner, [=]() {
		m_goingToSleepWhileConnected = true;
		runner->disconnect();
	});

	m_lastUsername = m_username->text();
	m_lastPassword = m_password->text();

	if (!runner->connect(config, m_lastUsername, m_lastPassword)) {
		show();
		setEnabled(true);
		setStatusText();
		LogWindow::instance().show();
	}
}

void ConnectionWindow::checkForUpdates()
{
	if (m_updateGuard)
		return;
	m_api->checkForUpdates(this, [=](const QString &newVersion, const QString &url) {
		if (m_updateGuard)
			return;
		if (newVersion.isEmpty() || url.isEmpty()) {
			QTimer::singleShot(1000 * 60 * 30, this, &ConnectionWindow::checkForUpdates);
			return;
		}
		if (QVersionNumber::fromString(newVersion) <= QVersionNumber::fromString(qApp->applicationVersion()) || newVersion == m_settings.value("IgnoredVersion").toString()) {
			QTimer::singleShot(1000 * 60 * 10, this, &ConnectionWindow::checkForUpdates);
			return;
		}
		QMessageBox question(QMessageBox::Question, tr("Update Available"), tr("An update to %1 is available.\n\nWould you like to download it now?").arg(qApp->applicationName()), QMessageBox::Yes | QMessageBox::Ignore, this);
		question.setButtonText(QMessageBox::Yes, tr("&Download Now"));
		question.setButtonText(QMessageBox::Ignore, tr("Ignore this Update"));
		question.setDefaultButton(QMessageBox::Yes);
		m_updateGuard = true;
		int response = question.exec();
		m_updateGuard = false;
		if (response == QMessageBox::Ignore)
			m_settings.setValue("IgnoredVersion", newVersion);
		else if (response == QMessageBox::Yes) {
			QDesktopServices::openUrl(url);
			QTimer::singleShot(1000 * 60 * 60, this, &ConnectionWindow::checkForUpdates);
			return;
		}
		QTimer::singleShot(1000 * 60 * 10, this, &ConnectionWindow::checkForUpdates);
	});
}

void ConnectionWindow::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}

void ConnectionWindow::setStatusText(const QString &text)
{
	if (text.isEmpty()) {
		m_status->clear();
		m_status->setVisible(false);
	} else {
		m_status->setText(text);
		m_status->setVisible(true);
	}
}
