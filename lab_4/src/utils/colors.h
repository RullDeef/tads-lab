#ifndef __COLORS_H_
#define __COLORS_H_

#define __CLR_START "\033["
#define __CLR_END "m"

#define CLR_MAKE(params) __CLR_START params __CLR_END

#define __CLR_NORM "0"
#define __CLR_BOLD "1"

#define __CLR_UNDERLINE "4"

#define __CLR_BLACK_FG      "30"
#define __CLR_RED_FG        "31"
#define __CLR_GREEN_FG      "32"
#define __CLR_YELLOW_FG     "33"
#define __CLR_BLUE_FG       "34"
#define __CLR_MAGENTA_FG    "35"
#define __CLR_CYAN_FG       "36"
#define __CLR_WHITE_FG      "37"

#define __CLR_BLACK_BG      "40"
#define __CLR_RED_BG        "41"
#define __CLR_GREEN_BG      "42"
#define __CLR_YELLOW_BG     "43"
#define __CLR_BLUE_BG       "44"
#define __CLR_MAGENTA_BG    "45"
#define __CLR_CYAN_BG       "46"
#define __CLR_WHITE_BG      "47"

#define __CLR_BR_BLACK_FG   "90"
#define __CLR_BR_RED_FG     "91"
#define __CLR_BR_GREEN_FG   "92"
#define __CLR_BR_YELLOW_FG  "93"
#define __CLR_BR_BLUE_FG    "94"
#define __CLR_BR_MAGENTA_FG "95"
#define __CLR_BR_CYAN_FG    "96"
#define __CLR_BR_WHITE_FG   "97"

#define __CLR_BR_BLACK_BG   "100"
#define __CLR_BR_RED_BG     "101"
#define __CLR_BR_GREEN_BG   "102"
#define __CLR_BR_YELLOW_BG  "103"
#define __CLR_BR_BLUE_BG    "104"
#define __CLR_BR_MAGENTA_BG "105"
#define __CLR_BR_CYAN_BG    "106"
#define __CLR_BR_WHITE_BG   "107"

#define CLR_RESET           CLR_MAKE(__CLR_WHITE_FG ";" __CLR_NORM)
#define CLR_EMPH            CLR_MAKE(__CLR_UNDERLINE ";" __CLR_BOLD)

#define CLR_BLACK           CLR_MAKE(__CLR_BLACK_FG)
#define CLR_RED             CLR_MAKE(__CLR_RED_FG)
#define CLR_GREEN           CLR_MAKE(__CLR_GREEN_FG)
#define CLR_YELLOW          CLR_MAKE(__CLR_YELLOW_FG)
#define CLR_BLUE            CLR_MAKE(__CLR_BLUE_FG)
#define CLR_MAGENTA         CLR_MAKE(__CLR_MAGENTA_FG)
#define CLR_CYAN            CLR_MAKE(__CLR_CYAN_FG)
#define CLR_WHITE           CLR_MAKE(__CLR_WHITE_FG)

#define CLR_BR_BLACK        CLR_MAKE(__CLR_BR_BLACK_FG)
#define CLR_BR_RED          CLR_MAKE(__CLR_BR_RED_FG)
#define CLR_BR_GREEN        CLR_MAKE(__CLR_BR_GREEN_FG)
#define CLR_BR_YELLOW       CLR_MAKE(__CLR_BR_YELLOW_FG)
#define CLR_BR_BLUE         CLR_MAKE(__CLR_BR_BLUE_FG)
#define CLR_BR_MAGENTA      CLR_MAKE(__CLR_BR_MAGENTA_FG)
#define CLR_BR_CYAN         CLR_MAKE(__CLR_BR_CYAN_FG)
#define CLR_BR_WHITE        CLR_MAKE(__CLR_BR_WHITE_FG)

#define CLR_BR_BLACK_U      CLR_MAKE(__CLR_BR_BLACK_FG ";" __CLR_UNDERLINE)
#define CLR_BR_RED_U        CLR_MAKE(__CLR_BR_RED_FG ";" __CLR_UNDERLINE)
#define CLR_BR_GREEN_U      CLR_MAKE(__CLR_BR_GREEN_FG ";" __CLR_UNDERLINE)
#define CLR_BR_YELLOW_U     CLR_MAKE(__CLR_BR_YELLOW_FG ";" __CLR_UNDERLINE)
#define CLR_BR_BLUE_U       CLR_MAKE(__CLR_BR_BLUE_FG ";" __CLR_UNDERLINE)
#define CLR_BR_MAGENTA_U    CLR_MAKE(__CLR_BR_MAGENTA_FG ";" __CLR_UNDERLINE)
#define CLR_BR_CYAN_U       CLR_MAKE(__CLR_BR_CYAN_FG ";" __CLR_UNDERLINE)
#define CLR_BR_WHITE_U      CLR_MAKE(__CLR_BR_WHITE_FG ";" __CLR_UNDERLINE)

#define CLR_BLACK_U         CLR_MAKE(__CLR_BLACK_FG ";" __CLR_UNDERLINE)
#define CLR_RED_U           CLR_MAKE(__CLR_RED_FG ";" __CLR_UNDERLINE)
#define CLR_GREEN_U         CLR_MAKE(__CLR_GREEN_FG ";" __CLR_UNDERLINE)
#define CLR_YELLOW_U        CLR_MAKE(__CLR_YELLOW_FG ";" __CLR_UNDERLINE)
#define CLR_BLUE_U          CLR_MAKE(__CLR_BLUE_FG ";" __CLR_UNDERLINE)
#define CLR_MAGENTA_U       CLR_MAKE(__CLR_MAGENTA_FG ";" __CLR_UNDERLINE)
#define CLR_CYAN_U          CLR_MAKE(__CLR_CYAN_FG ";" __CLR_UNDERLINE)
#define CLR_WHITE_U         CLR_MAKE(__CLR_WHITE_FG ";" __CLR_UNDERLINE)

#endif
