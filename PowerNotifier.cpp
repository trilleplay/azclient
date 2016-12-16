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

#include "PowerNotifier.h"
#include <QDebug>

#ifdef Q_OS_MACOS
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>

void PowerNotifier::powerCallback(void *data, io_service_t, natural_t messageType, void *messageArgument)
{
	PowerNotifier *that = reinterpret_cast<PowerNotifier *>(data);
	switch (messageType) {
	case kIOMessageCanSystemSleep:
		qDebug() << "System idle and wants to go to sleep";
		IOAllowPowerChange(that->m_rootPowerDomain, (long)messageArgument);
		break;
	case kIOMessageSystemWillSleep:
		qDebug() << "System about to sleep";
		emit that->aboutToSleep();
		IOAllowPowerChange(that->m_rootPowerDomain, (long)messageArgument);
		break;
	case kIOMessageSystemWillPowerOn:
		qDebug() << "System is waking up from sleep.";
		break;
	case kIOMessageSystemHasPoweredOn:
		qDebug() << "System has resumed";
		emit that->resumed();
		break;
	}
}

PowerNotifier::PowerNotifier(QObject *parent) :
	QObject(parent),
	m_powerNotifier(MACH_PORT_NULL),
	m_powerNotifyPort(NULL)
{
	m_rootPowerDomain = IORegisterForSystemPower(this, &m_powerNotifyPort, &PowerNotifier::powerCallback, &m_powerNotifier);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(m_powerNotifyPort), kCFRunLoopCommonModes);
}

PowerNotifier::~PowerNotifier()
{
	CFRunLoopRemoveSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(m_powerNotifyPort), kCFRunLoopCommonModes);
	IODeregisterForSystemPower(&m_powerNotifier);
	IOServiceClose(m_rootPowerDomain);
	IONotificationPortDestroy(m_powerNotifyPort);
}
#else
PowerNotifier::PowerNotifier(QObject *parent) : QObject(parent) { }
#endif
