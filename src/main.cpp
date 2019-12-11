/*
                              PINE 64 - C++ lib                                            1
 Copyright (c) 2017 Daniele Contarino <daniele.contatino@studium.unict.it>               i111i
 This library is based over work by:                                                  .ft  .;  .tf.
     Eric Ptak <trouch@trouch.com>                                                    ft. ,fft. .ff
     Stefan Mavrodiev @ OLIMEX LTD <support@olimex.com>                               1fffffffffff1
     Kamil Trzcinski <ayufan@ayufan.eu>                                           ,LLf. ,fffffff, ,LLL,
Permission is hereby granted, free of charge, to any person obtaining a copy of   ,LLLLLLLi   iLLLLLLL,
this software and associated documentation files (the "Software"), to deal in     ,LLL;  1LLLLLi  ;LLL,
the Software without restriction, including without limitation the rights to        :LLLLLLLLLLLLLLL:
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies    .LLL;  1LLLLLLLi  ;LLL
of the Software, and to permit persons to whom the Software is furnished to do    :LLLLLLL:   :LLLLLLL,
so, subject to the following conditions:                                           iLLf. :LLLLL, .LLL;
The above copyright notice and this permission notice shall be included in all       1LLLLLLLLLLLLLi
copies or substantial portions of the Software.                                          :LLLLL:
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                  L
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                    L
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "max7219_controller.h"
#include <iostream>

using namespace std;
using namespace Pine64;

int main(void) {
    auto *man = new Max7219Controller<4, 1>(PI_GPIO_14, PI_GPIO_15, PI_GPIO_18);

    man->setup();

    int x = 0;

    while (true) {

        man->clear();

        man->set(x, 4);
        x++;

        man->draw();

        man->delay(500);
    }

    cout << "Exiting....." << endl;
    return 0;
}
