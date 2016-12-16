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

#import <Cocoa/Cocoa.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>

int main(int argc, char *argv[])
{
	OSStatus status;
	AuthorizationRef authRef;
	AuthorizationItem right = { "com.azirevpn.azclient", 0, NULL, 0 };
	AuthorizationRights rightSet = { 1, &right };
	FILE *ioPipe;
	char buf[1024];
	int ret = 1;
	ssize_t len;
	if (AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef) != errAuthorizationSuccess)
		status = errAuthorizationBadAddress;
	else
		status = AuthorizationCopyRights(authRef, &rightSet, kAuthorizationEmptyEnvironment,
										 kAuthorizationFlagDefaults | kAuthorizationFlagPreAuthorize
										 | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagExtendRights,
										 NULL);
	if (status != errAuthorizationSuccess)
		goto out;
	status = AuthorizationExecuteWithPrivileges(authRef, argv[1], 0, &argv[2], &ioPipe);
	if (status != errAuthorizationSuccess)
		goto out;
	while ((len = fread(buf, sizeof(char), sizeof(buf), ioPipe)) > 0)
		fwrite(buf, sizeof(char), len, stdout);
	while (wait(NULL) == -1);
	ret = 0;
out:
	AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);
	return ret;
}
