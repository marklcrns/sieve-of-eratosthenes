#include <iostream>
#include <iomanip>
#include <limits>
#include <tuple>
#include <vector>
#include <set>
#include <unistd.h>
#include <windows.h>

///////////////////////////// Function Prototyes ///////////////////////////////

static void show_usage(const std::string& name);

void sieve_of_eratosthenes(const std::vector<int>& vec, std::set<int>& primes);
auto prompt_int_input() -> long unsigned int;
auto prompt_restart() -> bool;
void cls(const HANDLE& hConsole);

template<typename T>
void print_set_by_column(const std::set<T>& set);
void print_console_colors(); // For Windows

/////////////////////////////// Main Execution /////////////////////////////////

int main(int argc, char* argv[])
{
  long unsigned int input = 0;

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
          input = static_cast<long unsigned int>(std::stoi(argv[++i]));
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
    integers.clear();
    primes.clear();
    cls(hStdout);

    // Get input and store in vector
    if (input == 0) { input = prompt_int_input(); }

    integers.resize(input);
    for (long unsigned int i = 0; i < input; ++i) { integers[i] = (int)i + 1; }

    // Sieve vector of prime numbers and print the result
    sieve_of_eratosthenes(integers, primes);
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
  std::cerr << "Usage: " << name << " [-h] [-c] [-p <int-piles>] [-s <int-stones>]\n"
    << "\n"
    << "Options:\n"
    << "\n"
    << "-n --number\tNumber to print all primes up to. Default = 2000\n"
    << "-h --help\tShow usage\n"
    << "\n"
    << std::endl;
}


void sieve_of_eratosthenes(const std::vector<int>& nums, std::set<int>& primes)
{
  HANDLE hStdout;
  hStdout= GetStdHandle(STD_OUTPUT_HANDLE);

  for (size_t i = 0; i < nums.size(); ++i)
  {
    // New line after 10 numbers in a row
    if (i % 10 == 0)
    {
      std::cout << std::endl;
    }

    int num = nums.at(i);
    if (num == 1) // Hide 1
    {
      SetConsoleTextAttribute(hStdout, 0);
      std::cout << std::setw(10) << num;
      continue;
    }

    // Sieve by multiple of primes already found
    bool newPrime = true;
    for (const auto& prime : primes)
    {
      // Assuming the set is sorted, only check if within the range of num
      if (num < prime) { break; }

      if (num % prime == 0)
      {
        newPrime = false;
        break;
      }
    }

    if (newPrime) // Color prime
    {
      primes.insert(num);
      SetConsoleTextAttribute(hStdout, 10);
    }
    else // No color composite
    {
      SetConsoleTextAttribute(hStdout, 8);
    }
    std::cout << std::setw(10) << num;
  }
  // Reset color to white
  SetConsoleTextAttribute(hStdout, 15);
}


auto prompt_int_input() -> long unsigned int {
  long unsigned int input;
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


