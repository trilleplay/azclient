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

#ifndef POWERNOTIFIER_H
#define POWERNOTIFIER_H

#include <QObject>

#ifdef Q_OS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>
#endif

class PowerNotifier : public QObject
{
	Q_OBJECT

public:
	PowerNotifier(QObject *parent = 0);

#ifdef Q_OS_MACOS
	~PowerNotifier();

private:
	io_connect_t m_rootPowerDomain;
	io_object_t m_powerNotifier;
	IONotificationPortRef m_powerNotifyPort;
	static void powerCallback(void *data, io_service_t, natural_t messageType, void *messageArgument);
#endif

signals:
	void aboutToSleep();
	void resumed();
};

#endif // POWERNOTIFIER_H
