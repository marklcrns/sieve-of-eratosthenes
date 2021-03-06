#include <iostream>
#include <iomanip>
#include <limits>
#include <tuple>
#include <vector>
#include <set>
#include <unistd.h>
#include <windows.h>

///////////////////////////// Function Prototyes ///////////////////////////////

static void show_usage(const std::string&);

void sieve_of_eratosthenes(int, std::set<int>&);
auto prompt_int_input() -> int;
auto prompt_restart() -> bool;
void cls(const HANDLE& hConsole);

template<typename T>
void print_set_by_column(const std::set<T>& set);
void print_console_colors(); // For Windows

/////////////////////////////// Main Execution /////////////////////////////////

int main(int argc, char* argv[])
{
  int input = 0;

  // Arguments handling
  if (argc > 1)
  {
    for (int i = 1; i < argc; ++i)
    {
      std::string arg = std::string(argv[i]);
      if (arg == "-")
      {
        show_usage(argv[0]);
        return 1;
      }

      if (arg == "-h" || arg == "--help")
      {
        show_usage(argv[0]);
        return 0;
      }

      if (arg == "-n" || arg == "--number")
      {
        if (argc > 2 && argc > i + 1)
        {
          std::size_t found = std::string(argv[i + 1]).find_first_not_of("0123456789");
          if (found != std::string::npos)
          {
            show_usage(argv[0]);
            return 1;
          }
          input = static_cast<int>(std::stoi(argv[++i]));
        }
        else
        {
          show_usage(argv[1]);
          return 1;
        }
      }
    }
  }

  std::cout << "Sieve of Eratosthenes\n"
    << "by Mark Lucernas <https://github.com/marklcrns>\n" << std::endl;
  std::cout << "\nPress Enter to continue" << std::endl;;
  std::string dummy;
  std::getline(std::cin, dummy);

  std::vector<int> integers;
  std::set<int> primes;

  HANDLE hStdout;
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

  do
  {
    primes.clear();
    cls(hStdout);

    // Get input and store in vector
    if (input == 0) { input = prompt_int_input(); }

    // Sieve vector of prime numbers and print the result
    sieve_of_eratosthenes(input, primes);
    std::cout << "\n\nAll the primes sieved below:" << std::endl;
    print_set_by_column(primes);

    // Tally
    std::cout << "\n\nTotal number of primes between 1 and "
              << input << " is: " << primes.size() << std::endl;

    input = 0;
  } while (prompt_restart());

  std::cout << "\nBye!" << std::endl;
  sleep(1);
  return 0;
}


////////////////////////// Function Declarations ///////////////////////////////


static void show_usage(const std::string& name)
{
  std::cerr << "Usage: " << name << " [-h] [-n <integer>]\n"
    << "\n"
    << "Options:\n"
    << "\n"
    << "-n --number\tNumber to print all primes up to. Default = 2000\n"
    << "-h --help\tShow usage\n"
    << std::endl;
}


void sieve_of_eratosthenes(int num, std::set<int>& primes)
{
  std::vector<int> integers;

  HANDLE hStdout;
  hStdout= GetStdHandle(STD_OUTPUT_HANDLE);

  for (int i = 1; i <= num; ++i)
  {
    // New line after 10 numbers in a row
    if (i % 10 == 0)
    {
      std::cout << std::endl;
    }

    if (i == 1) // Hide 1
    {
      SetConsoleTextAttribute(hStdout, 0);
      std::cout << std::setw(10) << i;
      continue;
    }

    // Sieve by multiple of primes already found
    bool newPrime = true;
    for (const auto& prime : primes)
    {
      // Assuming the set is sorted, only check if within the range of num
      if (i < prime) { break; }

      if (i % prime == 0)
      {
        newPrime = false;
        break;
      }
    }

    if (newPrime) // Color prime
    {
      primes.insert(i);
      SetConsoleTextAttribute(hStdout, 10);
    }
    else // No color composite
    {
      SetConsoleTextAttribute(hStdout, 8);
    }
    std::cout << std::setw(10) << i;
  }
  // Reset color to white
  SetConsoleTextAttribute(hStdout, 15);
}


auto prompt_int_input() -> int {
  int input;
  std::cout << "\nEnter a integer: ";
  while (!(std::cin >> input))
  {
    std::cin.clear();
    while (std::cin.get() != '\n') continue;
    std::cout << "\nInvalid integer value! " << std::endl;
    std::cout << "\nEnter an integer: ";
  }

  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return input;
}


auto prompt_restart() -> bool {
  char res;
  std::cout << "\nGo again? [Y/n]: ";
  while (!(std::cin >> res)
         || !(res == 'Y' || res == 'y' || res == 'N' || res == 'n'))
  {
    std::cout << "Go again? [Y/n]: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return res == 'Y' || res == 'y';
}


template<typename T>
  void print_set_by_column(const std::set<T>& set)
  {
    int i = 0;
    for (const auto& value : set)
    {
      if (i % 10 == 0) { std::cout << std::endl; }
      std::cout << std::setw(10) << value;
      ++i;
    }
  }


void print_console_colors()
{
  HANDLE handle;
  handle = GetStdHandle(STD_OUTPUT_HANDLE);

  for (short unsigned int k = 0; k <= 255; ++k)
  {
    if (k % 10 == 1) { std::cout << std::endl; }
    SetConsoleTextAttribute(handle, k);
    std::cout << std::setw(10) << k;
  }
  // Reset color to white
  SetConsoleTextAttribute(handle, 15);
}


// Clear screen, works for Windows and Unix
// Ref: https://docs.microsoft.com/en-us/windows/console/clearing-the-screen
void cls(const HANDLE& hConsole)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  SMALL_RECT scrollRect;
  COORD scrollTarget;
  CHAR_INFO fill;

  // Get the number of character cells in the current buffer.
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
  {
    return;
  }

  // Scroll the rectangle of the entire buffer.
  scrollRect.Left = 0;
  scrollRect.Top = 0;
  scrollRect.Right = csbi.dwSize.X;
  scrollRect.Bottom = csbi.dwSize.Y;

  // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
  scrollTarget.X = 0;
  scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

  // Fill with empty spaces with the buffer's default text attribute.
  fill.Char.UnicodeChar = TEXT(' ');
  fill.Attributes = csbi.wAttributes;

  // Do the scroll
  ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

  // Move the cursor to the top left corner too.
  csbi.dwCursorPosition.X = 0;
  csbi.dwCursorPosition.Y = 0;

  SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
}


