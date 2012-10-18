//
//  XSSNMPConfigController.m
//  XsnmpPrefPane
//
//  Created by James Wilson on 7/07/10.
//  Copyright 2010 LithiumCorp Pty Ltd. All rights reserved.
//

#import "XSSNMPConfigController.h"

#import "XSAuthenticatedCommand.h"

#define kPreferencesFile @"/Library/Preferences/com.xsnmp.xsnmp.plist"

@implementation XSSNMPConfigController

- (id) init
{
	self = [super init];
	if (!self) return nil;

	NSDictionary *config = [NSMutableDictionary dictionaryWithContentsOfFile:kPreferencesFile];
	manageConfig = [[config objectForKey:@"manageConfig"] copy];
	agentExtensionEnabled = [[config objectForKey:@"agentExtensionEnabled"] copy];
	snmpCommunity = [[config objectForKey:@"snmpCommunity"] copy];
	
	return self;
}

@synthesize manageConfig, agentExtensionEnabled, snmpCommunity;

- (NSArray *) argumentsForScript
{
	return [NSArray arrayWithObjects:[manageConfig description], snmpCommunity, [agentExtensionEnabled description], nil];
}

- (void) resetValueTimerCallback:(NSTimer *)timer
{
	NSDictionary *userInfo = [timer userInfo];
	NSLog (@"UserInfo is %@", userInfo);
	NSLog (@"resetValueTimerCallback restoring %@ to %@", [userInfo objectForKey:@"key"], [userInfo objectForKey:@"value"]);
	[self setValue:[userInfo objectForKey:@"value"] forKey:[userInfo objectForKey:@"key"]];
}

- (void) setManageConfig:(NSNumber *)value
{
	BOOL previouslyManagingConfig = [manageConfig boolValue];
	BOOL resetValue = NO;
	manageConfig = [value copy];
	
	if ([manageConfig boolValue] && !previouslyManagingConfig)
	{
		/* Switch on config management
		 *
		 * - Move the current config out of the way (backup)
		 * - Write out a new config based on settings/defaults 
		 * - (Re-)start snmpd launch daemon
		 */
		
		NSLog (@"Swiching on!");
		
		if ([XSAuthenticatedCommand runScript:@"enable_config_management" arguments:[self argumentsForScript]])
		{
			resetValue = NO;
			NSLog (@"Script did run");
		}
		else
		{
			resetValue = YES;
			NSLog (@"Sript did NOT run");
		}
	}
	else if (![manageConfig boolValue] && previouslyManagingConfig)
	{
		/* Switch off config management
		 *
		 * - Move current config out of the way (xsnmp-backup)
		 * - Move old config back into place
		 * - (Don't stop snmpd, let the user do that if they want)
		 */
		
		NSLog (@"Switching off");

		if ([XSAuthenticatedCommand runScript:@"disable_config_management" arguments:[self argumentsForScript]])
		{
			resetValue = NO;
			NSLog (@"Script did run");
		}
		else
		{
			resetValue = YES;
			NSLog (@"Sript did NOT run");
		}
	}

	if (resetValue) 
	{
		NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:previouslyManagingConfig], @"value", @"manageConfig", @"key", nil];
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self
									   selector:@selector(resetValueTimerCallback:)
									   userInfo:userInfo 
										repeats:NO];		
		[manageConfig release];
		manageConfig = [[NSNumber numberWithBool:previouslyManagingConfig] copy];
		NSLog (@"Restoring previous value");
	}
}

- (void) setAgentExtensionEnabled:(NSNumber *)value;
{
	BOOL previouslyEnabled = [agentExtensionEnabled boolValue];
	BOOL resetValue = NO;
	agentExtensionEnabled = [value copy];
	
	if ([agentExtensionEnabled boolValue] && !previouslyEnabled)
	{
		/* Switch on agent extension 
		 *
		 * - Check/Add the agentx master statement to config (append)
		 * - (Re-)start snmpd laumch daemon
		 * - (Re-)start xsnmp launch daemon
		 */
		if ([XSAuthenticatedCommand runScript:@"enable_xsnmp_agentx" arguments:[self argumentsForScript]])
		{
			resetValue = NO;
			NSLog (@"Script did run");
		}
		else
		{
			resetValue = YES;
			NSLog (@"Sript did NOT run");
		}
	}
	else if (![agentExtensionEnabled boolValue] && previouslyEnabled)
	{
		/* Switch off the agent extension 
		 *
		 * - (Don't remove the agentx statement -- it might not be there just for us )
		 * - Stop the xsnmp launch daemon 
		 */
		if ([XSAuthenticatedCommand runScript:@"disable_xsnmp_agentx" arguments:[self argumentsForScript]])
		{
			resetValue = NO;
			NSLog (@"Script did run");
		}
		else
		{
			resetValue = YES;
			NSLog (@"Sript did NOT run");
		}		
	}

	if (resetValue) 
	{
		NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:previouslyEnabled], @"value", @"agentExtensionEnabled", @"key", nil];
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self
									   selector:@selector(resetValueTimerCallback:)
									   userInfo:userInfo 
										repeats:NO];
		[agentExtensionEnabled release];
		agentExtensionEnabled = [[NSNumber numberWithBool:previouslyEnabled] retain];
	}
}

- (void) setSnmpCommunity:(NSString *)value
{
	NSString *previousCommunity = [snmpCommunity copy];
	[snmpCommunity release];
	snmpCommunity = [value retain];
	BOOL resetValue = NO;
	
	if (![previousCommunity isEqualToString:snmpCommunity])
	{
		if ([XSAuthenticatedCommand runScript:@"update_config" arguments:[self argumentsForScript]])
		{
			resetValue = NO;
		}
		else
		{
			resetValue = YES;
		}
	}

	if (resetValue)
	{
		NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:previousCommunity, @"value", @"snmpCommunity", @"key", nil];
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self
									   selector:@selector(resetValueTimerCallback:)
									   userInfo:userInfo 
										repeats:NO];
		[snmpCommunity release];
		snmpCommunity = [previousCommunity copy];	
	}
	
	[previousCommunity release];
}

@end
