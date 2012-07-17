#include "NCursesUserInterface.h"

#include <cassert>
#include <cstdarg>
#include <poll.h>

/**
 * @file server/NCursesUserInterface.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the readline implementation for the user command line
 * interface.
 */

namespace userinterface_errors
{
	NCursesError::NCursesError(std::string const &message)
		: Failure("ncurses error: " + message)
	{
	}
}

NCursesUserInterface::NCursesUserInterface()
	: deinitialized_(true),
	  current_position_(0)
{
	using userinterface_errors::NCursesError;

	/**
	 * NCursesFree is a class which only exists to free the resources
	 * allocated by ncurses in this scope.
	 * Even if this implementation throws an error the constructor
	 * of this class is called and the resources properly freed.
	 */
	struct NCursesFree
	{
		NCursesFree()
			: needs_free(true),
			  screen(0),
			  input_window(0)
		{
		}

		~NCursesFree()
		{
			if (!needs_free)
			{
				return;
			}

			if (input_window)
			{
				delwin(input_window);
			}

			endwin();

			if (screen)
			{
				delscreen(screen);
			}
		}

		bool needs_free;
		SCREEN *screen;
		WINDOW *input_window;
	};

	NCursesFree ncurses_free_object;

	screen_ = ncurses_free_object.screen = newterm(0, stdout, stdin);

	if (!screen_ || cbreak() == ERR)
	{
		throw NCursesError("failed to initialize ncurses");
	}

	assert(noecho() == OK); // only fails if no active screen
	assert(nonl() == OK); // only fails if no active screen

	if (keypad(stdscr, TRUE) == ERR)
	{
		throw NCursesError("failed to enable keypad support");
	}

	// screen region is two lines short because of input window
	if (setscrreg(0, LINES - 2) == ERR)
	{
		throw NCursesError("failed to set screen region");
	}

	// don't move cursor here
	if (leaveok(stdscr, TRUE) == ERR)
	{
		throw NCursesError("failed to leave cursor be");
	}

	input_window_ = ncurses_free_object.input_window = newwin(1, COLS, LINES - 1, 0);
	assert(scrollok(stdscr, TRUE) == OK); // only fails if no active window
	assert(scrollok(input_window_, TRUE) == OK); // only fails if no active window


	if (!input_window_ || wmove(input_window_, 0, 0) == ERR)
	{
		throw NCursesError("failed to initialize input ncurses window");
	}

	if (wnoutrefresh(stdscr) == ERR || wnoutrefresh(input_window_) == ERR || doupdate() == ERR)
	{
		throw NCursesError("failed to refresh windows");
	}

	ncurses_free_object.needs_free = false;
	deinitialized_ = false;
}

NCursesUserInterface::~NCursesUserInterface()
{
	deinitialize();
}

void NCursesUserInterface::run()
{
try
{
	using userinterface_errors::NCursesError;

	for (;;)
	{
		static ::pollfd fd { 0, POLLIN, 0 };

		switch (::poll(&fd, 1, 10))
		{
			case 0:
				// timeout
				if (quit_requested_)
				{
					printf("Press any key to exit\n");

					wtimeout(input_window_, -1);

					if (getch() == ERR)
					{
						throw NCursesError("fetching user input failed");
					}

					return;
				}

				continue;

			case -1:
				throw NCursesError("polling stdin failed");

			default:
				if (fd.revents & POLLIN)
				{
					break;
				}

				throw NCursesError("polling stdin failed");
		}

		wint_t input;

		if (get_wch(&input) == ERR)
		{
			throw NCursesError("fetching user input failed");
		}

		switch (input)
		{
			case L'\n':
			case L'\r':
			{
				current_position_ = 0;
				wclear(input_window_);

				try
				{
					process_line();
				}
				catch (...)
				{
					current_line_.clear();
					throw;
				}

				current_line_.clear();

				break;
			}

			case L'\b':
			case 0x7F: // DEL
			case KEY_BACKSPACE:
				if (current_position_ == 0)
				{
					break;
				}

				current_line_.erase(current_position_ - 1, 1);
				current_position_--;

				wmove(input_window_, 0, 0);
				wclrtoeol(input_window_);
				mvwprintw(input_window_, 0, 0, "%ls", current_line_.c_str());
				wmove(input_window_, 0, current_position_);

				break;

			case KEY_UP:
			case KEY_DOWN:
				// ignore key up/down

				break;

			case KEY_LEFT:
				if (current_position_ != 0)
				{
					current_position_--;
					wmove(input_window_, 0, current_position_);
				}

				break;

			case KEY_RIGHT:
				if (current_position_ != current_line_.length())
				{
					current_position_++;
					wmove(input_window_, 0, current_position_);
				}

				break;

			case KEY_DC:
				if (current_position_ == current_line_.length())
				{
					break;
				}

				current_line_.erase(current_position_, 1);

				wmove(input_window_, 0, 0);
				wclrtoeol(input_window_);
				mvwprintw(input_window_, 0, 0, "%ls", current_line_.c_str());
				wmove(input_window_, 0, current_position_);

				break;

			case KEY_END:
				current_position_ = current_line_.length();
				wmove(input_window_, 0, current_line_.length());

				break;

			case KEY_HOME:
				current_position_ = 0;
				wmove(input_window_, 0, 0);

				break;

			default:
				current_line_.insert(current_position_, 1, input);
				current_position_++;

				wprintw(input_window_, "%lc", input);
				wmove(input_window_, 0, current_position_);
				break;
		}

		wnoutrefresh(stdscr);
		wnoutrefresh(input_window_);
		doupdate();
	}
}
catch (...)
{
	deinitialize();
	throw;
}
}

void NCursesUserInterface::deinitialize()
{
	if (!deinitialized_)
	{
		delwin(input_window_);
		endwin();
		delscreen(screen_);
		deinitialized_ = true;
	}
}

void NCursesUserInterface::printfv(char const *format, ...)
{
try
{
	printf_mutex_.lock();

	va_list list;
	va_start(list, format);

	if (!deinitialized_)
	{
		vw_printw(stdscr, format, list);
		wnoutrefresh(stdscr);
		wnoutrefresh(input_window_);
		doupdate();
	}
	else
	{
		vfprintf(stderr, format, list);
	}

	va_end(list);

	printf_mutex_.unlock();
}
catch (...)
{
	deinitialize();
	throw;
}
}
