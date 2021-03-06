#ifndef CMDLINEARGS_H
#define CMDLINEARGS_H

/*
 *      Copyright (C) 2004-2009 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifdef _LINUX
#include "PlatformDefs.h"
#include "xwinapi.h"
typedef LPSTR PSZ;
#define _snprintf snprintf
#else
#include <windows.h>
#endif
#include <vector>
#include <string>

class CmdLineArgs : public std::vector<char*>
{
public:
    CmdLineArgs ()
    {
        // Save local copy of the command line string, because
        // ParseCmdLine() modifies this string while parsing it.
        PSZ cmdline = GetCommandLine();
        m_cmdline = new char [strlen (cmdline) + 1];
        if (m_cmdline)
        {
            strcpy (m_cmdline, cmdline);
            ParseCmdLine(); 
        } else {
#ifdef _LINUX
          delete[] cmdline;
#endif
        }
    }

    CmdLineArgs (const int argc, const char **argv)
    {
	std::string cmdline;
	for (int i = 0 ; i<argc ; i++)
	{
	    cmdline += std::string(argv[i]);
	    if ( i != (argc-1) )
	    {
		cmdline += " ";
	    }
	}
	m_cmdline = new char [cmdline.length() + 1];
	if (m_cmdline)
	{
	    strcpy(m_cmdline, cmdline.c_str());
	    ParseCmdLine();
	}
    }

    ~CmdLineArgs()
    {
        delete[] m_cmdline;
    }

private:
    PSZ m_cmdline; // the command line string

    ////////////////////////////////////////////////////////////////////////////////
    // Parse m_cmdline into individual tokens, which are delimited by spaces. If a
    // token begins with a quote, then that token is terminated by the next quote
    // followed immediately by a space or terminator.  This allows tokens to contain
    // spaces.
    // This input string:     This "is" a ""test"" "of the parsing" alg"o"rithm.
    // Produces these tokens: This, is, a, "test", of the parsing, alg"o"rithm
    ////////////////////////////////////////////////////////////////////////////////
    void ParseCmdLine ()
    {
        enum { TERM  = '\0',
               QUOTE = '\"' };

        bool bInQuotes = false;
        PSZ pargs = m_cmdline;

        while (*pargs)
        {
            while (isspace (*pargs))        // skip leading whitespace
                pargs++;

            bInQuotes = (*pargs == QUOTE);  // see if this token is quoted

            if (bInQuotes)                  // skip leading quote
                pargs++; 

            push_back (pargs);              // store position of current token

            // Find next token.
            // NOTE: Args are normally terminated by whitespace, unless the
            // arg is quoted.  That's why we handle the two cases separately,
            // even though they are very similar.
            if (bInQuotes)
            {
                // find next quote followed by a space or terminator
                while (*pargs && 
                      !(*pargs == QUOTE && (isspace (pargs[1]) || pargs[1] == TERM)))
                    pargs++;
                if (*pargs)
                {
                    *pargs = TERM;  // terminate token
                    if (pargs[1])   // if quoted token not followed by a terminator
                        pargs += 2; // advance to next token
                }
            }
            else
            {
                // skip to next non-whitespace character
                while (*pargs && !isspace (*pargs)) 
                    pargs++;
                if (*pargs && isspace (*pargs)) // end of token
                {
                   *pargs = TERM;    // terminate token
                    pargs++;         // advance to next token or terminator
                }
            }
        } // while (*pargs)
    } // ParseCmdLine()
}; // class CmdLineArgs


#endif // CMDLINEARGS_H
