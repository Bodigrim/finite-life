#!/usr/bin/awk -f

# Copyright 2012 Andrew "Bodigrim" Lelechenko <andrew.lelechenko@gmail.com>
#
# This file is part of Finite Life
#
# Finite Life is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This program is intended to aggregate statistical information about periods. Normally its input is a production of `undump` program. The following input format is expected:
# <board1> <period1>
# <board2> <period2>
# ...
#
# The current program calculates a number of appearences of each period and write output as following:
# <period1> <counts1>
# <period2> <counts2>
# ...
#
# Usage:
# cat life.undump | ./stat.awk
# or better
# cat life.undump | ./stat.awk | sort -n

{if($2>0) periods[$2]++}
END {for(i in periods) print i, periods[i]}