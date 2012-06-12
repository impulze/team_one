#ifndef NCURSESUSERINTERFACE_TCC_INCLUDED
#define NCURSESUSERINTERFACE_TCC_INCLUDED

/**
 * @file NCursesUserInterface.tcc
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Template implementations for the NCurses user command line interface.
 */

template <class... T>
void NCursesUserInterface::printf(std::string const &format, T &&... args)
{
	printw(format.c_str(), std::forward<T>(args)...);
	refresh();
}

#endif
