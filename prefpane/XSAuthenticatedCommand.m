//
//  XSAuthenticatedCommand.m
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "XSAuthenticatedCommand.h"
#import "XsnmpPrefPanePref.h"

@interface XSAuthenticatedCommand (Private)
- (void) informDelegteHelperFinished;
- (void) informDelegteHelperFailed;
- (BOOL) processDataFromHelper:(NSString *)data;
@end

@implementation XSAuthenticatedCommand

#pragma mark "Generic Methods

+ (XSAuthenticatedCommand *) runScript:(NSString *)scriptName arguments:(NSArray *)arguments
{
	XSAuthenticatedCommand *authCommand = [[XSAuthenticatedCommand alloc] init];
	if ([authCommand runScript:scriptName arguments:arguments]) 
	{
		return [authCommand autorelease];
	}
	else
	{
		[authCommand release];
		return nil;
	}
}

- (BOOL) runScript:(NSString *)scriptName arguments:(NSArray *)arguments
{
	NSLog (@"runScript has args %@", arguments);
	
	OSStatus err = 0;
	if (!authRef)
	{ err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef); }
	if (err == 0)
	{
		/* Setup arguments array */
		char *args[[arguments count]+3];
		args[0] = (char *) [[[XsnmpPrefPanePref bundle] pathForResource:@"xsnmp_helper.rb" ofType:nil] cStringUsingEncoding:NSUTF8StringEncoding];
		NSLog(@"Argv0 = %s", args[0]);
		args[1] = (char *) [scriptName cStringUsingEncoding:NSUTF8StringEncoding];
		NSLog(@"Argv1 = %s", args[1]);
		int i;
		for (i=0; i < [arguments count]; i++)
		{
			NSString *argument = [arguments objectAtIndex:i];
			args[i+2] = (char *) [argument cStringUsingEncoding:NSUTF8StringEncoding];
			NSLog(@"Argv%i = %s (%@)", i+2, args[i+2], argument);
			
		}
		for (NSString *argument in arguments)
		{
		}
		args[[arguments count]+2] = NULL;
		NSLog(@"Argv%lu = NULL", (unsigned long)[arguments count]+2);
						
		char *helperPath = (char *) [[[XsnmpPrefPanePref bundle] pathForAuxiliaryExecutable:@"XsnmpHelperTool"] cStringUsingEncoding:NSUTF8StringEncoding];
		NSLog (@"Helper path is %s (%@)", helperPath, [[XsnmpPrefPanePref bundle] bundlePath]);
				
		err = AuthorizationExecuteWithPrivileges(authRef,helperPath,kAuthorizationFlagDefaults,args,&pipe);	
		if (err == errAuthorizationSuccess)
		{
			/* Authorized and executed */
			readHandle = [[NSFileHandle alloc] initWithFileDescriptor:fileno(pipe)];
			[[NSNotificationCenter defaultCenter] addObserver:self
													 selector:@selector(dataToBeReadFromHelper:)
														 name:NSFileHandleReadCompletionNotification
													   object:readHandle];	
			[readHandle readInBackgroundAndNotify];
		}
		else
		{ 
			/* Failed to authorize or execute */
			[self informDelegteHelperFailed];
			return NO;
		}
	}
	
	return YES;
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];

	if (authRef)
	{ AuthorizationFree(authRef,kAuthorizationFlagDefaults); }
	[readHandle release];
	[super dealloc];
}

- (BOOL) processDataFromHelper:(NSString *)data
{
	/* Return YES to keep reading */
	/* Return NO to suppress further read */
	return YES;
}

- (void) dataToBeReadFromHelper:(NSNotification *)notification
{
	NSDictionary *resultDictionary = [notification userInfo];
	NSData *readData = [resultDictionary objectForKey:NSFileHandleNotificationDataItem];
	
	NSString *string = [[[NSString alloc] initWithData:readData encoding:NSUTF8StringEncoding] autorelease];
	if (string && [string length] > 0)
	{
		/* Interpret */
		BOOL keepReading = [self processDataFromHelper:string];
		if (keepReading)
		{ [readHandle readInBackgroundAndNotify]; }
	}
	else
	{
		/* Failed to read! */
		[self informDelegteHelperFailed];
	}
	
	NSLog (@"Read %@", string);
}

#pragma mark "Delegate Methods"

@synthesize delegate;

- (void) informDelegteHelperFinished
{
	if ([delegate respondsToSelector:@selector(commandDidFinish:)])
	{ [delegate performSelectorOnMainThread:@selector(commandDidFinish:) withObject:self waitUntilDone:YES]; }
}

- (void) informDelegteHelperFailed
{
	if ([delegate respondsToSelector:@selector(commandDidFail:)])
	{ [delegate performSelectorOnMainThread:@selector(commandDidFail:) withObject:self waitUntilDone:YES]; }
}

@end
