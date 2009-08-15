/*
	This file is a part of madfish-webtoolkit project.
	http://code.google.com/p/madfish-webtoolkit/

	(c) 2009 Alexander "Mad Fish" Chehovsky
	See License.txt for licensing information.
*/

/**
	\file OSUtils.h
	\brief Declaration of functions for interaction with OS.
*/

#pragma once
#ifndef _OSUTILS_H
#define	_OSUTILS_H

namespace CoreToolkit
{

/**
	\brief Contains functions for interacting with environment.
*/
class Environment
{
public:
#ifdef WIN32
	static bool terminated;
#endif
	//! Initialize the environment. Called automatically
	static void Init();
	/**
		Check for termination status.
		Program is considered as terminated when Ctrl+C is pressed or
		window is closed or system is shutting down or 
		SIGINT, SIGQUIT or SIGTERM signal arrives.
	*/
	static bool CheckForTermination();
	/**
		Wait for program termination.
	*/
	static void WaitForTermination();
};

}

#endif

