/**
 * G++-specific class takes an int and provides stream references.
 *
 * Copyright 2014 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _HPP_FDES_STREAM
#define _HPP_FDES_STREAM

#include <ext/stdio_filebuf.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

namespace fr {

  namespace socket {

    class fdes_stream {
      typedef std::shared_ptr<__gnu_cxx::stdio_filebuf<char> > filebuf_pointer;
      int fdes;
      filebuf_pointer buf_in;
      filebuf_pointer buf_out;

    public:

      // Hmm why not just make these public and let you use them
      // straight from the object?
      std::istream stream_in;
      std::ostream stream_out;

      fdes_stream(int fdes) :
        fdes(fdes),
        buf_in(std::make_shared<__gnu_cxx::stdio_filebuf<char > >(fdes, std::ios_base::in)),
        buf_out(std::make_shared<__gnu_cxx::stdio_filebuf<char > >(fdes, std::ios_base::out)),
        stream_in(buf_in.get()),
        stream_out(buf_out.get())
      {
      }

      fdes_stream(const fdes_stream &copy) = delete;

      ~fdes_stream()
      {
	close(fdes);
      }

      int get_fdes() const
      {
	return fdes;
      }

      std::istream& get_istream()
      {
	return stream_in;
      }

      std::ostream& get_ostream()
      {
	return stream_out;
      }
    };

  }

}

#endif
